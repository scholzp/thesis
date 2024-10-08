## [Isolating Program Execution on L4Re/Fiasco.OC]
[local](../sources/diplom.pdf)
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

## [Producing wrong data without doing anything obviously wrong](https://cs.pomona.edu/~michael/courses/csci190s21/papers/wrong_data.pdf)
[local](../sources/wrong_data.pdf)


## [Accuracy of Performance Counter Measurements](https://folia.unifr.ch/documents/318206/files/ITR0805.pdf)
[local](../sources/ITR0805.pdf)
* First study on accurary of performance counters
* explains PFC
* There seems to be an error rate that increases with duration when reading performance counters
-> might need to check if this is still the case

## [Non-Determinism and Overcount on Modern Hardware Performance Counter Implementations](https://icl.utk.edu/files/publications/2013/icl-utk-1246-2013.pdf)
[local](../sources/icl-utk-1246-2013.pdf)

* Performance Counters can be affected by Interrupts, PF, FPU handling and so on
* May not be important for us, because a lot of these are not part for us


### Misc
[Blog on Performance Measurement Units](https://easyperf.net/blog/2018/06/01/PMU-counters-and-profiling-basics)