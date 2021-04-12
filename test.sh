#!/bin/bash
set -x

# ====================
curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
# ====================

# run examples and tests
export LD_LIBRARY_PATH=${install_dir}/lib:$LD_LIBRARY_PATH
mkdir -p logs Player-Data

parties=${1:-2}
hostname=${2:-"localhost"}
# run_tests <program name>
function run_tests() {
  cd ${curdir}
  prog=$1
  if [ ! -f "${builddir}/${prog}" ]; then
    return
  fi

  echo -e "run ${prog} ...\n"
  sleep 1
  Scripts/setup-ssl.sh $parties
  cp -f ${builddir}/${prog} ./
  for ((i = 1; i < $parties; i++)); do
    ./${prog} $i $parties >logs/${prog}-$i 2>&1 &
  done
  ./${prog} 0 $parties | tee logs/${prog}-0 2>&1
}
# run_tests mascot_example.x
# run_tests poc_online_op_test.x
# run_tests poc_online_op_perf.x

# run_tests <program name>
function run_tests_ex() {
  cd ${curdir}
  prog=$1
  if [ ! -f "${builddir}/${prog}" ]; then
    return
  fi

  echo -e "run ${prog} ...\n"
  sleep 1
  Scripts/setup-ssl.sh $parties
  cp -f ${builddir}/${prog} ./
  for ((i = 1; i < $parties; i++)); do
    ./${prog} -P $i -N $parties -h ${hostname} -lgp1 381 -lgp2 256 >logs/${prog}-$parties-$i.log 2>&1 &
  done
  ./${prog} -P 0 -N $parties -h ${hostname} -lgp1 381 -lgp2 256 | tee logs/${prog}-$parties-0.log 2>&1
  cat logs/${prog}-$parties-0.log | grep POC | grep elapsed
}
# run_tests_ex poc_main.x
# run_tests_ex poc_main2.x
run_tests_ex poc_main2fork.x
