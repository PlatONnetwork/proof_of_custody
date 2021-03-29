#!/bin/bash
set -x

curdir=$(pwd)

# compile
cd ${curdir}
bash ./compile.sh

# test
cd ${curdir}
bash ./test.sh
