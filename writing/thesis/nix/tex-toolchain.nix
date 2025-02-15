{ pkgs }:
with pkgs;
texlive.combine {
  inherit (texlive) scheme-small
    biber
    biblatex
    csquotes
    currfile
    datatool
    filemod
    glossaries
    hyphenat
    lastpage
    latexmk
    mfirstuc
    siunitx
    standalone
    todonotes
    tracklang
    xfor
    xpatch;
}
