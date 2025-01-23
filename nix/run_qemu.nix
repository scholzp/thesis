# Convenient shell script that starts QEMU with the kernel, initrd,
# and out-of-tree Linux kernel module. The root shell of the kernel
# is connected to stdio.

{ kernel
, initrd
, start_address
, writeShellScript
, qemu
}:

let
  kernelPath = "${kernel}/bzImage";
  initrdPath = "${initrd}/initrd.gz";
  qemuBin = "${qemu}/bin/qemu-system-x86_64";
in
writeShellScript "run_qemu_demo" ''
  ${qemuBin} \
    -kernel ${kernelPath} \
    -cpu host \
    -accel kvm \
    -smp 4 \
    -append "nr_cpus=3 console=ttyS0 memmap=4K!${start_address}K" \
    -initrd ${initrdPath} \
    -serial stdio \
    -m 512M \
''