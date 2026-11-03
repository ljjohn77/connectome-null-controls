# Five Pre-Registered Negative Results

Each result below was killed by a criterion **registered before the measurement**.
That is the point: the criterion existed first, so the failure is informative rather
than a post-hoc rationalization.

Context: a frozen *Drosophila* MaleCNS connectome (166,700 neurons / 25.58M synapses,
LIF network) reading proprioception and driving a 29-DoF Unitree G1 for punching.
Task: classify 5 posture families (or read intent from neural activity).

---

## N-1 — Descending-neuron population does not encode intent (0 bits)

**Claim tested:** the descending-neuron (DN) population carries which posture family is present.

**Criterion:** LOO nearest-centroid over 1314 DN spike counts, permutation null (NPERM≥1000),
PASS = acc > null p95 AND acc > 2×chance AND p < 0.05.

**Measured:**

| Representation | LOO 5-class | chance | null p95 | p | verdict |
|---|---|---|---|---|---|
| DN spikes, 1314 dim | 74.1% | 20% | — | — | — |
| DN spikes, exclusive subset (26 dim) | 79.2% | 20% | 37.5% | 0.0010 | PASS |
| Gaussian population code | 16.7% | 20% | 35.4% | 0.84 | FAIL |

Classification *within* the DN space succeeds (a linear readout reaches ~83%),
but the group-level information is 0 bits: DNg12 fires at 0.20 Hz with **2.5% inter-family
variation** (A=2.02, B=2.05, C=2.02, D=2.02, E_rest=2.00 spikes), i.e. an input-independent
floor. DNg11 is active (13-14 Hz) but **constant** across families. MDN sits on the threshold.
Two of the three groups carry no family information at all.

**Note:** these DN numbers are themselves not citable without a strong null —
random projection of the same input also reaches 100%. See `zero-model-controls.md`.

**Ref:** N-1 (this document). Implementation notes available on request.

---

## N-2 — A hand-designed 3-bit code covers only 2 effective classes

**Claim tested:** a 3-group binary signature (DNg12/DNg11/MDN) distinguishes 7 primitives.

**Criterion:** per-trial key consistency across families, plus permutation null.

**Measured:** χ²=17.35 (df=9, p=0.043), mutual information I(family;key)=**0.313 bits**
of a 2-bit ceiling. 4-class decoding from the key alone = **43.8%** (chance 25%).
Per-trial keys overlap severely — key=3 appears in all four families (3/4/8/6 times).
Aggregated per family, only **2 classes** separate (A+C vs B+D).

**Ref:** N-2 (this document).

---

## N-3 — A nonlinear readout is worse than linear

**Claim tested:** distinguishing the 5 families requires a spatiotemporal (nonlinear) transform.

**Criterion (pre-registered):** if `Δacc = MLP − Ridge` has a 95% CI containing 0,
the superposition hypothesis fails and the MLP direction stops.

**Measured:** MLP **74.1% ± 1.9** vs Ridge **82.7% ± 0.0** → Δ = **−8.5% ± 1.9**,
CI excludes 0. The MLP is *worse*, not merely equal.

Controls (all behave correctly, so the measurement is trustworthy): random-weight MLP 16.7%,
shuffled labels 18.9%, majority class 20.0%, uniform random 21.6% (chance 20%).

**Interpretation:** at 36,000 samples the nonlinear hypothesis is over-parameterized.
This is a sample-size statement, not an architecture statement.

**Ref:** N-3 (this document).

---

## N-4 — Motor-neuron channel rates carry no family information either

**Claim tested:** after DN fails, maybe identity lives downstream in motor neuron (MN) channels.

**Criterion:** same LOO + permutation criterion, applied to 6-channel MN rate vectors.

**Measured:** MN layer is not silent — ~1,500 spikes/trial, ~300/708 MNs active
(100% of trials non-zero). But across 5 families:

| Channel | Family range | Variation |
|---|---|---|
| ch0 | 13.85 – 14.45 Hz | **4%** |
| ch1 | 3.19 – 3.28 Hz | **3%** |

Zero usable bits. Root cause is structural: the drive is broad-spectrum, so flexor and
extensor co-fire and cancel (`Σ dir·rate ≈ 0.000`, a defect already recorded before this test).
This is not a channel-selection problem — changing the channel table does not change
the information content of a broad-spectrum upstream drive.

**Ref:** N-4 (this document).

---

## N-5 — A frozen LIF network has no habituation / novelty dynamics

**Claim tested:** the connectome could carry temporal information (habituation) that a
static angle readout cannot, since identical inputs repeat 10×.

**Criterion (pre-registered):** d1-1 = DN early-window response at repetition 10 must be
≤70% of repetition 1 (≥30% decay); d1-2 = recovery ≥50% after switching family;
d1-3 = permutation p<0.05. **Stop condition:** d1-1 unmet ⇒ close the direction,
do not tune parameters.

**Measured** (44 trials, jitter = 0 so angles are bit-identical):

```
trial  0..9  (family A): early-window nonzero DN = 261, 261, 261, 261, 261, 261, 261, 261, 261, 261

d1-1: 261 -> 261, ratio = 1.000  (criterion <= 0.70)  FAIL
d1-2: recovery = 0.000           (criterion >= 0.50)  FAIL
d1-3: slope = 0.0000, p = 1.0000 (criterion < 0.05)   FAIL
```

Frozen LIF + no plasticity + no noise + no adaptation current ⇒ a deterministic map.
Same input, same output, regardless of repetition count. There is no mechanism for
habituation to emerge. Direction closed per the pre-registered stop condition.

**Ref:** N-5 (this document).

---

## What survived

Not everything failed. Two things were confirmed with proper controls:

**The connectome's topology does carry functional information** — for *timing/triggering*,
not for classification. Three-model null comparison:

| Brain | Trigger | Contact frames | Peak force |
|---|---|---|---|
| Real connectome | @36 ms | 9 | **102.8 N** |
| Degree-preserving rewiring | @88 ms (2.4× slower) | 0 | 0.0 N |
| Erdős–Rényi random graph | never triggers | 0 | 0.0 N |

Degree-preserving rewiring keeps every neuron's in/out degree distribution intact and
only changes *who connects to whom* — and that alone is enough to destroy the behavior.
So the contribution is topological, not a degree-distribution artifact.

**The engineering is intact**: 29-DoF kinematics (10.0 mm drop over 10 s standing),
whole-body policy closed loop (root z ∈ [0.755, 0.793]), and a punch landing 102.8 N.

---

## The honest summary

> This project **demonstrated** that a frozen fly connectome's specific topology is
> sufficient to drive a real physical humanoid into a measurable punch (102.8 N,
> zero after rewiring).
>
> It **did not demonstrate**, and no longer claims, that the connectome can read intent
> from proprioception, distinguish action types, or produce temporal/novelty codes.
> Those three were 0 bits / fully deterministic across five independent measurements,
> consistent with 24 primary sources from 2024–2026.
