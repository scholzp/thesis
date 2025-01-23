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
  # Hardcode start address of reserved low memory. Will be multiplied by 1024.
  # Look for page granularity, which means the address should be divisible by 4.
  start_address = "800";
  stdenv = pkgs.stdenv;
  kmod = pkgs.callPackage ./nix/buildKmod.nix {inherit lib; inherit stdenv; inherit kernel; inherit start_address;};
  initrd = pkgs.callPackage ./nix/initrd.nix {inherit lib; inherit pkgs; inherit kmod;};
  runQemu = pkgs.callPackage ./nix/run_qemu.nix {inherit kernel; inherit initrd;inherit start_address;};
in {
  inherit initrd;
  inherit kmod;
  inherit runQemu;
}
