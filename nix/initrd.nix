# Nix derivation that creates a minimal initrd with tools from busybox.
# The initrd sets up a minimal typical Linux environment and inserts
# the debugcon out-of-tree kernel module.

{ pkgs
, lib
, kmod
, tee_kernel
}:

let
  kmodMod = "${kmod}/lib/modules/${kmod.kernel-version}/updates/kmod.ko";
  Tkernel = "${tee_kernel}";
in
pkgs.makeInitrd {
  contents = [{
    object = pkgs.writers.writeBash "init" ''
      set -eu
      export PATH=${lib.makeBinPath
        ([
           # Basic shell dependencies
           pkgs.bashInteractive
           pkgs.busybox
           # If you want, you can add other utilities here.
           # They might require more kernel features.
           # pkgs.fd
           pkgs.usbutils
           pkgs.tpm2-tools

        ])
      }

      mkdir -p /proc /sys /tmp /run /var
      mount -t proc none /proc
      mount -t sysfs none /sys
      mount -t tmpfs none /tmp
      mount -t tmpfs none /run

      # Insert the tee kernel module.
      cp ${Tkernel} /tmp/tee_kernel
      insmod ${kmodMod}

      #create device nodes
      mdev -s

      # Enter bash (the root shell)
      setsid bash

    poweroff -f
    '';
    symlink = "/init";
  }];
}

    #   # Create device nodes.
    #   mdev -s

    #   echo -n "/dev/debugcon: "
    #   ls /dev | grep -q debugcon && echo EXISTS || echo 'NOT FOUND'

    #   ${testAppBin}

    #   # Enter bash (the root shell)
    #   setsid cttyhack bash

    #   poweroff -f