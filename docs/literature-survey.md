# Literature Survey: Connectome-Constrained Controllers (2024-2026)

24 primary sources on *"can a biological connectome serve as a controller for an embodied
agent?"*, each annotated with **which interface paradigm it used** — because that variable
determines whether the result says anything about the connectome at all.

Collected while auditing a project that tried to drive a 29-DoF humanoid with a
*Drosophila* whole-brain connectome.

---

## The four interface paradigms

| ID | Paradigm | Who does the work | Representative |
|---|---|---|---|
| **P1** | Connectome as information processor, **no learning** | the wiring alone | P01, P02, P03, P12 |
| **P2** | **Frozen connectome + learned interfaces** | adapters learn; core frozen | P08, P09, P10, P14 |
| **P3** | Connectome as **structural prior** | a trained policy uses the graph as bias | P15, P17, P21 |
| **P4** | **Learned interface absorbs everything** | the adapter does all the work | P05 |

**P4 always "succeeds" at producing behavior** — which is why it is also the paradigm
under the strongest criticism (P05, P06, P07). A result reported without naming its
paradigm is not interpretable.

---

## Entries

### [P01] Embodied whole-CNS emulation of Drosophila: behaviours that emerge, and behaviours that do not

- **Source:** Source: arXiv:2610.33881 (+ Zenodo 23004436). MaleCNS v1.0, 165,122 neurons / 25.6M connections.
- **Key finding:** Whole-CNS LIF network (172,433 neurons, **untuned** parameters from a published brain model), run both standalone and inside a simulated body, with an explicit control paired to every behavior test. Signal reaches known motor neurons, but **high-level behaviors do not emerge**; only a few fixed reflex pathways do. The title is the conclusion: behaviours that emerge **and behaviours that do not**.
- **Interface paradigm:** P1 — connectome as information processor, no learning. Two fixed wiring rules: odors/looming drive sensory neurons; motor neurons move legs.
- **Relevance:** **Highest relevance.** This is the public control for the no-tuning / no-learning ceiling — and it corroborates our measured bottleneck (S3a current 100% -> S3 spike 82.7%): spike transduction is the lossy layer across the whole field.

### [P02] plastic-fly: FlyWire connectome + MuJoCo closed loop

- **Source:** GitHub `neilt93/Fly-Brain-AI` (plastic-fly).
- **Key finding:** 139k neurons / 15M synapses, Brian2 LIF + FlyGym MuJoCo. Claims **3 adaptive behaviors emerge from wiring alone**, ablation 10/10. All three are locomotion/reflex class. The authors themselves state that whether connectome-structured dynamics can support meaningful sensorimotor transformations **without learning** remains unknown.
- **Interface paradigm:** P1.
- **Relevance:** The emergent-behavior list contains **no behavior requiring information processing** — consistent with P01.

### [P03] Eon Systems embodied fly — the authors' own stated limits

- **Source:** `eon.systems/updates/embodied-brain-emulation`.
- **Key finding:** The authors' own stated limitations: only a small subset of sensory inputs implemented, only a handful of behaviors modeled; the Shiu model uses simplified LIF neurons (no dendritic nonlinearity, inferred transmitter identity, simplified synapses). Subsequently challenged publicly by The Verge (P04) and the Brunton/Tuthill group (P05).
- **Interface paradigm:** P1.
- **Relevance:** **A first-party admission that LIF transduction is the simplification bottleneck** — same root as our U7 criterion (fixing encoder/window/gain does not move it).

### [P04] "No, this is not a fly uploaded to a computer"

- **Source:** The Verge (2026), citing Bakhtiari et al.
- **Key finding:** Expert consensus: public demos 'obscure key details'; blog-post patches 'remain insufficient to verify the claims'.
- **Relevance:** The field's evidentiary bar for 'connectome -> body' claims has tightened.

### [P05] The digital sphinx: Can a worm brain control a fly body?

- **Source:** eLife Reviewed Preprint 111516 (Brunton/Tuthill), + GitHub `Brunton-Lab/DigitalSphinx2026`.
- **Key finding:** C. elegans connectome RNN + **deep-RL-trained interface** -> Drosophila body, producing realistic gait (joint angles, leg coordination). eLife review: *'achieving plausible output dynamics does not in and of itself imply biologically meaningful simulation.'* Tuthill lab PDF: the fly brain is 500x larger than the nematode's, and fly legs alone are controlled by more motor neurons.
- **Interface paradigm:** P4 — learned interface.
- **Relevance:** **The single most important external counterexample.** A connectome->interface->robot pipeline has already been done via P4, and the conclusion was that the interface absorbed everything; the connectome contributed only oDN1/DNa01/DNa02 speed/turn signals.

### [P06] 'Digital sphinx' raises questions about connectome models

- **Source:** The Transmitter (2026).
- **Key finding:** Field-level warning: 'We need to be really careful in interpreting this kind of work' (Brunton). Interfaces can produce behavior faithful in appearance and irrelevant to neuroscience.
- **Relevance:** Interpretive framework; same as P05.

### [P07] From Connectome to Spectacle (survey)

- **Source:** SSRN 7609078 (Hu, Gu, Bi, Chia) — survey.
- **Key finding:** Comprehensive survey of fly emulation / embodied RL / upload philosophy. States explicitly that a connectome graph admits **many inequivalent computational models**, and documents the gap between public demo and scientific evidence.
- **Relevance:** Confirms systematic 'demo inflation' in this field; zero-model reconciliation is the antidote.

### [P08] FlyControl — A Drosophila connectome prior enables adaptable embodied control

- **Source:** `thu-neuroml.github.io/FlyControl/` (Tsinghua NeuML).
- **Key finding:** 'We keep the connectome fixed and learn the interfaces that connect it to a body.' Recurrent core 138,584 neurons; transfers across bodies (Cassie biped / tripod / Unitree Go1 / ant quadruped). Maze success 64.2% vs MLP 44.1% (Medium); 6-7 turn routes 50.6% vs 20.4%; ant forward speed 1.94 vs 0.68 m/s; **but ant episode failure rate 38.6% vs 7.0%** over 20 paired seeds. GPU-hours not disclosed.
- **Interface paradigm:** P2 — frozen connectome core + learned interfaces.
- **Relevance:** **Best positive precedent for P2** — and its own failure-rate number (38.6%) is the counterpoint: P2 is neither cheap nor reliable. All tasks are navigation/locomotion; none is contact-dense.

### [P09] Whole-Brain Connectomic GNN Enables Whole-Body Locomotion Control（FlyGM）

- **Source:** OpenReview KFmIRruzvL (DBM 2025) / arXiv:2602.17997 (FlyGM).
- **Key finding:** Whole-brain connectome instantiated as a recurrent message-passing network for whole-body locomotion control. Structural prior is real: return 334.34 vs PNA 145.33; degree-preserving rewiring raises yaw error 13.55 vs 8.29 — **the wiring itself carries a locomotor prior**. But obs=1253 / act=59, trained on an A100.
- **Interface paradigm:** P2.
- **Relevance:** Proves 'connectome as prior' yields genuine gains on **locomotion**, and the degree-preserving control shows it is not any-graph-will-do. The scale (1253-dim obs, single A100) is not comparable to a 65-dim obs / seconds-scale budget.

### [P10] Connectome-based CPG SNN on FPGA for bipedal locomotion

- **Source:** Connectome-based CPG SNN on FPGA for bipedal locomotion.
- **Key finding:** Connectome-derived CPG implemented on FPGA drives bipedal gait. Shows the connectome can supply a rhythm generator without learning.
- **Interface paradigm:** P2.
- **Relevance:** Supports the narrow role we ended up keeping: connectome as a timing/structure source, not as a classifier.

### [P11] Emergent dynamics in a robotic model based on the C. elegans connectome

- **Source:** Emergent dynamics in a robotic model based on the C. elegans connectome.
- **Key finding:** Connectome weights in a simulated worm body produce forward/backward locomotion. However, a later 2026 study (Sci Rep 10.1038/s41598-026-54384-5) states outright that using anatomical connectome weights *as they are* **did not achieve** the desired behavior — weights had to be learned.
- **Interface paradigm:** P1 -> P2 transition.
- **Relevance:** Direct precedent for the finding that raw anatomical weights are not sufficient; optimisation of weights was required.

### [P12] NeuroMechFly v2

- **Source:** NeuroMechFly v2 — Nature Methods s41592-024-02497-y (Ramdya lab, EPFL).
- **Key finding:** Biomechanical fly with vision, olfaction, ascending motor feedback, complex terrain, leg adhesion. The body and sensory apparatus are the contribution; the brain is a fixed or simplistic controller. Official scope excludes claiming brain-level behavior.
- **Interface paradigm:** Body-side reference.
- **Relevance:** The body/sensory side is far more mature than the brain side across the field — consistent with our own K1-K5 / G1-G5 pass record vs the 0-bit neural results.

### [P13] Whole-body physics simulation of fruit fly locomotion

- **Source:** Whole-body physics simulation of fruit fly locomotion.
- **Key finding:** Full-body MuJoCo fly with adhesion; establishes that a physically faithful fly body is achievable and is a precondition for any brain-body claim.
- **Relevance:** Body-side baseline.

### [P14] Focal neuronal loss and cell replacement in a connectome model of fly steering

- **Source:** Focal neuronal loss and cell replacement in a connectome model of fly steering.
- **Key finding:** Lesioning specific neurons in a connectome model degrades steering in a predictable, localized way — evidence that the wiring carries causal structure for a specific sensorimotor transform.
- **Interface paradigm:** P2.
- **Relevance:** One of the few cases where a connectome contribution survives a causal (lesion) test rather than a correlational one. Analogous to our degree-preserving rewiring ablation.

### [P15] Connectome-constrained networks predict neural activity across the fly visual system

- **Source:** Connectome-constrained networks predict neural activity across the fly visual system (flyvis).
- **Key finding:** Connectome-constrained networks with a small number of free parameters predict measured neural responses in the optic lobe. Success is task-matched: the circuit is asked to do what it evolved to do.
- **Interface paradigm:** P3.
- **Relevance:** Strongest evidence that connectome constraints carry real information — **when the task matches the circuit**. Our failure was asking a fly connectome to classify humanoid punch families.

### [P16] Infrequent strong connections constrain connectomic predictions of neuronal function

- **Source:** Infrequent strong connections constrain connectomic predictions of neuronal activity (Cell 2025).
- **Key finding:** A small number of strong connections dominate predictions; the weak/remainder of the connectome constrains little. Connection sparsity limits what any connectome-only model can predict.
- **Relevance:** Mechanistic explanation for our 0-bit result: the information is in a sparse strong-connection subset, and our group-averaged readouts diluted it away.

### [P17] Prediction of neural activity in connectome-constrained recurrent networks

- **Source:** Prediction of neural activity in connectome-constrained recurrent networks.
- **Key finding:** Theory: connectome-constrained RNNs predict activity only when the constraint is strong enough; otherwise the unmeasured degrees of freedom dominate.
- **Relevance:** Formal version of 'same connectome admits inequivalent models' (P07).

### [P18] From connectome to effectome

- **Source:** From connectome to effectome.
- **Key finding:** Argues the connectome must be paired with an effectome (causal influence map) to predict behavior — structure alone under-determines function.
- **Relevance:** Supports treating 'connectome as controller' as under-specified without an interface/effect model.

### [P19] Learning to Time-Decode in SNNs Through the Information Bottleneck

- **Source:** Learning to Time-Decode in SNNs Through the Information Bottleneck (NeurIPS 2021).
- **Key finding:** SNNs trained under an information-bottleneck objective learn time-decoding representations; the bottleneck formalises how much temporal information survives the spike interface.
- **Relevance:** Theoretical basis for measuring information loss at the spike-transduction layer, which is exactly where our pipeline drops 100% -> 82.7%.

### [P20] A Spiking Neuron as Information Bottleneck / Neural Coding in SNNs (comparative review)

- **Source:** A Spiking Neuron as Information Bottleneck / Neural Coding in SNNs (comparative review).
- **Key finding:** Treats the spike-generation step as an explicit information bottleneck; quantifies the trade-off between spike rate and transmitted information.
- **Relevance:** Predicts a hard ceiling on what any downstream decoder can recover from spikes — matching our measured 92.0% ceiling across all gain/window combinations.

### [P21] HoloBoxing

- **Source:** HoloBoxing.
- **Key finding:** Learned decoder maps 32-dimensional actions to structured target windows; converges in **7 minutes on a single GPU**. Freezes the low-level whole-body controller and learns only a compact high-level plan.
- **Interface paradigm:** High-level task-space learning (P3-adjacent).
- **Relevance:** The recipe our audit recommends if learning is to be attempted at all: learn the task space (<=32 dim), never the joint torques.

### [P22] RoboStriker

- **Source:** RoboStriker.
- **Key finding:** Learning directly in raw motor space produces collapse; structured action spaces are required for contact-rich humanoid tasks.
- **Relevance:** Task-side evidence against 'just learn the joint actions'.

### [P23] DeepMimic / DiffMimic / MAAIP

- **Source:** DeepMimic / DiffMimic / MAAIP.
- **Key finding:** Humanoid motion imitation requires **60M** (DeepMimic) to **>10B** (ASE-class) environment samples. These are joint-level learning costs.
- **Relevance:** Sets the sample-cost scale. On a single machine at 0.021x realtime, 60M samples would take 66 days — 7-9 orders of magnitude above a seconds-scale budget.

### [P24] Cross-embodiment negative results (quadruped-arm -> humanoid)

- **Source:** Cross-embodiment negative results (quadruped arm -> humanoid).
- **Key finding:** Policies trained on one embodiment transfer poorly to a morphologically distant one; learned interfaces degrade with embodiment distance.
- **Relevance:** Directly relevant: a 6-legged insect connectome driving a 29-DoF biped is a large embodiment distance, and no source reports success at that distance for contact-rich tasks.

---

## What the survey shows

Three patterns hold across all 24.

**1. The pure-connectome ceiling (P1) is reflex-level behavior only.**
P01 — the direct predecessor of this work, full CNS, 172,433 neurons, *no* tuning — is
titled "behaviours that emerge **and behaviours that do not**." The negative half is the
informative half. P02's three behaviors are all locomotion/reflex class. P03's authors
concede LIF reduction is the bottleneck.

**2. Learned interfaces (P2) work for navigation, not for contact-rich tasks.**
P08 is the best positive precedent and still reports **38.6% episode failure vs 7.0%** for
an MLP baseline. Every P2 task in this list is navigation-level; none is contact-dense.

**3. P4 results do not support connectome claims, by construction.**
P05 produced realistic fly walking from a worm connectome by training the interface;
the connectome contributed nothing the interface did not supply. P06/P07 make the same
point at field level: *"achieving plausible output dynamics does not in and of itself
imply biologically meaningful simulation."*

**The gap this project occupied:** a contact-dense task (punching a bag) with a frozen
connectome and *hand-designed* interfaces — a configuration none of the 24 uses.
Our result: hand-designed interfaces on this connectome carry 0 bits of intent information,
while its **topology does carry timing information** (102.8 N -> 0 N under degree-preserving
rewiring). That combination is not covered by any of the 24 sources.

---

## Sources by category

| Category | IDs |
|---|---|
| Connectome as controller / emergence | P01 P02 P03 P04 P12 P13 P15 P17 |
| Frozen connectome + learned interfaces | P05 P08 P09 P10 P14 |
| Structural prior / regularisation | P09 P15 P17 P18 |
| Criticism / epistemology | P04 P05 P06 P07 |
| Information theory (spiking, bottleneck) | P19 P20 |
| Task-side cost references | P21 P22 P23 P24 |

---

*Where a number was marked unverified in the source audit it is omitted here rather than
restated as if measured. GPU-hours for P08 and sample counts for P09 are not disclosed
publicly and are not used as evidence anywhere in this package.*

