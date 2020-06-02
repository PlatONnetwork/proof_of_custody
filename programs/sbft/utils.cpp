//
// Created by yangzhou on 5/15/20.
//

#include <KeyfileIOUtils.hpp>
#include <communication/CommFactory.hpp>
#include "common.h"

const std::string kPrivateKeyPrefix = "private_file_";

const std::string kDefaultHost = "127.0.0.1";
const uint16_t kDefaultPort = 9000;

const uint32_t kDefaultBuffLength = 1024 * 1024;
const int kDefaultReplicaNum = 4;

bool GetReplicaConfig(
  int replica_id, const std::string& prefix, bftEngine::ReplicaConfig& replica_conf);

int CreateConfig(int argc, const char* argv[], DemoConfig& conf) {
  if (argc < 2) {
    return -1;
  }

  int replica_id = atoi(argv[1]);

  bft::communication::NodeMap node_map;

#if 1
  for (int i = 0; i < kDefaultReplicaNum; ++i) {
    if (replica_id != i) {
      node_map.insert({i,
                       bft::communication::NodeInfo{
                         kDefaultHost, static_cast<uint16_t>(kDefaultPort + i), true}});
    }
  }
#else
  for (int i = 0; i < kDefaultReplicaNum; ++i) {
    //if (replica_id != i) {
    node_map.insert(
      {i,
       bft::communication::NodeInfo{kDefaultHost, static_cast<uint16_t>(kDefaultPort + i), true}});
    //}
  }
#endif

  conf.comm_conf = std::make_shared<bft::communication::PlainTcpConfig>(
    kDefaultHost, kDefaultPort + replica_id, kDefaultBuffLength, node_map, 30000, replica_id,
    nullptr);
  // init replica config

  if (!GetReplicaConfig(replica_id, kPrivateKeyPrefix, conf.replica_conf)) {
    return -1;
  }

  conf.replica_conf.numOfClientProxies = 4;
  conf.replica_conf.viewChangeProtocolEnabled = true;
  conf.replica_conf.viewChangeTimerMillisec = 10 * 1000;
  conf.replica_conf.statusReportTimerMillisec = 10000;
  conf.replica_conf.concurrencyLevel = 1;
  conf.replica_conf.debugPersistentStorageEnabled = true;
  conf.replica_conf.singletonFromThis();

  return 0;
}

bool GetReplicaConfig(
  int replica_id, const std::string& prefix, bftEngine::ReplicaConfig& replica_conf) {
  std::string file;
  file = "./" + prefix + std::to_string(replica_id);
  return inputReplicaKeyfile(file, replica_conf);
}

std::shared_ptr<bft::communication::PlainTcpConfig> CreateClientConfig(int replica_id) {
  bft::communication::NodeMap node_map;

#if 1
  for (int i = 0; i < kDefaultReplicaNum; ++i) {
    if (replica_id != i) {
      node_map.insert({i,
                       bft::communication::NodeInfo{
                         kDefaultHost, static_cast<uint16_t>(kDefaultPort + i), true}});
    }
  }
#else
  int num_of_replicas = kDefaultReplicaNum;
  int num_of_clients = 3;
  for (int i = 0; i < (num_of_replicas + num_of_clients); ++i) {
    node_map.insert(
      {i,
       bft::communication::NodeInfo{kDefaultHost, static_cast<uint16_t>(kDefaultPort + i),
                                    i < num_of_replicas}});
  }
#endif

  //for (int i = 0; i < (num_of_replicas + num_of_clients); i++)
  //  nodes.insert({i, NodeInfo{ip, static_cast<uint16_t>(base_port_ + i * 2), i < num_of_replicas}});

  return std::make_shared<bft::communication::PlainTcpConfig>(
    kDefaultHost, kDefaultPort + replica_id, kDefaultBuffLength, node_map, 30000, replica_id,
    nullptr);
}
