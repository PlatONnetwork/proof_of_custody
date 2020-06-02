//
// Created by yangzhou on 5/15/20.
//

#include "bftengine/Replica.hpp"
#include <iostream>

#include "common.h"

class ServerRequest : public bftEngine ::IRequestsHandler {
 public:
  ServerRequest() = default;

 public:
  virtual int execute(
    uint16_t clientId, uint64_t sequenceNum, uint8_t flags, uint32_t requestSize,
    const char* request, uint32_t maxReplySize, char* outReply, uint32_t& outActualReplySize,
    concordUtils::SpanWrapper& parent_span) {
    if (requestSize == sizeof(uint64_t) * 2) {
      auto type = *(uint64_t*)request;
      if (type == ECHO_VALUE) {
        auto value = *(((uint64_t*)request) + 1);
        auto reply_ptr = reinterpret_cast<uint64_t*>(outReply);
        *reply_ptr = value;
        outActualReplySize = sizeof(uint64_t);
        return 0;
      }
    }
    return -1;
  }
};
