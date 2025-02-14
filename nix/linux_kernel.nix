{ lib
  # the selected Linux kernel from "pkgs.linux_*"
, selectedLinuxKernelPkg
, pkgs
}:

let
  # Function that builds a kernel from the provided Linux source with the
  # given config.
  buildKernel = selectedLinuxKernelPkg: pkgs.linuxKernel.manualConfig {
    inherit (pkgs) stdenv lib;

    src = selectedLinuxKernelPkg.src;
    configfile = ./kernel.config;

    version = selectedLinuxKernelPkg.version;
    modDirVersion = selectedLinuxKernelPkg.version;

    # Property comes from `manualConfig.nix` in nixpkgs and allows the read
    # `configfile` from within the derivation.
    allowImportFromDerivation = true;
  };
in
buildKernel selectedLinuxKernelPkg