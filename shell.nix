{ sources ? import ./nix/sources.nix { }
, pkgs ? import sources.nixpkgs { }
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    argc
    binutils
    cargo-nextest
    clang-tools
    gcc
    gnumake
    grub2
    niv
    pkgs.niv
    pkgs.cmake-format
    qemu
    rustup
    swtpm
    unixtools.xxd
    xorriso
  ];

  # Environment variables
  OVMF = "${pkgs.OVMF.fd}/FV/OVMF.fd";
}