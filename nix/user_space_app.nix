{ lib, pkgs, stdenv}:
stdenv.mkDerivation {
  pname = "tee_user_space_app";
  version = "0.0.1";

  src = ../implementation/user_app;

  postInstall = ''
    mkdir $out/bin
    cp tee_userspace "$out/bin/tee_userspace"
  '';

  meta = {
    platforms = [ "x86_64-linux" ];
  };
}
