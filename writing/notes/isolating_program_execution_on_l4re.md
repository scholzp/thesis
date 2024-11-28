## [Isolating Program Execution on L4Re/Fiasco.OC](../sources/diplom.pdf)
* Talks about Trusted computing base (TCB)
* Tries to reduce TCB to Kernel + App
* enclave defines as isolated process
* When counting lines of code include library LoC
* library code might also be security relevant
* enclaves should only receive secrets once they are identified
* TPM verifies software stack
* implements Enma (Enclave manager)
* TCB includes Enma, Fiasco.OC, sigma0 and moe
* Related work grouped into different subjects
    * Software to remove Kernel from TCB
    * Hardware features
* Interrupltibility playes a big role if certain attacks are successful
* Kann sich nicht gegen Side channel verteidigen
* Quoting enclave (QE) for remote attestation (maybe we can also have two enclaves?)
* Private signing key of QE attests system state  and structure
* libtmps and [swtpm](https://github.com/stefanberger/swtpm) for developing tpm drivers
* Terminal and TAN generatir example applications
* Advisories against whom can not be defended
    * boot chain manipulation
    * Software with high priviledges (SMM, Monitor Mode, Secure World, 
        Hardware Devices, physical access in general, side channels ) 