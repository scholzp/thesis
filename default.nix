# {
#   system ? builtins.currentSystem,
#   pkgs,
# }:
# let
#   pkgs = import pkgs {
#     config = { };
#     overlays = [ ];
#     inherit system;
#   };
# in
let
  pkgs = import <nixpkgs> { };
in
let
  kernel = pkgs.linuxPackages.kernel;
  lib = pkgs.lib;
  tee_kernel = implementation/kernel.elf32;
  # Hardcode start address of reserved low memory.
  # Take care of page granularity, which means the address should be divisible by 4096.
  start_address = "0xc8000";
  low_mem_size = "4096";

  stdenv = pkgs.stdenv;
  kmod = pkgs.callPackage ./nix/buildKmod.nix {inherit lib; inherit stdenv; inherit kernel; inherit start_address; inherit low_mem_size;};
  initrd = pkgs.callPackage ./nix/initrd.nix {inherit lib; inherit pkgs; inherit kmod; inherit tee_kernel;};
  runQemu = pkgs.callPackage ./nix/run_qemu.nix {inherit kernel; inherit initrd; inherit start_address; inherit low_mem_size;};
in {
  inherit initrd;
  inherit kmod;
  inherit runQemu;
}
