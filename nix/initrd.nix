# Nix derivation that creates a minimal initrd with tools from busybox.
# The initrd sets up a minimal typical Linux environment and inserts
# the debugcon out-of-tree kernel module.

{ pkgs
, lib
, kmod
, tee_kernel
, dbus_session_conf
, root_user
}:

let
  kmodMod = "${kmod}/lib/modules/${kmod.kernel-version}/updates/kmod.ko";
  Tkernel = "${tee_kernel}";
  session_conf = "${dbus_session_conf}";
  passwd = "${root_user}";
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
           pkgs.dbus
        ])
      }

      mkdir -p /proc /sys /tmp /run /var /etc
      mount -t proc none /proc
      mount -t sysfs none /sys
      mount -t tmpfs none /tmp
      mount -t tmpfs none /run

      # add root as user
      cp ${passwd} /etc/passwd
      cp ${pkgs.bashInteractive}/bin/bash /run/bash

      # create machine-id
      dbus-uuidgen >> /etc/machine-id

      # create the dbus session config
      mkdir /etc/dbus-1
      cp ${session_conf} /etc/dbus-1/session.conf

      # Insert the tee kernel module
      cp ${Tkernel} /tmp/tee_kernel
      insmod ${kmodMod}

      #create device nodes
      mdev -s

      # Enter bash (the root shell)
      su

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