#! /bin/bash

shopt -s nullglob

cd tables
for i in $(ls -A *.tex)
do
    pdflatex $i
done
cd ..
