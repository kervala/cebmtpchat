#! /bin/bash

for i in `grep "/* This file" * | cut -d ":" -f 1`; do echo $i; done > toto
for i in `ls *.{h,cpp}`; do if [ "`grep $i toto`" = "" ]; then echo $i; fi; done

