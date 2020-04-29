#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
using namespace std;

class UnixSOcket {
 public:
  int partyid_ = -1;
  string sname;

  UnixSOcket(int partyid) : partyid_(partyid) {
    sname = string("server_socket" + to_string(partyid));
  }
};

class UnixClient : public UnixSOcket {
 public:
  using UnixSOcket::UnixSOcket;

  int server_sockfd, client_sockfd;
  int sockfd;

  int init() {
    int len;
    struct sockaddr_un address;
    int result;
    int i, byte;
    char send_buf[128], ch_recv;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, sname.c_str());
    len = sizeof(address);

    do {
      result = connect(sockfd, (struct sockaddr*)&address, len);
      if (result == -1) {
        //cout << "00000000000000000000000000000000000000000000" << endl;
        usleep(20000);
        continue;
        printf("ensure the server is up\n");
        perror("connect");
        exit(EXIT_FAILURE);
      }
      break;
    } while (true);

    return 0;
  }

  int send(const char* data, int len) {
    int i, byte;
    if ((byte = write(sockfd, data, len)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    return 0;
  }
  int send_to_server() {
    int i, byte;
    char send_buf[128], ch_recv;
    for (i = 0; i < 5; i++) {
      sprintf(send_buf, "client massage %d", i); 
      if ((byte = write(sockfd, send_buf, sizeof(send_buf))) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }
      if ((byte = read(sockfd, &ch_recv, 1)) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      printf("receive from server data is: %c\n", ch_recv);
    }
    return 0;
  }
  int uninit() {
    close(sockfd);
    return 0;
  }
};

class UnixServer : public UnixSOcket {
 public:
  using UnixSOcket::UnixSOcket;

  int server_sockfd, client_sockfd;

  int init() {
    int server_len, client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    int i, byte;
    char ch_send, recv_buf[128];

    unlink(sname.c_str());
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0); 
	  
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, sname.c_str());
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);

    listen(server_sockfd, 5);

    printf("server waiting for  client connect\n");
    client_len = sizeof(client_address);
	
    client_sockfd =
      accept(server_sockfd, (struct sockaddr*)&client_address, (socklen_t*)&client_len);
    printf("the server wait form client data\n");

    return 0;
  }

  int recv(char* data, int len) {
    int i, byte;

    if ((byte = read(client_sockfd, data, len)) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    return 0;
  }
  int recv_from_client() {
    int i, byte;
    char ch_send, recv_buf[128];
    for (i = 0, ch_send = '1'; i < 5; i++, ch_send++) {
      if ((byte = read(client_sockfd, recv_buf, sizeof(recv_buf))) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      printf("the massage receiver from client is: %s\n", recv_buf);
      sleep(1);
      if ((byte = write(client_sockfd, &ch_send, 1)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }
    }
    return 0;
  }
  int uninit() {
    //close(client_sockfd);
    close(server_sockfd);
    unlink(sname.c_str());
    return 0;
  }
};