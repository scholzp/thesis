## [Detecting Spectre Attacks Using Hardware Performance Counters](../sources/10338791_hpc_spectre.pdf)

* Spectre:
    1) Variant one: exploits conditional branch prediction
    2) Variant two: exploits unconditional jump target prediction
    * Mispredicted loads leave data in cache -> microarchitectural behavior
    * Any observable effects of a wrong prediction might cause leakage of confidential data
* Try to detect Spectre attack proatvily
* Attacker tries to train the branch predictor to predict a certain branch by executing this path repeated
    * This reduces mispredictions below an expected value
    * This makes the preparation of the attack observable through the misprediction rate
* Arrays used for the attack have a certain size, which is used as branching condition and to access memory.
    * To utilize speculative execution to read secret data, the size of the arrays must not be cached (increase time in which speculative execution can be done)
    * Cache must be flushed to evict array sizes -> number of cache flushes increases while the attack is being run
    * Cache flushes are an observable side effect of a running Spectre attack
* Hardware performance counter to monitor microarchitectural events relevant to Spectre attacks
    * cache references
    * cache misses
    * branch instructions retired
    * branch mispredictions
* Use of sliding window to collect data
* Collect data of applications while not attack is running and while an attack is running, once every 100ms
* Data from both runs is labeled and used for Machine learning classifier training
* Algorithms used:
    * Support vector machines (SVM)
    * logistic regression 
    * Multilayer perceptron
* Weighted moving average used to smooth noise, more recent data got more weight
* Calculate average of consecutive decisions, if threshold is exceeded the conclusion is that an attack is running
* Decision each second, sliding window size of 10
* MLP performed best
* Different load scenarios tested to study influence on detection rate
    * CPU intensive workloads slow down attack and therefore reduce LLC miss rate
    * Memory intensive produce more LLC misses and increase detection rate
    * Combining CPU and memory intensive workloads show increased detection rate in comparison to CPU intensive only workloads
* Atomically executing certain parts of the attack increase success rate
    * Interrupting these parts decrease success rate
* Evasive strategy by reducing bandwidth of the attack
    * Sleep between atomic tasks (reduce bandwidth)
    * Sleep after all tasks completed (reduces bandwidth)
    * Insert instruction between atomic tasks (slow attack, manipulates HPCs)
    * Insert instructions after all tasks completed (slow attack, manipulates HPCs)
* MLP relatively resilent against evasion

