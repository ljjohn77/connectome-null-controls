# NeuroStars post

**Category:** `Connectomics` (or `Data analysis` / `Computational neuroscience`)

**Tags:** `connectome` `null-models` `reproducibility` `drosophila` `spiking-network`

---

## TITLE

> Five pre-registered negative results: using a whole-brain Drosophila connectome as a robot controller

*(alternate, if you want to emphasise methodology over the negative result:)*
> Staging localization + zero-model controls: how to tell whether a connectome-constrained model actually uses the connectome

---

## BODY

```
I want to share the negative half of a project, because the field's positive
half is well covered and the negative half keeps getting rediscovered
independently by different groups.

**Setup.** MaleCNS v1.0 (166,700 neurons / 25.58M synapses) instantiated as an
LIF network in C++, reading proprioception and driving a Unitree G1 humanoid
(29 DoF). Frozen connectome, hand-designed sensory encoder and motor readout.
Goal: does the connectome select which punch to throw?

**Result: no, not from neural activity — but yes, for timing.**

---

### The five negative results

All five had criteria registered *before* the measurement. That's the only
reason I trust them; several are the kind of result you can rationalise away
after the fact.

**N-1. Descending-neuron population carries ~0 bits of intent information.**
Of three named groups: DNg12 fires 2-3 spikes per 240 ms window with **2.5%
inter-family variation** (an input-independent floor — an all-zero-joint
condition and a 1.9 rad pose differ by 0.02 spikes); DNg11 is active at
13-14 Hz but *constant* across families; MDN sits on the decision threshold.
A linear readout of the full 1314-DN vector does reach 82.7% LOO, so the
space is "decodable" in a narrow sense — but the group-level features that
action selection would use are flat. Both statements are true; reporting only
the first is how a 0-bit result gets read as a 83% success.

**N-2. A hand-designed 3-group binary code covers 2 effective classes, not 7.**
χ²=17.35 (df=9, p=0.043) so it's above chance, but mutual information is
0.313 bits of a 2-bit ceiling, and per-trial codes overlap heavily across all
four families (one code appears in every family: 3/4/8/6 times). Aggregated
per family, exactly two classes separate.

**N-3. A nonlinear readout is worse than linear.**
MLP 256-128: 74.1% ± 1.9. Ridge: 82.7% ± 0.0. Δ = −8.5% ± 1.9, CI excludes 0.
At 36,000 samples the nonlinear hypothesis is over-parameterized. Controls all
behaved: random-weight MLP 16.7%, shuffled labels 18.9%, majority 20.0%
(chance 20%).

**N-4. Motor-neuron channel rates: also 0 bits.**
The MN layer is *not* silent — ~1,500 spikes/trial, ~300/708 MNs active,
100% of trials non-zero. But across families the 6-channel rates vary by
**4% and 3%**. Root cause is structural: the drive is broad-spectrum, so
flexor and extensor co-fire and cancel (Σ dir·rate ≈ 0.000, a defect recorded
before this test). Not a channel-selection problem.

**N-5. A frozen LIF network has no habituation dynamics.**
Identical input repeated 10×: early-window response 261, 261, 261, ... 261.
Ratio 1.000 (criterion ≤0.70 to call it decay). 44 trials, jitter=0.
Frozen LIF + no plasticity + no noise + no adaptation current = a
deterministic map. There is no mechanism for habituation, so the direction
was closed under its own pre-registered stop condition.

---

### The staging method (the part I'd most want others to reuse)

"Does the brain work?" is unactionable. Measure separability at every stage
with the *same* decoder instead:

    S0  joint angles, 29 dim                    100.0%
    S1  encoder drive, 6370 dim                 100.0%
    S2  sensory neuron spikes, 6370 dim         100.0%
    S3a descending neuron synaptic current      100.0%   <- arrives INTACT
    S3  descending neuron spikes, 1314 dim       82.7%   <- loss is HERE

Information arrives at the descending neurons' synaptic current intact and is
destroyed by spike transduction. That reframes the problem from "connectome
doesn't carry information" to "spike transduction is lossy" — different
literature, different fix, different paper.

If anyone has measured the same 100% → X% drop and found a way to move it,
I'd genuinely like to know. My ceiling scan (drive gain ×8, four time windows,
gaussian vs rate coding) tops out at 92.0%; nothing reached 100%.

---

### What survived: topology matters, for timing

Three-model null comparison on a closed loop that punches a bag:

| Brain | Trigger | Contact frames | Peak force |
|---|---|---|---|
| Real connectome | @36 ms | 9 | **102.8 N** |
| Degree-preserving rewiring | @88 ms (2.4× slower) | 0 | 0.0 N |
| Erdős–Rényi, matched size | never | 0 | 0.0 N |

Degree-preserving rewiring keeps every neuron's in- and out-degree exactly,
changing only *who connects to whom* — and that alone destroys the behavior.
So the contribution is topological, not a degree-distribution artifact. That's
a real, causal, falsifiable result, and it's the one I'd defend.

---

### The two traps that cost the most time

**1. Fisher Sb/Sw > 1.0 is not a valid separability threshold.**
Under the null, E[Sb/Sw] = (K-1)/(K(N-1)) = 0.068 for K=4, N=12 — not zero.
I measured 0.409 (6× null, ANOVA F=6.00, p=0.0016) which corresponds to **92%
LOO accuracy**, and the criterion called it "inseparable." Also, if you compute
intra-class dispersion as mean Euclidean distance to centroid it scales as √d
and inflates with dimensionality. Use permutation nulls.

**2. "100%" was not citable and I didn't notice for months.**
Random Gaussian projection, sparse random projection (2%), and random neuron
subsets **all reached 100%** on the same data. Without that control, a
structure-specific claim is indistinguishable from a random linear map.

---

### Code and writeup

https://github.com/ljjohn77/connectome-null-controls — MIT (code), CC BY 4.0 (docs)

- `tools/staging_probe.cpp`: per-stage LOO + permutation null + random-projection
  control. Zero dependencies, self-testing, deliberately includes a case that
  must fail.
- `docs/negative-results.md`: the five above, with the number that killed each.
- `docs/zero-model-controls.md`: the three null models and what each rules out.
- `docs/literature-survey.md`: 24 primary sources (2024–2026) annotated by
  **interface paradigm**:

| | Paradigm | Who does the work |
|---|---|---|
| P1 | connectome as information processor, no learning | the wiring |
| P2 | frozen connectome + learned interfaces | the adapters |
| P3 | connectome as structural prior | a trained policy |
| P4 | learned interface absorbs everything | the adapter (connectome decorative) |

That annotation changed how I read the literature. Reports of the form "the
connectome produces 3 behaviors" are almost always P1 with reflex-class
behaviors, or P4 where the interface did the work. Neither supports a claim
that the connectome computes. P2 is the only paradigm with a real positive
precedent — and even FlyControl reports 38.6% vs 7.0% episode failure against
an MLP baseline, so the prior buys speed and costs robustness.

---

Happy to discuss, and especially interested if anyone has:
- a connectome-constrained model where a *hand-designed* interface (not a
  learned one) produced measurable task-level information transfer;
- a measurement of the spike-transduction information loss in their own
  pipeline, to compare against my 100% → 82.7%;
- thoughts on whether P2 is viable for contact-rich tasks at all, since every
  P2 precedent I found is navigation/locomotion.
```

---

## Posting notes

- NeuroStars is small and technical — a long, specific post is welcome. No need to trim.
- If you get a reply from someone in the Ramdya / Tuthill / Shiu orbit, engage seriously;
  they have the closest prior work.
- The "especially interested if anyone has..." block is what converts a post into a
  discussion. Keep it.
