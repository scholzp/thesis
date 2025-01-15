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
  stdenv = pkgs.stdenv;
in {
  kmod = pkgs.callPackage ./nix/buildKmod.nix {inherit lib; inherit stdenv; inherit kernel;};
}
