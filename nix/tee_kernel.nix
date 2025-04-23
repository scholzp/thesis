{PROJECT_ROOT}:
let
  sources = import ./sources.nix;
  nixpkgs-mozilla = import sources.mozilla-local;
  # nixpkgs-mozilla = import sources.nixpkgs-mozilla;

  pkgs = import sources.nixpkgs {
    overlays = [
      nixpkgs-mozilla
    ];
  };

  toolchain = (pkgs.rustChannelOf {
    rustToolchain = ../implementation/os/phipsboot/rust-toolchain.toml;
    sha256 = "sha256-HZY1Jz9tB9OYBIvX0W5ypfN+4/870F6OlVilKKCsQW8=";
    #        ^ After you run `nix-build`, replace this with the actual
    #          hash from the error message - Redo if the error happens to occur 
    #          again (only likely if toolchain.toml changes)
  }).rust;
  naersk = pkgs.callPackage sources.naersk {
    cargo = toolchain;
    rustc = toolchain;
  };

  ROOT = "${PROJECT_ROOT}/implementation/os";
  CARGO_BUILD_STD_FLAGS = "-Z build-std=core,alloc,compiler_builtins -Z build-std-features=compiler-builtins-mem";

  PHIPSBOOT_SRC = ../implementation/os/phipsboot;
  PHIPSBOOT_RUST_TARGET = "x86_64-unknown-none-static";
  PHIPSBOOT_RUST_TARGET_FILE = "${PHIPSBOOT_SRC}/bin/${PHIPSBOOT_RUST_TARGET}.json";
  PHIPSBOOT_CARGO_FLAGS = ["--verbose" "--target ${PHIPSBOOT_RUST_TARGET_FILE} ${CARGO_BUILD_STD_FLAGS}"];
  PHIPSBOOT_CARGO_ARTIFACT = "${PHIPSBOOT_SRC}/target/${PHIPSBOOT_RUST_TARGET}/release/phipsboot";

in
naersk.buildPackage {
  depsBuildBuild = [
    pkgs.llvmPackages_14.bintools
  ];
  src = ../implementation/os/phipsboot;

  RUSTFLAGS="-C target-cpu=x86-64";
  RUST_SRC_PATH=./.;
  # see https://github.com/nix-community/naersk/pull/328
  # It's important to update the lock file to the one of the rust toolchain used
  additionalCargoLock = ./tee_kernel_rust_std_cargo.lock;
  # cargoBuildOptions takes a function as input. This function is expected to take a list as input and produce a list of
  # the options, which this lambda does. 
  cargoBuildOptions = (x:  x ++ PHIPSBOOT_CARGO_FLAGS );
  remapPathPrefix = true;
  postInstall = '' 
    objcopy -O elf32-i386 $out/bin/phipsboot $out/bin/phipsboot.elf32
	  cp $out/bin/phipsboot $out/bin/phipsboot.elf64
  '';
}