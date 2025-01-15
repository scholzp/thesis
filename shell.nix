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
  ];

  # Environment variables
  OVMF = "${pkgs.OVMF.fd}/FV/OVMF.fd";
}