//
// Created by yangzhou on 5/15/20.
//
#include <bftengine/Replica.hpp>
#include <bftengine/SimpleClient.hpp>
#include <communication/CommFactory.hpp>
#include <simplestatetransfer/SimpleStateTransfer.hpp>
#include <thread>
#include <iostream>
#include "common.h"
#include "communication/ICommunication.hpp"
//#include "server_request.hpp"

struct ClientParams {
  uint32_t numOfOperations = 4600;
  uint16_t clientId = 4;
  uint16_t numOfReplicas = 4;
  uint16_t numOfClients = 1;
  uint16_t numOfFaulty = 1;
  uint16_t numOfSlow = 0;
  std::string configFileName;
  bool measurePerformance = false;

  uint16_t get_numOfReplicas() { return (uint16_t)(3 * numOfFaulty + 2 * numOfSlow + 1); }
};

#include <string>
using namespace std;

string get_nonce(int p) {
  string s = "12345";

  ClientParams cp;
  cp.clientId = 4 + p;

  auto conf = CreateClientConfig(cp.clientId);

  bftEngine::SeqNumberGeneratorForClientRequests* pSeqGen =
    bftEngine::SeqNumberGeneratorForClientRequests::createSeqNumberGeneratorForClientRequests();
  bft::communication::ICommunication* comm = bft::communication::CommFactory::create(*conf.get());
  bftEngine::SimpleClient* client =
    bftEngine::SimpleClient::createSimpleClient(comm, cp.clientId, cp.numOfFaulty, cp.numOfSlow);
  auto aggregator = std::make_shared<concordMetrics::Aggregator>();
  client->setAggregator(aggregator);
  comm->Start();

  {
    // Generate a value to store.
    uint64_t expectedLastValue = (4 + 1) * (4 + 7) * (4 + 18);
    std::cout << "expectedLastValue:" << expectedLastValue << std::endl;

    // Prepare request parameters.
    const uint32_t kRequestLength = 2;
    const uint64_t requestBuffer[kRequestLength] = {ECHO_VALUE, expectedLastValue};
    const char* rawRequestBuffer = reinterpret_cast<const char*>(requestBuffer);
    const uint32_t rawRequestLength = sizeof(uint64_t) * kRequestLength;

    const uint64_t requestSequenceNumber = pSeqGen->generateUniqueSequenceNumberForRequest();

    const uint64_t timeout = bftEngine::SimpleClient::INFINITE_TIMEOUT;

    const uint32_t kReplyBufferLength = sizeof(uint64_t);
    char replyBuffer[kReplyBufferLength];
    uint32_t actualReplyLength = 0;

    client->sendRequest(
      bftEngine::ClientMsgFlag::EMPTY_FLAGS_REQ, rawRequestBuffer, rawRequestLength,
      requestSequenceNumber, timeout, kReplyBufferLength, replyBuffer, actualReplyLength);

    auto reply_value = *(uint64_t*)(replyBuffer);
    if (reply_value == expectedLastValue) {
      std::cout << "reply success value:" << reply_value << std::endl;
    } else {
      std::cout << "reply failed value:" << reply_value << " expect:" << expectedLastValue
                << " actualReplyLength:" << actualReplyLength << std::endl;
    }
  }
  comm->Stop();

  return s;
}
