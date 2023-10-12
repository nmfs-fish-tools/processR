#!/bin/bash -e 

set -x

./clean.sh

mkdir _library

R CMD build .
R CMD INSTALL -l _library processR_0.0.1.tar.gz
#R CMD BATCH mytest.R || echo "mytest failed"

