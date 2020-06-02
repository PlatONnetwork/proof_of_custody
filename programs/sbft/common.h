//
// Created by yangzhou on 5/15/20.
//

#pragma once

#include <communication/CommDefs.hpp>
#include <bftengine/ReplicaConfig.hpp>
#define ECHO_VALUE ((uint64_t)200)
struct DemoConfig {
  DemoConfig() = default;
  bftEngine::ReplicaConfig replica_conf;
  std::shared_ptr<bft::communication::PlainTcpConfig> comm_conf;
};

struct State {
  // Number of modifications made.
  uint64_t stateNum = 0;
  // Register value.
  uint64_t lastValue = 0;
};
int CreateConfig(int argc, const char* argv[], DemoConfig& conf);
std::shared_ptr<bft::communication::PlainTcpConfig> CreateClientConfig(int replica_id);
