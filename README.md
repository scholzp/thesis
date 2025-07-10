# thesis
This is the source code of my University Diploma Thesis (equivalent to a masters
degree). It contains source code to build my thesis, as well as the parts of the
implementation described in the thesis.

## Written Part
From the directory's root change into writing/thesis:

```
cd writing/thesis
```

Follow the instructions of the README.md that you will find there.

## Implementation
This repository contains code for a Linux kernel module that can be used to run
the TEECore prototype. The TEECore prototype is included as a git submodule. To
fully initialize the submodule run 

```
git submodule init
```

### How to build
First you need to build TEECore as follows and copy the binary to the
implementation root (can be skipped if you use the kernel.elf32 image that is
part of this repository):

```
cd implementation/TEECore
make
cp build/phipsboot.elf32 ../kernel.elf32
```

Proceed with steps below.


#### Nix
Currently the only supported way.

From the directory's root run:

```
nix-build
```

This will build the following mandatory items:
* The initrd, containing the kernel module (in result)
* The Linux bzImage (in result-3)

These build outputs can be used to boot on bare metal. The initrd contains the
Linux user space application, the kernel module and TEECore. The bzImage is
required for booting Linux.

Additionally, the following build outputs are relevant:
* ISO-image that contains Linux with all modules
* The Linux kernel module binary (result-4)
* A Qemu setup that contains all modules (execute result-5)
* A Qemu setup that contains all modules and uses the swtpm (in result-6)
* A script that sets up a swtpm (in result-7)
* A Linux userspace application to interact with TEECore (result-8)