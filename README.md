<p align="center">
  <a href="https://github.com/CMU-SAFARI/Pythia">
    <img src="Hybrid-Prefetcher-FSM-RL logo.png" alt="Logo" width="200" height="100">
  </a>
  <h3 align="center">A Customizable Hardware Prefetching Framework Using Online Reinforcement Learning
  </h3>
</p>

<p align="center">
   
   
</p>

<!-- ## Update
### Aug 13th, 2021
It has been brought to our attention that the Ligra and PARSEC-2.1 traces required to evaluate the artifact are not correctly getting downloaded using the `download_traces.pl` script. For now, we ask the reader to download **all** Ligra and PARSEC-2.1 traces (~10 GB) by (1) clicking on the mega link (see [Section 5](https://github.com/CMU-SAFARI/Pythia#more-traces)), and (2) clicking on "Download as ZIP" option. We are working with megatools developer to resolve the issue soon.  -->

<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#what-is-Hybrid-Prefetcher-FSM-RL">What is Hybrid-Prefetcher-FSM-RL?</a></li>
    <li><a href="#about-the-framework">About the Framework</a></li>
    <li><a href="#prerequisites">Prerequisites</a></li>
    <li><a href="#installation">Installation</a></li>
    <li><a href="#preparing-traces">Preparing Traces</a></li>
    <ul>
      <li><a href="#more-traces">More Traces</a></li>
    </ul>
    <li><a href="#experimental-workflow">Experimental Workflow</a></li>
      <ul>
        <li><a href="#launching-experiments">Launching Experiments</a></li>
        <li><a href="#rolling-up-statistics">Rolling up Statistics</a></li>
      </ul>
    </li>
    <li><a href="#code-walkthrough">Code Walkthrough</a></li>
       <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>Hybrid-Prefetcher-FSM-RL

## What is Hybrid-Prefetcher-FSM-RL?

>In particular, our novelty here is to cast our hybrid memory prefetching framework with a lightweight Finite State Machine (FSM) (in Pythia) for simple, repetitive memory accesses (stride or next-line access), and a more adaptive Reinforcement Learning (RL) agent (from Pythia) for more complex or irregular accesses. The FSM serves as a sped-up, low-cost decision-layer that infers access patterns using a rule-based approach and micro-cache respectively and issues prefetches immediately and preemptively when confident. In scenarios where the pattern is too complex or uncertain, the system dynamically reverts to a RL engine that utilizes the learned Q-values to make improved predictions. This hybrid method minimizes superfluous learning of obvious patterns, increases efficiency, and generalizes more well across workloads with regular and irregular memory behaviors.
 Hybrid-Prefetcher-FSM-RL is presetend at MICRO 2021.

>
## About The Framework

Our hybrid prefetcher was implemented on top of the Pythia framework integrated into the [ChampSim simulator](https://github.com/ChampSim/ChampSim). To enable hybrid functionality, we significantly extended the existing prefetcher pipeline in ChampSim by incorporating a lightweight rule-based FSM module along with reinforcement learning (RL) logic adapted from Pythia. The FSM was designed from scratch using instruction and data-flow based classification, while the RL component reuses the core logic of Pythia’s Q-learning-based decision-making. Our modifications also include the design of a classification table, FSM cache, and updates to tracking structures like the evaluation queue. All modules were integrated to maintain compatibility with prior prefetchers such as:

* Stride [Fu+, MICRO'92]
* Streamer [Chen and Baer, IEEE TC'95]
* SMS [Somogyi+, ISCA'06]
* AMPM [Ishii+, ICS'09]
* Sandbox [Pugsley+, HPCA'14]
* BOP [Michaud, HPCA'16]
* SPP [Kim+, MICRO'16]
* Bingo [Bakshalipour+, HPCA'19]
* SPP+PPF [Bhatia+, ISCA'19]
* DSPatch [Bera+, MICRO'19]
* MLOP [Shakerinava+, DPC-3'19]
* IPCP [Pakalapati+, ISCA'20]

Most of the  prefetchers (e.g., SPP [1], Bingo [2], IPCP [3]) reuse codes from [2nd]() and [3rd]() data prefetching championships (DPC). Others (e.g., AMPM [4], SMS [5]) are implemented from scratch and shows similar relative performance reported by previous works.

## Prerequisites

The infrastructure has been tested with the following system configuration:
  * G++ v6.3.0 20170516
  * CMake v3.20.2
  * md5sum v8.26
  * Perl v5.24.1

## Installation

0. Install necessary prequisites
    ```bash
    sudo apt install perl
    ```
1. Clone the GitHub repo
   
   ```bash
    git clone https://github.com/Capstone-Group-One/Hybrid-Prefetcher-FSM-RL.git
   ```
2. Clone the bloomfilter library inside Pythia home directory
   
   ```bash
   cd Hybrid_Prefetcher_FSM_RL
   git clone https://github.com/mavam/libbf.git libbf

   ```
3. Build bloomfilter library. This should create the static `libbf.a` library inside `build` directory
   
    ```bash
    cd libbf
    mkdir build && cd build
    cmake ../
    make clean && make
    ```
4. Build Pythia for single/multi core using build script. This should create the executable inside `bin` directory.
   
   ```bash
   cd Hybrid-Prefetcher-FSM-RL
   # ./build_champsim.sh <l1_pref> <l2_pref> <llc_pref> <ncores>
   ./build_champsim.sh multi multi no 1
   ```
   Please use `build_champsim_highcore.sh` to build ChampSim for more than four cores.

5. _Set appropriate environment variables as follows:_

    ```bash
    source setvars.sh
    ```

## Preparing Traces

> [Update: Dec 18, 2024] The trace will be downloaded in two phases: (1) all traces, except Ligra and PARSEC workloads, will be downloaded using the automated script, and (2) the Ligra and PARSEC traces needs to be downloaded manually from Zenodo repository mentioned below. 

1. Use the `download_traces.pl` perl script to download all traces, except Ligra and PARSEC.

    ```bash
    mkdir $PYTHIA_HOME/traces/
    cd $PYTHIA_HOME/scripts/
    perl download_traces.pl --csv artifact_traces.csv --dir ../traces/
    ```
> Note: The script should download **138** traces. Please check the final log for any incomplete downloads.

2. Once the trace download completes, please verify the checksum as follows. _Please make sure all traces pass the checksum test._

    ```bash
    cd $PYTHIA_HOME/traces
    md5sum -c ../scripts/artifact_traces.md5
    ```

3. Download the Ligra and PARSEC traces from these repositories:
    - Ligra: https://doi.org/10.5281/zenodo.14267977
    - PARSEC 2.1: https://doi.org/10.5281/zenodo.14268118

4. If the traces are downloaded in some other path, please change the full path in `experiments/MICRO21_1C.tlist` and `experiments/MICRO21_4C.tlist` accordingly.

### More Traces
* Our simulation infrastructure is completely compatible with all prior ChampSim traces used in CRC-2 and DPC-3. One can also convert the CVP-2 traces (courtesy of Qualcomm Datacenter Technologies) to ChampSim format using [the following converter](https://github.com/ChampSim/ChampSim/tree/master/cvp_tracer). The traces can be found in the follwing websites:
     * CRC-2 traces: http://bit.ly/2t2nkUj
     * DPC-3 traces: http://hpca23.cse.tamu.edu/champsim-traces/speccpu/
     * CVP-2 traces: https://www.microarch.org/cvp1/cvp2/rules.html

## Experimental Workflow
Our experimental workflow consists of two stages: (1) launching experiments, and (2) rolling up statistics from experiment outputs.

### Launching Experiments
1. To create necessary experiment commands in bulk, we will use `scripts/create_jobfile.pl`
2. `create_jobfile.pl` requires three necessary arguments:
      * `exe`: the full path of the executable to run
      * `tlist`: contains trace definitions
      * `exp`: contains knobs of the experiements to run
3. Create experiments as follows. _Please make sure the paths used in tlist and exp files are appropriate_.
   
      ```bash
      cd $PYTHIA_HOME/experiments/
      perl ../scripts/create_jobfile.pl --exe $PYTHIA_HOME/bin/perceptron-multi-multi-no-ship-1core --tlist MICRO21_1C.tlist --exp MICRO21_1C.exp --local 1 > jobfile.sh
      ```

4. Go to a run directory (or create one) inside `experiements` to launch runs in the following way:
      ```bash
      cd experiments_1C
      source ../jobfile.sh
      ```

5. If you have [slurm](https://slurm.schedmd.com) support to launch multiple jobs in a compute cluster, please provide `--local 0` to `create_jobfile.pl`

### Rolling-up Statistics
1. To rollup stats in bulk, we will use `scripts/rollup.pl`
2. `rollup.pl` requires three necessary arguments:
      * `tlist`
      * `exp`
      * `mfile`: specifies stat names and reduction method to rollup
3. Rollup statistics as follows. _Please make sure the paths used in tlist and exp files are appropriate_.
   
      ```bash
      cd experiements_1C/
      perl ../../scripts/rollup.pl --tlist ../MICRO21_1C.tlist --exp ../MICRO21_1C.exp --mfile ../rollup_1C_base_config.mfile > rollup.csv
      ```

4. Export the `rollup.csv` file in you favourite data processor (Python Pandas, Excel, Numbers, etc.) to gain insights.



## Code Walkthrough
> Hybrid-Prefetcher-FSM-RL was code-named Scooby (the mistery-solving dog) during the developement. So any mention of Scooby anywhere in the code inadvertently means Hybrid-Prefetcher-FSM-RL.

* The top-level files for Hybrid-Prefetcher-FSM-RL are `prefetchers/scooby.cc` and `inc/scooby.h`. These two files declare and define the high-level functions forHybrid-Prefetcher-FSM-RL (e.g., `invoke_prefetcher`, `register_fill`, etc.). 
* The released version of our hybrid prefetcher defines two key decision-making components: a rule-based FSM engine and a reinforcement learning engine (inherited from Pythia). These components differ in their design goals and operational scope. The FSM engine is implemented using static rules and pattern-matching logic (based on delta, PC, branch outcome, and cache status) to detect simple and repetitive memory access patterns with minimal overhead. On the other hand, the RL engine leverages Pythia’s QVStore infrastructure to learn adaptive strategies over time for more complex, irregular patterns. The integration between FSM and RL is handled by a classification mechanism that decides whether a memory access should be handled by FSM or fall back to RL. The implementation of the FSM logic is contained within the `prefetcher/` directory (see `fsm_prefetcher.h` and `scooby.cc`), while the RL engine and QVStore logic are reused from Pythia’s source files under the `src/` and `inc/` directories. This hybrid approach enables efficient use of both deterministic logic and learned behavior to improve prefetch accuracy while keeping area overhead predictable and bounded.
* `inc/feature_knowledge.h` and `src/feature_knowldege.cc` define how to compute each program feature from the raw attributes of a deamand request. If you want to define your own feature, extend the enum `FeatureType` in `inc/feature_knowledge.h` and define its corresponding `process` function.
* `inc/util.h` and `src/util.cc` contain all hashing functions used in our evaluation. Play around with them, as a better hash function can also provide performance benefits.





## Contact

Vikaas - ssvvikaas4u@gmail.com

## Acknowledgements
We acklowledge support from FDU-VANCOUVER.
