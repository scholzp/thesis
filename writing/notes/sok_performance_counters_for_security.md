## [SoK: The Challenges, Pitfalls, and Perils of Using Hardware Performance Counters for Security](../sources/10094405_sok_hpc_for_security.pdf)

* HPCs support sampling based on events
    * Polling: Read HPC on specified time points
    * PMI: Performance measurements Interrupt, sent when threshold on counter is exceeded
* Identify sources of inaccuracy
    * External sources: OS, scheduling, Microarchitectural behavior, memory layout and pressure can change between runs
    * Non-determinism: hardware Interrupts, periodic timers, depending on OS and other applications running on the system
    * Overcounting: certain events might be overcounted depedning on the processor implementation
    * Variations of tools: Implementation of tools have effect on the result counted by them. Measurements with different tools might not be comparable
* proper initialization:
    * Context switching must respect HPC values and settings
    * Interrupt handling: PMI must be properly handled
    * Process filtering upon PMI: Once configured, PMIs are triggered by all applications
    * Non-determinism: Use events that yield deterministic results
* Measurement methods:
    * Coarse measurement: logs the accumulated values for whole program
    * Snipped measurement: Code snippets are measured 
    * Fine grained: Use interrupts configured to fire every n event occurrence
* Determinism of HPC should be evaluated to determine measurement errors
* My own take: SMI might have an effect on HPC values
    * should be tested of System Management mode can be detected by HPC values changing 
* Non-determinism still existed in 2019 Skylake, Sandy Bridge and Haswell CPUs
* Compares values with [PIN](https://software.intel.com/sites/landingpage/pintool/docs/98484/Pin/html/index.html) debugging/code instrumentation tool of Intel and single stepping tool
* rep instruction prefix introduces inaccuracy
    * undercount of instructions, overcount of branch retired
    * 2-byte instructions have roughly twice the amount of one byte instructions
* string instructions produce under-/overcount
* Page faults introduce error to the counter
* authors conclude that these errors can be catastrophical for security applications
* PMI based approach can detect ROP attacks, albeit with a high False Positive rate
    * Noise reduces detection rate
* For malware detection authors find that HPC are to fragile 
* PMI introduce Skid (time between instruction triggering interrupt and delivering the interrupt)
* I should really test the determinsm of cache eviction events