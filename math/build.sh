#!/usr/bin/env bash
# Compile all derivation docs to PDF, aux files go to build/.
set -euo pipefail
cd "$(dirname "$0")"

latexmk -xelatex -interaction=nonstopmode -halt-on-error -outdir=. -auxdir=build *.tex

