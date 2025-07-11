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
  # select a LTS kernel
  selectedLinuxKernelPkg = pkgs.linux_6_6; 
  kernel = pkgs.callPackage ./nix/linux_kernel.nix { inherit selectedLinuxKernelPkg; };
  lib = pkgs.lib;
  tee_kernel = implementation/kernel.elf32;
  # Hardcode start address of reserved low memory.
  # Take care of page granularity, which means the address should be divisible by 4096.
  start_address = "0x09000";
  low_mem_size = "4096";

  stdenv = pkgs.stdenv;
  # tee_kernel = pkgs.callPackage ./nix/tee_kernel.nix{};
  kmod = pkgs.callPackage ./nix/buildKmod.nix {inherit lib; inherit stdenv; inherit kernel; inherit start_address; inherit low_mem_size;};
  dbus_session_conf = pkgs.callPackage ./nix/dbus_conf.nix {};
  root_user = pkgs.callPackage ./nix/root_user.nix {};
  user_app = pkgs.callPackage ./nix/user_space_app.nix {};
  initrd = pkgs.callPackage ./nix/initrd.nix {inherit lib; inherit pkgs; inherit kmod; inherit tee_kernel; inherit dbus_session_conf; inherit root_user; inherit user_app;};
  runQemu = pkgs.callPackage ./nix/run_qemu.nix {inherit kernel; inherit initrd; inherit start_address; inherit low_mem_size;};
  runQemuTpm = pkgs.callPackage ./nix/run_qemu_tpm.nix {inherit kernel; inherit initrd; inherit start_address; inherit low_mem_size;};
  createIsoImage = lib.makeOverridable(pkgs.callPackage ./nix/create-iso-image.nix {inherit start_address; inherit low_mem_size;});
  swtpm = pkgs.callPackage ./nix/swtpm.nix {};
  isoImage = createIsoImage {inherit kernel; inherit initrd; };
in {
  inherit tee_kernel;
  inherit initrd;
  inherit kmod;
  inherit kernel;
  inherit runQemu;
  inherit swtpm;
  inherit runQemuTpm;
  inherit isoImage;
  inherit user_app;
}
