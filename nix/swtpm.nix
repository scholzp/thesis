{ pkgs, writeShellApplication }:
writeShellApplication {
  name = "start-swtpm";

  runtimeInputs = [ pkgs.swtpm ];

  text = ''
    mkdir -p /tmp/swtpm1
    swtpm socket \
    --tpmstate dir=/tmp/swtpm1 \
    --ctrl type=unixio,path=/tmp/swtpm1/swtpm-sock \
    --log level=20 \
    --tpm2
  '';
}