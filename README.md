# Staging Localization & Zero-Model Controls

**A methodology package for answering "does my brain-inspired controller actually work, and if not, where exactly does it fail?"**

Extracted from a 570-decision-card project that tried to drive a 29-DoF humanoid with a
*Drosophila* whole-brain connectome. The engineering succeeded. The neuroscience claim failed.
This package contains the parts that are worth reusing.

License: code **MIT**, documentation **CC BY 4.0**.

---

## The two problems this addresses

### 1. "It doesn't work" is not a result

Benchmarks routinely report that a connectome-constrained or brain-inspired controller
"achieves 3 behaviors" or "fails to emerge". Neither tells you what to do next.

**Staging localization** asks instead: *at which stage of the pipeline is information lost,
and by how much?* Same decoder at every stage, so the drop-off is attributable.

Measured on our pipeline (Drosophila connectome → Unitree G1, 5-class intent):

```
S0  joint angles, 29 dim                    100.0%
S1  encoder drive, 6370 dim                 100.0%
S2  sensory neuron spikes, 6370 dim         100.0%
S3a descending neuron synaptic current      100.0%   <- information arrives INTACT
S3  descending neuron spikes, 1314 dim       82.7%   <- destroyed by spike transduction
```

The actionable conclusion is **not** "the connectome doesn't carry information" but
"spike transduction loses it". Those imply completely different next steps.
Without staging, the project would have abandoned the wrong layer.

### 2. "100% accuracy" is usually not citable

We measured: random Gaussian projection, sparse random projection (2%), and random
neuron subsets **all reached 100%** on the same data. Any success claim without a
strong null control is indistinguishable from a random linear map.

The `random_project()` control in `tools/staging_probe.cpp` prints this explicitly:

```
separable (orthogonal, sigma=0.3)   acc=1.000  => PASS
pure noise (must FAIL)              acc=0.170  => FAIL (correct)
random projection of same data      acc=0.993  => also passes -> your structure must beat THIS
```

---

## The statistical trap we fell into

Our first separability criterion was **Fisher ratio `Sb/Sw > 1.0`**. It reported
"inseparable" and we almost abandoned the whole line of work.

The criterion was wrong. Under the null hypothesis,

```
E[Sb/Sw] = (K-1) / (K(N-1))          # 0.068 for K=4, N=12 — NOT zero
```

Measured `Sb/Sw = 0.409` is **6× the null** (one-way ANOVA F=6.00, df=(3,44), p=0.0016),
corresponding to **92% leave-one-out accuracy** — yet `>1.0` labels it inseparable.

It also inflates with dimensionality: `intra` computed as mean Euclidean distance to the
centroid scales as `√d`, so 26 dimensions inflate the denominator ~5.1×.

**Use permutation nulls, not fixed thresholds.** See `tools/staging_probe.cpp`.

---

## What's in here

| Path | Content |
|---|---|
| `tools/staging_probe.cpp` | Per-stage LOO + permutation null + random-projection control. Self-testing, no dependencies. |
| `docs/negative-results.md` | Five pre-registered negative results, each with the number that killed it. |
| `docs/zero-model-controls.md` | The three null models (degree-preserving rewiring, Erdős–Rényi, label permutation) and what each one rules out. |
| `docs/literature-survey.md` | 24 primary sources on connectome-as-controller (2024–2026), with the interface paradigm each one used. |

---

## Build and run

```bash
g++ -std=c++17 -O2 tools/staging_probe.cpp -o staging_probe
./staging_probe
```

No external dependencies. The self-test runs in under a second.

---

## Wiring it into your own pipeline

1. For each stage, build `X` (`n_samples × d_features`) and shared labels `y`.
2. Call `eval_stage(name, X, y, K, seed)` per stage.
3. The stage where accuracy collapses is your information-loss point.
4. **Mandatory:** run `random_project()` on the same input at the same width.
   If it matches, your result is not attributable to the structure you claim.

Pre-register the criterion before looking at results. All five negative results in
`docs/negative-results.md` were killed by criteria that were registered first.

---

## Attribution and upstream data

This package contains **no third-party data**. To reproduce the numbers you need:

- **MaleCNS v1.0** connectome (Janelia FlyEM, CC BY 4.0) — 166,700 neurons / 25.58M edges
  `https://storage.googleapis.com/flyem-male-cns/v1.0/connectome-data/flat-connectome/`
- **Unitree G1** model and policy assets (BSD-3-Clause) — `unitreerobotics/unitree_rl_gym`
- **GEAR-SONIC** weights (NVIDIA Open Model License) — not redistributed here
- **MuJoCo** (Apache 2.0) — `google-deepmind/mujoco`

Each negative result in `docs/negative-results.md` is identified by a stable
`N-1`..`N-5` label. Cite those labels, not internal identifiers.
