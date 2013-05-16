#!/bin/sh

PREAMBLE=$(cat pres-preamble.tex)
cat awesome2d.rst | rst2beamer\
    --verbose\
    --documentoptions=ignorenonframetext\
    '--output-encoding=UTF-8:strict'\
    --overlaybullets=false\
    --latex-preamble="$PREAMBLE" > awesome2d.tex
pdflatex awesome2d.tex

