#!/bin/bash
set -x
# set -e

# ====================
curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
# ====================

cd ${builddir}/cbft
mkdir -p log

# =====================
server_nums=7 # 4
faulty_nums=2 # 1
client_nums=5 # 1
id_index=0
# =====================

echo -e "generate keyfiles"
${curdir}/3rd/concord-bft/build/tools/GenerateConcordKeys -n ${server_nums} -f ${faulty_nums} -o private_replica_
sleep 2

echo -e "start serers"
for ((i = 0; i < $server_nums; i++)); do
  ./server -id ${id_index} -c ${client_nums} -r ${server_nums} >log/server-${i}.log 2>&1 &
  id_index=$(($id_index + 1))
done
sleep 2

echo -e "start client(s)"
first_cid=${id_index}
id_index=$(($id_index + 1))
for ((i = 1; i < $client_nums; i++)); do
  ./client -id ${id_index} -cl ${client_nums} -r ${server_nums} -f ${faulty_nums} >log/client-${i}.log 2>&1 &
  id_index=$(($id_index + 1))
done
# ./client -id ${first_cid} -cl ${client_nums} | tee log/client-0.log 2>&1
./client -id ${first_cid} -cl ${client_nums} -r ${server_nums} -f ${faulty_nums} >log/client-0.log 2>&1

sleep 2
echo -e "stop all servers and client(s)"
killall server client
sleep 1
killall server client

echo "DONE"
