## [CFIMon: Detecting Violation of Control Flow Integrity using Performance Counters](../sources/cfimon-dsn12.pdf)

* x86 Processors offer high precision modes
    * Intel: Precise Event Based Sampling (PEBS)
    * AMD: Instruction-based sampling (IBS)
    * PowerPC: Instruction Marking
* Branch tracing in intel processors: Branch Trace Store (BTS)
* AMD manual says for Interrupt based sampling the IP differs up to 72 instructions
* PEBS processes events in batches; atomic-freeze feature ensures exact IP of the instruction which triggered the Interrupt
* Branch trace store records all control transfer events, enabling monitoring of whole control flow of an application
* CFIMon has two phases: offline and online phase
* Offline Phase generates sets of legal target addresses:
    * Every branching instruction has either one or two targets
    * Through static analysis these possible target addresses are collected
    * For indirect jumps (e.g. jmp %eax) not possible through statically scanning binary
    * Indirect call can only point to start of function, those addresses can be obtained by scanning
* authors find that in Apache Web server through monitoring the binary at run time, that 94.7% of indirect jumps have one target address, 99.3% have less or equal to two target addresses. Only 0.1% have more then 10 different addresses
* every branch takes has three states: legal, illegal, suspicious
* branches considered suspicious are forwarded to a diagnose module
* diagnose module tolerates a fixed number of suspicious jumps for the last n branches
* 6.1% performance overhead on average
* A number of Control flow exploits were tested and found
* No false positives about a period of several days in daily use