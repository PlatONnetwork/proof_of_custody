#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

class UnixSOcket {
 public:
  int partyid_ = -1;
  string socket_path_ = "";

  UnixSOcket(int partyid) : partyid_(partyid) {
    socket_path_ = string("/tmp/server_socket" + to_string(partyid));
  }
};

class UnixClient : public UnixSOcket {
  using UnixSOcket::UnixSOcket;
  int client_sockfd_;

 public:
  int init() {
    int len;
    struct sockaddr_un address;
    int result;

    if ((client_sockfd_ = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      cerr << "socket error!" << endl;
      exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, socket_path_.c_str());
    len = sizeof(address);

    int time_retry = 20000; //us
    int retries = 60 * 1000 * 1000 / time_retry; // 1 min
    bool connect_ok = false;
    do {
      result = connect(client_sockfd_, (struct sockaddr*)&address, len);
      if (result == 0) {
        connect_ok = true;
        break;
      }
      usleep(time_retry);
    } while (retries-- > 0);
    if (!connect_ok) {
      cerr << "ensure the server is up!" << endl;
      exit(EXIT_FAILURE);
    }
    return 0;
  }

  int send(const char* data, int len) {
    int byte;
    if ((byte = write(client_sockfd_, data, len)) == -1) {
      cerr << "write error!" << endl;
      exit(EXIT_FAILURE);
    }

    return 0;
  }
  int uninit() {
    close(client_sockfd_);
    return 0;
  }
};

class UnixServer : public UnixSOcket {
  using UnixSOcket::UnixSOcket;
  int server_sockfd_, client_sockfd_;

 public:
  int init() {
    int server_len, client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;

    unlink(socket_path_.c_str());
    server_sockfd_ = socket(AF_UNIX, SOCK_STREAM, 0);

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, socket_path_.c_str());
    server_len = sizeof(server_address);

    bind(server_sockfd_, (struct sockaddr*)&server_address, server_len);

    listen(server_sockfd_, 5);

    client_len = sizeof(client_address);
    client_sockfd_ =
      accept(server_sockfd_, (struct sockaddr*)&client_address, (socklen_t*)&client_len);

    return 0;
  }

  int recv(char* data, int len) {
    int byte;

    if ((byte = read(client_sockfd_, data, len)) == -1) {
      cerr << "read error!" << endl;
      exit(EXIT_FAILURE);
    }

    return 0;
  }
  int uninit() {
    //close(client_sockfd_);
    close(server_sockfd_);
    unlink(socket_path_.c_str());
    return 0;
  }
};