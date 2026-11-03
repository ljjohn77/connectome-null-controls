# r/MachineLearning post

**Suggested flair:** `[D]` (Discussion) or `[R]` (Research)

**Submit as:** text post (self-post, not link post — link posts get filtered)

---

## TITLE (pick one, ≤300 chars)

**Option A (recommended — leads with the trap):**
> [D] I nearly abandoned a working approach because my separability criterion was wrong. Fisher Sb/Sw > 1.0 is not a valid threshold.

**Option B (leads with the negative results):**
> [D] Five pre-registered negative results on using a Drosophila connectome as a robot controller — and the three null models that made them trustworthy

**Option C (leads with the methodology):**
> [D] Staging localization: instead of "does it work?", ask "at which layer is the information lost?"

---

## BODY

```
Hi all. I spent a long time on a project that tried to use a whole-brain Drosophila
connectome (166,700 neurons, 25.58M synapses, LIF network) as the controller for a
29-DoF humanoid. The engineering worked. The neuroscience claim did not.

I'm posting because two methodology lessons cost me months, and neither is standard
practice in the connectome/neuromorphic literature.

---

**1. My separability criterion was wrong, and it almost killed the project.**

I used "Fisher discriminant ratio Sb/Sw > 1.0" to decide whether neural populations
encoded class information. It reported "inseparable" and I started writing up the
conclusion that the connectome carries no task information.

Under the null hypothesis, Sb/Sw does not have expectation 0. It has expectation

    (K-1) / (K(N-1))

which is 0.068 for K=4 classes, N=12 samples. So ">1.0" is not a threshold at the
noise floor — it demands near-zero within-class variance.

Measured on my own data: Sb/Sw = 0.409. That's 6x the null. One-way ANOVA gives
F=6.00, df=(3,44), p=0.0016. And the actual leave-one-out decoding accuracy for that
same data is 92%.

The criterion said "inseparable" about data that decodes at 92%.

Worse, it gets harder as dimensionality grows: I computed intra-class dispersion as
mean Euclidean distance to the centroid, which scales as sqrt(d). At 26 dimensions
that inflates the denominator by ~5x on pure noise.

The fix is not a better threshold. It's a permutation null: shuffle labels, recompute,
NPERM >= 1000, compare against the resulting p95.

---

**2. "100% accuracy" was not citable, and I didn't notice for a long time.**

At one point I had several representations all decoding at 100%. Then I ran the control
I should have run first: replace the neural pipeline with a random linear projection at
the same output width.

Random Gaussian projection: 100%.
Sparse random projection at 2% density: 100%.
Randomly selected neuron subset: 100%.

If a structure-free linear map hits the ceiling, then the ceiling doesn't tell you
anything about the structure you claim.

---

**What the project actually produced (all pre-registered):**

- Descending-neuron population carries ~0 bits of intent information. One group fires
  at 0.20 Hz with 2.5% inter-class variation (an input-independent floor); a second is
  active but constant across classes; a third sits on threshold.
- A hand-designed 3-group binary code covers only 2 effective classes (mutual
  information 0.313 bits of a 2-bit ceiling; per-trial codes overlap across all classes).
- A nonlinear readout is *worse* than linear: MLP 74.1% +/- 1.9 vs ridge 82.7% +/- 0.0,
  delta = -8.5% +/- 1.9, CI excludes 0. At 36k samples the nonlinear hypothesis is
  just over-parameterized.
- Motor-neuron channel rates: 4% / 3% inter-class variation. Also 0 bits.
- A frozen LIF network with no plasticity is fully deterministic — 44 identical-input
  trials produce bit-identical responses. No habituation to measure.

**What survived:** the connectome's *topology* does carry functional information, for
timing. Degree-preserving rewiring (keeps every neuron's in/out degree, only changes
who connects to whom) drops a measured 102.8 N punch contact to 0 N. An Erdos-Renyi
graph at matched size never triggers at all.

So the honest summary is: connectome = trigger, not classifier.

---

**What I put on GitHub** (MIT for code, CC BY 4.0 for docs):

https://github.com/ljjohn77/connectome-null-controls

- `tools/staging_probe.cpp` — per-stage LOO + permutation null + random-projection
  control. Zero dependencies, self-testing, includes a case that *must* fail so you can
  verify the criterion can actually reject.
- `docs/negative-results.md` — all five, with the specific number that killed each one.
- `docs/zero-model-controls.md` — degree-preserving rewiring, Erdos-Renyi, label
  permutation: what each one rules out.
- `docs/literature-survey.md` — 24 primary sources (2024-2026) on connectome-as-controller,
  each annotated with which *interface paradigm* it used. That annotation turned out to
  matter more than any accuracy number: "learned interface" results always produce
  behavior, which is exactly why they don't support connectome claims.

The staging method is the part I'd most want people to steal: instead of asking "does
the brain work?", measure separability at every stage with the same decoder, and find
the layer where it collapses. In my case information arrived at the descending neurons'
synaptic current at 100% and left their spike output at 82.7%. That reframed the whole
problem from "connectome is useless" to "spike transduction is lossy" — completely
different course of action.

Happy to go into detail on any of it.
```

---

## Posting notes

- **Do not** add "please star / please share" — instant removal on r/ML.
- **Do not** cross-post the same text to multiple subs on day 1.
- The `[D]` flair generally gets more substantive replies than `[R]` for negative results.
- Best posting window: weekday 13:00–16:00 UTC.
- If a mod asks about self-promotion: this is not a product, it's a null-result
  methodology release with no commercial angle. That's usually fine.
