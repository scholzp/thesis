{ sources ? import ./nix/sources.nix { }
, pkgs ? import sources.nixpkgs { }
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gnumake
    niv
    rustup
    pkgs.niv
    pkgs.cmake-format
    clang-tools
    unixtools.xxd
  ];

  # Environment variables
  OVMF = "${pkgs.OVMF.fd}/FV/OVMF.fd";
}