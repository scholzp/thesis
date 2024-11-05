{ sources ? import ./nix/sources.nix { }
, pkgs ? import sources.nixpkgs { }
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gnumake
    niv
    rustup
  ];

  # Environment variables
  OVMF = "${pkgs.OVMF.fd}/FV/OVMF.fd";
}