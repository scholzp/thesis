# Convenient shell script that starts QEMU with the kernel, initrd,
# and out-of-tree Linux kernel module. The root shell of the kernel
# is connected to stdio.

{ kernel
, initrd
, start_address
, low_mem_size
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
    -accel kvm \
    -smp 6 \
    -append "nr_cpus=3 console=ttyS0 memmap=${low_mem_size}!${start_address} nox2apic loglevel=7 nokaslr" \
    -initrd ${initrdPath} \
    -serial stdio \
    -m 2G \
    -no-reboot \
    -no-shutdown \
    -debugcon file:./debugcon.txt \
    -d cpu_reset,int,cpu,guest_errors \
    -cpu host \
''