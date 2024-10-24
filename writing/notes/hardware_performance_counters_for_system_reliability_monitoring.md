## [Hardware Performance Counters for System Reliability Monitoring](../sources/hpc_for_reliability_monitoring.pdf)
* tries to detect anomalies with the use of Hardware performance counters (HPC) 
* Claims to be the first to use HPC for reliability monitoring
* different architectures (even of same ISA) offer different counter (events)
* create profiles of fault free program execution of chosen benchmarks
* use single stuck fault 
    * single fault in the circuit, stuck to either logical 0 or 1
* use event driven Gem5 simulator to inject faults
* gem5 executed in AWS VM
* Events measured:
    * instructions committed
    * number of function calls
    * number of integer instructions
    * number of load instructions
* reading HPC every 10ms (equivalent to 200,000 cycles)
* injection of fault lead to execeptions (PF, Invalid OpCode, kernel panic, 
Invalid Instruction pointer)
* Interval sampling allows tracing when program behaved different
* This way it the authors were able to discover faults and recovery from them 
* without timing data, one can only tell if a program terminated early
    * no difference between crash and terminating with error
