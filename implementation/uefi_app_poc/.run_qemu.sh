#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# Helper script invoked by the Makefile.

QEMU_DIR=".qemu"

# This script starts an OVMF-based UEFI-environment in a QEMU VM, which
# executes the UEFI-app written in Rust.
#
# The "OVMF.fd"-file must be passed as environment variable.
function fn_main() {
    QEMU_ARGS=(
        # Disable default devices
        # QEMU by default enables a ton of devices which slow down boot.
        "-nodefaults"

        "-bios"
        # OVMF comes as environment variable
        "$OVMF"

        # Use a standard VGA for graphics
        "-vga"
        "std"

        # Use a modern machine, with acceleration if possible.
        "-machine"
        "q35,accel=kvm:tcg"

        # Allocate some memory
        "-m"
        "128M"

        # Mount a local directory as a FAT partition
        "-drive"
        "format=raw,file=fat:rw:${QEMU_DIR}"

        # Enable serial
        #
        # Connect the serial port to the host. OVMF is kind enough to connect
        # the UEFI stdout and stdin to that port too.
        "-serial"
        "stdio"

        # Setup monitor
        "-monitor"
        "vc:1024x768"

        # CPU settings
        "-cpu"
        "host"
    )

    echo "Executing: qemu-system-x86_64 " "${QEMU_ARGS[@]}"
    qemu-system-x86_64 "${QEMU_ARGS[@]}"
}

fn_main