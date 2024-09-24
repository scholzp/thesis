### [Intel SGX Explained](https://eprint.iacr.org/2016/086.pdf)
[local](2016-086.pdf)
* Secure remote execution: running software on a remote computer owned and maintained by an untrusted party (SRE)
* Homomorphic Encryption solved SRE, but has overhead
* SGX relies on software attestation
* Owner of computer can load any code to enclave, but remote user can refuse to load data into enclave which's signature does not match an expected value 
* In comparison to TPM and TXT, SGX reduced trusted computing base by covering only private data and code that executes on it.
* SGX employs reserved memory region called Processor Reserved Memory (PRM) and CPU protects PRM from all non-enclave excess
* untrusted systems software assigns pages to enclave, that it stores in enclave page cache (EPC)
* Initial enclave state is created by system software, which therefore knows it's state
#### physical attacks
* Port attacks via Device, which causes the victim to boot from device
* Debugports, attack via relayed Ringbus signal, GDXC
* Bus tapping attacks, DRAM Bus tapping expensive and complicated 
* Imaging attack: Read global secrets that are shared by all chips of a family with microscopy
* Power analysis attacks 
* Priviledged software attacks

#### Software attacks

### [Principles of Remote Attestation](https://web.cs.wpi.edu/~guttman/pubs/remote_attest.pdf)
[local](remote_attest.pdf)
* Makes claims about properties of a target and offers evidence to an appraiser over network
* Five core principles
    * deliver temporarely fresh evidience of the running systems (not just images)
    * comprehensive information about target should be available
    * target or owner of target should be able to constrains disclosure of information
    * mechanisms shoudl have explicit semantics to allow making decisions from serveral claims
    * attestation mechanism must be trustworthy
* Some kind of attacks on remote systems work by manipulating software
* Goal: Provide reliable evidience about state of software
* attestation protocol uses crypto
* target and measurement tools must be serperated from each other; target should not be able to infer with measurement tools ('cause of potential corruption; protect integrety of measurement tools)
* 

### [A Survey of Published Attacks on Intel SGX](https://arxiv.org/pdf/2006.13598)
[local](2006.13598v1.pdf)
* SGX protects against other software

### [Shielding applications from an untrusted cloud with Haven](https://www.usenix.org/system/files/conference/osdi14/osdi14-paper-baumann.pdf)
[local](osdi14-paper-baumann.pdf)

### [The Heisenberg Defense: Proactively Defending SGX Enclaves against Page-Table-Based Side-Channel Attacks](https://arxiv.org/pdf/1712.08519)
[local](1712.08519v1.pdf)

### [On the Challenges of Detecting Side-Channel Attacks in SGX](https://arxiv.org/pdf/2011.14599)
[local](2011.14599v2.pdf)

### [AEPICleak](https://aepicleak.com/aepicleak.pdf)
[local](aepicleak.pdf)
* Leak SGX enclave data/keys by directly accessing data when it's passed from L2 to LLC.
* Uses APCI (not x2APIC) memory mapped registers to access neighboring cache lines 