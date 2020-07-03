#!/bin/bash
#set -x

curdir=$(pwd)
chmod +x *.sh

# <prog> program name in programs/.
# <n> the number of parties.
if [ $# -lt 2 ]; then
    echo "$0 <prog> <n>"
    exit 1
fi
prog=$1
n=$2

# compile
# ./compile.sh

# test
mkdir -p log out
for ((i = 1; i < $n; i++)); do
    ./${prog} $i >log/${prog}-$i.log 2>&1 &
done
./${prog} 0 >log/${prog}-0.log 2>&1
#./${prog} 0

echo 'DONE! See ./log/*'
exit 0
