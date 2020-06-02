//
// Created by yangzhou on 5/15/20.
//
#define USE_COMM_PLAIN_TCP

#include <communication/CommFactory.hpp>
#include <simplestatetransfer/SimpleStateTransfer.hpp>
#include <bftengine/Replica.hpp>
#include <thread>
#include "communication/ICommunication.hpp"
#include "common.h"

#include "server_request.hpp"

int main(int argc, const char* argv[]) {
  DemoConfig conf;
  if (CreateConfig(argc, argv, conf) == -1) {
    return -1;
  }
  auto comm = bft::communication::CommFactory::create(*conf.comm_conf.get());

  State state;
  ServerRequest request;
  bftEngine::SimpleInMemoryStateTransfer::ISimpleInMemoryStateTransfer* st =
    bftEngine::SimpleInMemoryStateTransfer::create(
      &state, conf.replica_conf.numOfClientProxies, conf.comm_conf->selfId, 1, 4, true);

  auto replica =
    bftEngine::IReplica::createNewReplica(&conf.replica_conf, &request, st, comm, nullptr);

  replica->start();

  while (replica->isRunning()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  delete st;
  delete comm;
}
