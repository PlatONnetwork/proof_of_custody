#!/bin/bash
set -x

curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
mkdir -p logs Player-Data

build_type=Debug
build_type=Release
verbose=0

# spdz
cd ${builddir}
cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir} \
  -DCMAKE_BUILD_TYPE=${build_type} -DVERBOSE=${verbose}
make -j8

# run examples and tests
export LD_LIBRARY_PATH=${install_dir}/lib:$LD_LIBRARY_PATH

parties=2
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
run_tests mascot_example.x
run_tests poc_online_op_test.x
run_tests poc_main.x
