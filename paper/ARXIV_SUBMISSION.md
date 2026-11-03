# arXiv Submission Guide

## What you have

`arxiv-paper.tex` — complete, self-contained LaTeX source (no external figures, no
bibtex file needed, bibliography is inline).

Static checks already passed: braces balanced, all `\begin`/`\end` matched,
all `\cite` keys resolve, no unused bibitems.

## Compile it (no local LaTeX needed)

**Overleaf (recommended, 5 minutes):**

1. Go to https://www.overleaf.com/project
2. New Project → Blank Project
3. Delete the default `main.tex`, upload `arxiv-paper.tex`
4. Menu → Settings → Compiler: **pdfLaTeX**
5. Recompile. You should get ~7 pages, no errors.

If it compiles cleanly, download the PDF — that is your arXiv submission file.

**Or install BasicTeX locally (~100 MB):**

```bash
brew install --cask basictex
sudo tlmgr update --self
sudo tlmgr install authblk booktabs hyperref geometry listings xcolor
pdflatex arxiv-paper.tex && pdflatex arxiv-paper.tex
```

## Submit to arXiv

1. https://arxiv.org/submit
2. **Primary category:** `cs.LG` (Machine Learning)
   **Cross-list:** `q-bio.NC` (Neurons and Cognition)
   — the paper is primarily a measurement/methodology result, so `cs.LG` first.
3. **License:** CC BY 4.0 (matches the docs license; arXiv's default CC BY is fine
   and maximises reuse)
4. **Title:**
   ```
   Staging Localization and Zero-Model Controls for Connectome-Constrained Controllers: Five Pre-Registered Negative Results
   ```
5. **Authors:** `Lee`
6. **Abstract:** copy the `\begin{abstract}...\end{abstract}` body from the .tex,
   stripping LaTeX commands. Plain-text version below.
7. **Comments field:**
   ```
   7 pages, 2 tables, no figures. Code and documentation:
   https://github.com/ljjohn77/connectome-null-controls
   ```
8. Upload the PDF (and optionally the .tex source — arXiv accepts both).
9. **Endorsement:** if this is your first submission to `cs.LG`, arXiv may require an
   endorsement code from an established author. If prompted, the fastest route is
   usually to submit to `q-bio.NC` instead, which often has different endorsement
   requirements.

## Plain-text abstract (paste into the form)

```
Connectome-constrained models of neural control are increasingly reported to
"produce behaviours". We argue that two measurement practices are missing from
this literature, and we quantify what their absence costs. First, a widely used
separability criterion - the Fisher discriminant ratio Sb/Sw thresholded at 1.0 -
has a non-zero null expectation of (K-1)/[K(N-1)]; we show the criterion rejects
data that decodes at 92% accuracy. Second, decoder accuracies are routinely
reported without a strong null control; we show that random linear projections,
sparse random projections, and random neuron subsets all reach 100% on the same
data, so such numbers do not identify structure. We then report five
pre-registered negative results from a system that drives a 29-DoF humanoid with
a frozen Drosophila whole-brain connectome (166,700 neurons, 25.58M synapses,
LIF dynamics): descending-neuron and motor-neuron populations carry approximately
zero bits of task-family information; a nonlinear readout underperforms a linear
one; and a frozen LIF network exhibits no habituation. A staging measurement
localizes the information loss to spike transduction: information arrives at the
descending neurons' synaptic current at 100% and leaves their spike output at
82.7%. We also report what survives: the connectome's topology carries causal
timing information, with degree-preserving rewiring reducing a measured 102.8 N
punch contact to 0 N while an Erdos-Renyi graph of matched size fails to trigger
at all. We release the measurement toolkit and a 24-source survey annotated by
interface paradigm.
```

## Before you submit — three things to decide

1. **Do you want your real name or just "Lee"?** arXiv shows author names publicly
   and permanently (versions cannot be deleted, only replaced).
2. **Is the email `ljjohn@hotmail.com` the one you want on the paper?** It is
   currently in the `\thanks{}` footnote and will be visible to everyone.
3. **Endorsement.** If `cs.LG` requires an endorsement code, you need someone with
   prior arXiv submissions in that category. Alternatives: submit to `q-bio.NC`, or
   skip arXiv and lead with the Reddit/NeuroStars posts (which need no endorsement).

## Order of operations (suggested)

1. Compile on Overleaf, check the PDF reads correctly → this is the long pole
2. Post to NeuroStars (small, technical, will engage)
3. Post to r/MachineLearning (bigger, faster feedback, harsher)
4. Submit to arXiv once you have any feedback — the version you post can incorporate it

Posting to Reddit/NeuroStars first also gives you something to put in the arXiv
"comments" field ("discussed on ...").
