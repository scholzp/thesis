# Nix derivation that creates a minimal initrd with tools from busybox.
# The initrd sets up a minimal typical Linux environment and inserts
# the debugcon out-of-tree kernel module.

{ pkgs
, lib
, kmod
}:

let
  kmodMod = "${kmod}/lib/modules/${kmod.kernel-version}/updates/kmod.ko.xz";
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

        ])
      }

      mkdir -p /proc /sys /tmp /run /var
      mount -t proc none /proc
      mount -t sysfs none /sys
      mount -t tmpfs none /tmp
      mount -t tmpfs none /run

      # Insert the debugcon kernel module.
      insmod ${kmodMod}

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