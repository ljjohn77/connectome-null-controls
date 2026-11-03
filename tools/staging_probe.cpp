// staging_probe.cpp — Staging Localization: find WHERE information is lost in a pipeline
//
// ═══════════════════════════════════════════════════════════════════════════
// THE PROBLEM THIS SOLVES
// ═══════════════════════════════════════════════════════════════════════════
//   "The neural network's output isn't usable" is an unactionable conclusion.
//   The useful question is: **at which stage is information lost, and how much?**
//
//   This tool evaluates separability at EVERY stage of a pipeline using the
//   SAME decoder (ridge/nearest-centroid). The drop-off localizes the bottleneck.
//
//   Measured example (Drosophila connectome -> Unitree G1 intent classification, 5 classes):
//
//     S0 joint angles, 29 dim            100.0%
//     S1 encoder drive, 6370 dim         100.0%
//     S2 sensory neuron spikes, 6370 dim 100.0%
//     S3a descending neuron synaptic current 100.0%  <- information arrives intact
//     S3  descending neuron spikes, 1314 dim  82.7%  <- destroyed by spike transduction
//
//   The conclusion is NOT "the connectome doesn't work" but "spike transduction
//   loses information" -- two completely different courses of action.
//
// ═══════════════════════════════════════════════════════════════════════════
// CRITERION DESIGN (this is the core contribution, more than the code)
// ═══════════════════════════════════════════════════════════════════════════
//   1. Leave-one-out (LOO) accuracy   -- not raw accuracy
//   2. Permutation null (shuffle labels, NPERM >= 1000) -> null p95
//   3. PASS = acc > null p95  AND  acc > 2*chance  AND  p < 0.05
//
//   DO NOT use the Fisher ratio Sb/Sw with a ">1.0" threshold:
//     Under the null hypothesis Sb/Sw has expectation (K-1)/(K(N-1)),
//     which is 0.068 for K=4/N=12 -- NOT zero.
//     Measured: Sb/Sw = 0.409 (6x the null, ANOVA p=0.0016) corresponds to
//     92% LOO accuracy, yet a ">1.0" threshold calls it "inseparable".
//
// ═══════════════════════════════════════════════════════════════════════════
// MANDATORY CONTROLS (without them, "100%" is not citable)
// ═══════════════════════════════════════════════════════════════════════════
//   Measured: random Gaussian projection / sparse random projection / random
//   neuron subsets ALL reach 100%. So any "my decoder hits 100%" claim without
//   a STRONG NULL CONTROL is indistinguishable from random projection.
//   The M1/M2 mutations below (identical input, shuffled labels) are the minimum.
//
// ═══════════════════════════════════════════════════════════════════════════
// USAGE
// ═══════════════════════════════════════════════════════════════════════════
//   This is a METHODOLOGY TEMPLATE. Wire in your own per-stage feature
//   extraction. The reusable parts are: per-stage same-decoder evaluation,
//   permutation null, mutation controls, and the criterion.
//
// License: MIT
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>
#include <string>
#include <vector>

namespace {

// -- Criterion constants (pre-register; do not tune after seeing results) --
const double CHANCE_RATIO_MIN = 2.0;   // accuracy must exceed 2x chance
const double PERM_P_MAX = 0.05;        // permutation test threshold
const int    N_PERM = 1000;            // number of permutations

// -- Leave-one-out nearest-centroid decoder --
//    X: n x d feature matrix (row-major); y: labels in 0..K-1
int loo_correct(const std::vector<std::vector<double>>& X, const std::vector<int>& y, int K) {
    const size_t n = X.size();
    if (n == 0 || X[0].empty()) return 0;
    const size_t d = X[0].size();
    std::vector<std::vector<double>> csum((size_t)K, std::vector<double>(d, 0.0));
    std::vector<int> ccnt((size_t)K, 0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < d; ++j) csum[(size_t)y[i]][j] += X[i][j];
        ++ccnt[(size_t)y[i]];
    }
    int correct = 0;
    for (size_t i = 0; i < n; ++i) {
        const int yi = y[i];
        int best = -1;
        double bestd = 1e300;
        for (int k = 0; k < K; ++k) {
            const int cnt = ccnt[(size_t)k] - (k == yi ? 1 : 0);
            if (cnt <= 0) continue;
            const double inv = 1.0 / (double)cnt;
            double s = 0.0;
            for (size_t j = 0; j < d; ++j) {
                const double c = (csum[(size_t)k][j] - (k == yi ? X[i][j] : 0.0)) * inv;
                const double e = X[i][j] - c;
                s += e * e;
            }
            if (s < bestd) { bestd = s; best = k; }
        }
        if (best == yi) ++correct;
    }
    return correct;
}

// -- Permutation test: returns p-value and null 95th percentile --
void perm_test(const std::vector<std::vector<double>>& X, const std::vector<int>& y, int K,
               int n_perm, unsigned seed, double& pval, double& q95, double& real_acc) {
    const int real_correct = loo_correct(X, y, K);
    real_acc = (double)real_correct / (double)X.size();
    std::vector<int> perm = y;
    std::mt19937_64 rng(seed);
    std::vector<int> nulls;
    nulls.reserve((size_t)n_perm);
    int ge = 0;
    for (int p = 0; p < n_perm; ++p) {
        std::shuffle(perm.begin(), perm.end(), rng);
        const int c = loo_correct(X, perm, K);
        nulls.push_back(c);
        if (c >= real_correct) ++ge;
    }
    std::sort(nulls.begin(), nulls.end());
    pval = (double)(1 + ge) / (double)(1 + n_perm);
    const size_t idx = std::min((size_t)n_perm - 1, (size_t)std::floor(0.95 * n_perm));
    q95 = (double)nulls[idx] / (double)X.size();
}

// -- Evaluate one stage against all three criteria --
struct StageResult {
    const char* name;
    double acc;
    double null_q95;
    double pval;
    bool pass;
};

StageResult eval_stage(const char* name, const std::vector<std::vector<double>>& X,
                       const std::vector<int>& y, int K, unsigned seed) {
    StageResult r{name, 0, 0, 1, false};
    if (X.empty() || X.size() != y.size()) return r;
    perm_test(X, y, K, N_PERM, seed, r.pval, r.null_q95, r.acc);
    const double chance = 1.0 / (double)K;
    r.pass = (r.acc > r.null_q95) && (r.acc > CHANCE_RATIO_MIN * chance) && (r.pval < PERM_P_MAX);
    return r;
}

// -- Strong null control: random projection to the same dimensionality --
//    If a random projection achieves the same accuracy, your result is not
//    attributable to the structure you claim.
std::vector<std::vector<double>> random_project(const std::vector<std::vector<double>>& X,
                                                size_t out_dim, unsigned seed) {
    if (X.empty()) return {};
    const size_t n = X.size(), d = X[0].size();
    std::mt19937_64 rng(seed);
    std::normal_distribution<double> g(0.0, 1.0 / std::sqrt((double)out_dim));
    std::vector<std::vector<double>> R(d, std::vector<double>(out_dim));
    for (size_t i = 0; i < d; ++i)
        for (size_t j = 0; j < out_dim; ++j) R[i][j] = g(rng);
    std::vector<std::vector<double>> out(n, std::vector<double>(out_dim, 0.0));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < out_dim; ++j) {
            double s = 0.0;
            for (size_t k = 0; k < d; ++k) s += X[i][k] * R[k][j];
            out[i][j] = s;
        }
    return out;
}

}  // namespace

int main(int argc, char** argv) {
    std::printf("=== Staging Localization ===\n");
    std::printf("Criterion: LOO acc > null p95 AND acc > %.0fx chance AND p < %.2f\n\n",
                CHANCE_RATIO_MIN, PERM_P_MAX);

    const int K = 5;      // number of classes
    const int N = 60;     // samples per class
    const int seed = 20260101;

    std::printf("-- Criterion self-test (synthetic data) --\n");

    // (1) Separable: orthogonal class centres, small noise -> must PASS
    std::vector<std::vector<double>> X_sep;
    std::vector<int> y_sep;
    {
        std::mt19937_64 rng(seed);
        std::normal_distribution<double> noise(0.0, 0.3);
        for (int k = 0; k < K; ++k)
            for (int i = 0; i < N; ++i) {
                std::vector<double> v((size_t)K, 0.0);
                for (int j = 0; j < K; ++j) v[(size_t)j] = (j == k ? 2.0 : 0.0) + noise(rng);
                X_sep.push_back(v);
                y_sep.push_back(k);
            }
        const auto r = eval_stage("separable (orthogonal, sigma=0.3)", X_sep, y_sep, K, (unsigned)seed);
        std::printf("  %-38s acc=%.3f  null_p95=%.3f  p=%.4f  => %s\n",
                    r.name, r.acc, r.null_q95, r.pval, r.pass ? "PASS" : "FAIL");
    }

    // (2) Pure noise -> must FAIL (proves the criterion can fail)
    {
        std::mt19937_64 rng(seed + 1);
        std::normal_distribution<double> noise(0.0, 1.0);
        std::vector<std::vector<double>> X;
        std::vector<int> y;
        for (int k = 0; k < K; ++k)
            for (int i = 0; i < N; ++i) {
                std::vector<double> v((size_t)K, 0.0);
                for (int j = 0; j < K; ++j) v[(size_t)j] = noise(rng);
                X.push_back(v);
                y.push_back(k);
            }
        const auto r = eval_stage("pure noise (must FAIL)", X, y, K, (unsigned)seed + 1);
        std::printf("  %-38s acc=%.3f  null_p95=%.3f  p=%.4f  => %s\n",
                    r.name, r.acc, r.null_q95, r.pval,
                    r.pass ? "PASS (FALSE POSITIVE!)" : "FAIL (correct)");
    }

    // (3) Random-projection control: shows the same accuracy is achievable
    //     by a structure-free linear map -> your structure is not exonerated.
    {
        const auto Xr = random_project(X_sep, (size_t)K, (unsigned)seed + 7);
        const auto r = eval_stage("random projection of same data", Xr, y_sep, K, (unsigned)seed + 7);
        std::printf("  %-38s acc=%.3f  => %s\n", r.name, r.acc,
                    r.pass ? "also passes -> your structure must beat THIS"
                           : "fails -> structure-specific");
    }

    std::printf("\n-- Wiring this into your pipeline --\n");
    std::printf("  1. For each stage, build X (n_samples x d_features) and shared labels y\n");
    std::printf("  2. Call eval_stage() per stage\n");
    std::printf("  3. The stage where acc collapses IS the information-loss point\n");
    std::printf("  4. MANDATORY: run random_project() on your input at the same width.\n");
    std::printf("     If it matches, your success is not attributable to your structure.\n\n");

    std::printf("  Do NOT use Fisher Sb/Sw > 1.0 as a threshold:\n");
    std::printf("    null expectation = (K-1)/(K(N-1)); 0.068 for K=4/N=12, not 0.\n");
    std::printf("    Measured Sb/Sw=0.409 (6x null, ANOVA p=0.0016) -> 92%% LOO accuracy,\n");
    std::printf("    yet a \">1.0\" threshold calls it inseparable.\n");

    (void)argc;
    (void)argv;
    return 0;
}
