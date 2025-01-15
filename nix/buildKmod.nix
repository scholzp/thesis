{ lib, stdenv, kernel}:
stdenv.mkDerivation {
  pname = "kmod";
  version = "0.0.1";

  src = ../implementation/kmod;

  nativeBuildInputs = kernel.moduleBuildDependencies;

  makeFlags = [
    "KERNELDIR=${kernel.dev}/lib/modules/${kernel.modDirVersion}/build"
    "INSTALL_MOD_PATH=$(out)"
  ];

  installTargets = [ "install" ];

  meta = {
    platforms = [ "x86_64-linux" ];
  };
}
