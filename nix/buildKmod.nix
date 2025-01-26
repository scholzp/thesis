{ lib, stdenv, kernel, start_address, pkgs}:
stdenv.mkDerivation {
  pname = "kmod";
  version = "0.0.1";

  src = ../implementation/kmod;

  nativeBuildInputs = kernel.moduleBuildDependencies;

  buildInputs = with pkgs; [ unixtools.xxd ];

  passthru = {
    # The kernel used to build the attribute.
    kernel-version = kernel.version;
  };

  makeFlags = [
    "KERNELDIR=${kernel.dev}/lib/modules/${kernel.modDirVersion}/build"
    "INSTALL_MOD_PATH=$(out)"
    "START_ADDRESS=${start_address}"
  ];

  installTargets = [ "install" ];

  meta = {
    platforms = [ "x86_64-linux" ];
  };
}
