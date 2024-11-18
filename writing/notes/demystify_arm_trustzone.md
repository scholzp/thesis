## [Demystifying Arm TrustZone: A Comprehensive Survey](../sources/acmcsur2019-tz.pdf)

* Introduced to ARM Application (Cortex A) processors in 2004, later in Microcontroller SoC (Cortex M)
* Two protection domains:
    * Normal World
    * Secure World
* Secure Monitor on Cortex-A for Context Switching, Hardware on Cortex-M
* Strong hardware isolation

### TrustZone an Cortex-A
* Secure and Normal world
* Processor operates exclusivly in one of both world at a time
* Bit 33 "Not Secure(NS)" bit in Secure Configuration Registers (SCR)
* Dedicated Processor mode (Monitor mode) preserves Processor state at context switch
* Privileged Secure Monitor Call (SMC) bridges worlds
* Trust Zone address space controller (TZASC) can configure DRAM memory to be only accessible from Secure World
    * Instructions for TZASC only available in Secure World Mode
* Similar controller for ROM and SRAM with TrustZone Memory Adapter (TZMA)
* TZASC and TZMA are optional
* Tagged Cache to differ between worlds
* TrustZone Protection Controller (TZPC) controls access to Devices
* Generic Interrupt Controller (GIC) supports prioritizing of interrupts in normal/secure world
    * Prevent DoS attacks from normal world aiming on secure world
* Monitor mode sole entry point into secure world
* Switch from/to Monitor Mode from the other worlds though SMC instruction

### Microcontroller (Cortex-M)
* Similar to implementation in Application Cores
* Optimized for low power and switching latencies
* Memory-Map based: Non Secure Memory-> Normal World, Secure Memory -> Secure World
* No Monitor mode, but 3 instruction for bridging worlds with multiple secure function entry points
    * Secure Gateway (SG): Switch to secure world in first instruction of a secure entry point
    * Branch with Exchange from non-secure world (BXNS): branch/return from secure to non-secure software
    * Branch with link and Exchange from non-secure world (BLXNS): call non-secure functions from secure world
* Most of the register file is shared between worlds
* Secure world divided into secure and non-secure callable (NSC)

### TEE implementation
* Isolation though worlds: Rich Os in non secure world is unable to access TEE in secure world
* World state reflected on I/O Paths, NS bit can be propagated to IO Devices
* Devices can decide on NS bit how to interact with the system
* Special hardware component (TPM) can be used to enable Remote attestation
    * e.g. TPM verify TEE kernel/TEE application
    * Manges cryptographic keys
* Differ between TEE kernel and TEE service applications
    * TEE kernel: Implements basic OS functionality, manages (multiple) TEE applications, manage memory/enforce memory isolation for each TEE
    * TEE service: implement specific function with own memory management
