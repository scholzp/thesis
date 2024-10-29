## [Towards Self-monitoring Enclaves: Side-Channel Detection Using Performance Counters (p120-136)](../sources/secure_it_systems.pdf)

* looks at class of Load Value Injection attacks (LVI)
    * inject data to enclave to provoke leak
* Although new versions of hardware and TEE implementations mitigate known attacks, the authors argue that TEE should be able to detect when they are under attack
* studies the possibility for enclaves to be self monitoring
* mentions control channel attacks executed by repeatly causing page faults
* identify cache-based side channel as a high risk vector
* Mention TSX as one possible mitigation (but TSX is abandoned)
* uses [SGX-step](https://github.com/jovanbulck/sgx-step) to execute PoC attacks
    * attacks still contain major steps and therefore are enough to study effects on HPC
* Three variants of LVI attacks: 
    * Store buffer injection
    * transient control flow hijacking
    * L1D cache injection
* High page fault count with low minor/major count is empirical found to be a good indicator of an LVI attack 
* Used together with cache misses and cache references
* Events finally used: total number of instructions, LLC misses, LLC references, page faults (total, minor, minor)
* Attack identification through thresholds 
    * highest number of PF for applications 2322 vs. 10.000 per attack sample
* Attacks can be detected 
    * L1D variant needs to run longer before being detected, overall more complicated to detect
* authors conclude that at the moment no self monitoring in SGX is possible
* the define four requirements for self monitoring
    1) interface from which measurement routines are accessed need to be available
    2) confidentiality, integrity, availability must be upheld even if untrusted environment is compromised
    3) measurements must be able to perform for events happening on the inside and outside of the enclave
    4) measurements performed from untrusted environment must not be affected by any evnets from within the enclave
* SGX only fulfills 4
    * SGX doesn't allow HPC measurements from within enclaves
    * HPCs are not protected 