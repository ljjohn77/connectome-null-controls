# Zero-Model Controls for Connectome-Constrained Controllers

If your connectome-constrained network achieves a behavior, the null hypothesis is not
"nothing happens" — it is **"any network of the same size would achieve it."**
Three null models are needed, and each rules out a different alternative explanation.

---

## The three null models

### 1. Degree-preserving rewiring (Maslov–Sneppen)

Swap edge targets while keeping every neuron's in-degree and out-degree distribution
intact. Only *who connects to whom* changes.

**Rules out:** "the result comes from the degree distribution, not the topology."

**Usage:**
```cpp
// C++ — see tools/staging_probe.cpp for the swap loop
std::mt19937_64 rng(42);
for (size_t s = 0; s < edges.size(); ++s) {
    size_t i = rng() % edges.size(), j = rng() % edges.size();
    if (edges[i].pre == edges[j].post || edges[j].pre == edges[i].post) continue;
    std::swap(edges[i].post, edges[j].post);
}
// then recompute the file checksum — a stale checksum makes the loader
// reject the file, which is correct behaviour but easy to misread as a bug
```

**Measured on our pipeline:**

| Brain | Trigger time | Contact frames | Peak force |
|---|---|---|---|
| Real connectome | @36 ms | 9 | **102.8 N** |
| Degree-preserving rewiring | @88 ms | 0 | 0.0 N |
| Erdős–Rényi | never | 0 | 0.0 N |

Degree-preserving rewiring alone destroys the behavior (102.8 N → 0 N). So the
contribution is topological.

### 2. Erdős–Rényi random graph

Same neuron count, same edge count, edges placed uniformly at random. Destroy degree
structure as well as topology.

**Rules out:** "any sparse random graph of this size would do."

**Measured:** the ER graph **never triggers**. The behavioral pathway requires both
the topology and the degree structure.

### 3. Label permutation (for any decoding claim)

Shuffle trial labels, recompute the decoder, NPERM ≥ 1000 times. This is the null for
*decodability*, not for *behavior*.

**Rules out:** "the classifier exploits sample count or class imbalance."

**Measured example:** DN spike counts give LOO 74.1% with p = 0.0010 against the
label-permutation null — the DN space is genuinely decodable — yet DN group rates carry
0 bits of family information (`docs/negative-results.md` N-1). Both statements are true
and neither substitutes for the other. Reporting only the first is how a 0-bit result
gets mistaken for a 74% success.

---

## The control we did NOT run, and why it matters

**Random linear projection at the same output width.**

We measured that random Gaussian projection, sparse random projection (2% density), and
random neuron subsets **all reach 100%** on our data. This was discovered late, and it
retroactively invalidated every "100% accuracy" number the project had produced.

`tools/staging_probe.cpp` now runs this control automatically:

```
separable (orthogonal, sigma=0.3)   acc=1.000  => PASS
pure noise (must FAIL)              acc=0.170  => FAIL (correct)
random projection of same data      acc=0.993  => also passes -> your structure must beat THIS
```

**The rule:** a structure-specific claim requires
`accuracy(your_structure) > accuracy(random_projection)`.
If both hit the ceiling, the task is too easy to distinguish anything — you need a
harder task or a different measurement, not a better decoder.

---

## Literature context

The connectome-as-controller literature has a control problem, and it is openly
acknowledged:

- **"Digital sphinx"** (Brunton & Tuthill, 2026) wired a *C. elegans* connectome to a
  *Drosophila* body and trained an artificial network to map motor neuron activity to leg
  actuators with deep RL. The result: "highly realistic fly walking — yet it is
  biologically meaningless." Behavioral fidelity without biological fidelity.
  `doi:10.64898/2026.03.20.713233`

- **FlyControl** (THU) reports the connectome prior buys speed but pays a **5.5× collapse
  rate** on the ant task (38.6% vs 7.0% failure for an MLP baseline). A prior that
  degrades robustness is not automatically a win.
  `thu-neuroml.github.io/FlyControl/`

- **Whole-CNS emulation** (Eon / Shiu, Zenodo 23004436) is titled, in part,
  *"behaviours that emerge from the connectome, and behaviours that do not."*
  The negative half is the informative half.
  `doi:10.5281/zenodo.23004436`

- **arXiv:2610.33881** restricts itself to two fixed rules joining brain to body
  (odors/looming drive sensory neurons; motor neurons move legs) precisely because
  learned interfaces can absorb all the credit.

A connectome result is only interpretable against a null that shares its size, degree
structure, and interface budget. Otherwise the claim is unfalsifiable.

---

## Checklist

- [ ] Degree-preserving rewiring control
- [ ] Erdős–Rényi control at matched size
- [ ] Label-permutation null (NPERM ≥ 1000) for every decoding claim
- [ ] Random-projection control at matched width
- [ ] Criterion pre-registered before seeing results
- [ ] A mutation that must FAIL (proves the criterion can fail)
- [ ] Behavior ablation: disable the claimed pathway, direction of change predicted

The last two are the ones most often skipped. In our project both caught real errors:
the shuffle-label mutation exposed a false-positive decoder, and the disable-pathway
ablation is what turned a vague "the brain triggers" into a measured 102.8 N → 0 N.
