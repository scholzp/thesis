{ pkgs }:
with pkgs;
texlive.combine {
  inherit (texlive) scheme-small
    biber
    biblatex
    csquotes
    datatool
    glossaries
    hyphenat
    lastpage
    latexmk
    mfirstuc
    siunitx
    todonotes
    tracklang
    xfor
    xpatch;
}
