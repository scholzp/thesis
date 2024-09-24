The goal of the work is to elaborate how a secure element can be implemented in
software and how the minimal hardware requirements must be for a CPU to support
such a secure element. Such a software secure element is to be designed in a way
that it can run entirely in the CPU cache. It uses only core local cache for 
executing its task. This makes it immune to cache based side channel attacks 
(Not really because HT/SMT). 
With a small working set of data and instructions, the operation only uses the
local cache (not really L3 is inclusive of all other levels on intel). To ensure no secrets are evicted from cache into RAM, the 
implementation uses performance counters (How is this possible if other cores use the L3 too, can other cores read secrets in Cache?). Communication to service 
clients is
realized through shared memory. Such shared memory must be marked as uncacheable
to not infer with the performance counters. Another open question is how a 
isolated CPU can be reassigned to the operating system (what makes this better 
then hotplugging in Linux). Maybe we can do workload isolation? what the value
add?

Kann man ein SE in Software bauen, dass ähnliche eigenschaften wie Hardware support enklaven hat?


## Isolation of CPU cores for remote attestation in x86_64 systems

Remote attestation is a technique used to prove to an observing entitity, that 
a system or part on an system is in a state expected by the observer.
* TPM; TXT protect bootflow but not user software
* SGX runs software in an untrusted environment; e.g. Digital rigths management
* Seals software in so called enclaves -> encrpted memory regions
* Enclave proves through remote attestation 
    1. It's identity (= confidentiality)
    2. Contains code expected by the client (= integraty)
* Client only uploads secrets to enclave if enclave hash matches expected value
* Not secure against side channel attacks such as Sepctre like attacks
* Archetectural problems in Sonny Cove Microarchitecture discloses data through local APIC memoy mapped registers
* Idea: Mitigate side channels/architectural channels through isolation
* Utlimate goal: Run code on isolated CPU and react to data breach; Eliminate all cache related channels by isolation of
a dedicated core 
* In this work elaborate how single cores can be used to run isolated softeware environments
* Additional goals: use performance counters for confidentiality checks and maybe remote attestation 
* More: communicatie with a host system; 