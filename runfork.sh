#!/bin/bash
#set -x

curdir=$(pwd)
chmod +x *.sh

# compile
./compile.sh

./scripts/run_program.sh mainfork $@
