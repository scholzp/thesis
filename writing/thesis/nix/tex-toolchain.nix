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
    makecell
    placeins
    siunitx
    standalone
    todonotes
    tracklang
    xfor
    xpatch;
}
