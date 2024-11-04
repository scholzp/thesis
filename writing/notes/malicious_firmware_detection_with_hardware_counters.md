## [Malicious Firmware Detection with Hardware Performance Counters](../sources/tmscs16_firmware_detect_hpc.pdf)

* Attacker model: Firmware manipulation
* Analysis of computational control flow in firmware
* Measures a specific number of Hardware counters 
* For each control flow path their is a vector of HPC events that describes it
* Control flow modification through
    * ROP (return oriented programming)
    * stack buffer overflow
* Control flow integrity (CFI) should be enforced for detection of attacks
* OS or firmware loads and executes applications on embedded devices
* Root of trust formed by boot ROM, pre-loader and bootloader
    * Reside in read only part of the system
    * ConFirm embedded into bootloader which protects it from modification
* 3 components of ConFirm
    * insertion module that places checkpoints in the firmware
    * HPC handler 
    * Database to store valid HPC signatures of the firmware
* bootloader loads Firmware into memory and then handles control to ConFirm
* ConFirm replaces instructions with own code, so called checkpoints
    * jump instruction to trampoline
    * trampoline jumps to ConFirm core, handles HPC, returns to trampoline
    * trampoline executes replaced instruction, returns
* Two checkpoints from a control window
* After each check window, HPCs are reset
* HPC values are used to generate signature
* Signature is checked in the database
* Checkpoints are inserted randomly when rebooting, helps against attacker who tries to mask attack by modifying HPC accordingly
* Two phases: Offline and online:
    1) Offline phase generates signatures from clean firmware on the target device before deployment. Firmware is split into checkpoints and then measured for all possible code paths and check windows.
    2) Online phase: Reads values of HPC while device is running. Generates signature from HPC counter values and compares to database. If match found, continue. If no match found, take predefined actions
* HPC events must be robust and repeatable to be used for signature generation
* Two processors tested: ARM Cortex A15, PowerPC e300c3
* Best events show a variations of 0.72 - 1.25 (ARM )and 1.05 - 2.35 (powerPC) percent variation between runs in the offline phase
* Average derivation at 18.9% (ARM) and 16.7% (powerPC)
    -> there seems to be some inaccuracy
* Signature based technique performs good for firmware with simple control flow, but not for complex control flow software
* for better results a Support Vector machine is used
* Offload analysis to remote system to reduce load on system under test
* With properly set parameters the SVM is able to perform classification with a FP/ FN rate below 10%
