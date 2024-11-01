## [openHCL](https://openvmm.dev/guide/user_guide/openhcl.html)

* [OpenVMM](https://openvmm.dev/guide/user_guide/openvmm.html) is microsofts opensource VMM
* supports different platforms/architectures (Win(x86, aarch64), Linux(x86); mac(aarch64))
* Rust based
* First [opensource](https://github.com/microsoft/openvmm) [commit](https://github.com/microsoft/openvmm/commit/304657e9a9240e6124d4432f92e7c81ca17fa373) from October 3rd 2024
    * Development started in 2019 closed source
* Openvmm is the VMM for the underlying OpenHCL 
* TCB includes host OS
* OS offers drivers/API for accessing real hardware to OpenHCL
* Whole concept relies on [Virtual Trust Levels](https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/tlfs/vsm) (VTLs)
* VTL are consecutive, higher VTL employs higher priveledges
* OpenHCL runs in VTL2 (Secure kernel of Windows runs in VTL1, Normal NT kernel in VTL0)
* OpenHCL uses VM stack of Host OS
* Hypervisor protects Memory of VMs through Second Level Address translation (SLAT)
* VM are called partitions in OpenVMM context
* openHCL implements virtual CPU and secure devices such as vTPM, is base of Paravirtualization
* OpenHCL offers Hypercalls to modify VMs 
* If technologies like TDX and AMD-SEV are missing, a softwarebackked implementation called Virtual Secure mode (https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/tlfs/vsm) (VSM) is used
* VSM offer capabilities and enlightenments to host and guest partitions that offer an API to create an manage new partitions from within OS/ existing partitions through hypercalls
* vCPUs and memory is associated to each partition
* Partitions, vCPUs have only access to memory of same VTL
* Additional but outdated source: [BSI review](../sources/vsm_architecture_signed.pdf) 




