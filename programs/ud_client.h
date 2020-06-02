#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
using namespace std;

int clientx(int partyid) {
  int sockfd;
  int len;
  struct sockaddr_un address;
  int result;
  int i, byte;
  char send_buf[128], ch_recv;

  string sname("server_socket" + to_string(partyid));

  //创建socket，指定通信协议为AF_UNIX,数据方式SOCK_STREAM
  if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //配置server_address
  address.sun_family = AF_UNIX;
  strcpy(address.sun_path, sname.c_str());
  len = sizeof(address);

  result = connect(sockfd, (struct sockaddr*)&address, len);

  if (result == -1) {
    printf("ensure the server is up\n");
    perror("connect");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < 5; i++) {
    sprintf(send_buf, "client massage %d", i); //用sprintf事先把消息写到send_buf
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
  close(sockfd);
  return 0;
}

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

    //创建socket，指定通信协议为AF_UNIX,数据方式SOCK_STREAM
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
    }

    //配置server_address
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, sname.c_str());
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr*)&address, len);

    if (result == -1) {
      printf("ensure the server is up\n");
      perror("connect");
      exit(EXIT_FAILURE);
    }
    return 0;
  }
  int send_to_server() {
    int i, byte;
    char send_buf[128], ch_recv;
    for (i = 0; i < 5; i++) {
      sprintf(send_buf, "client massage %d", i); //用sprintf事先把消息写到send_buf
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
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
class UnixServer {
  int partyid_ = -1;
  int server_sockfd, client_sockfd;
  string sname;

 public:
  UnixServer(int partyid) : partyid_(partyid) {
    sname = string("server_socket" + to_string(partyid));
  }
  int init() {
    int server_len, client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    int i, byte;
    char ch_send, recv_buf[128];

    unlink(sname.c_str());
    //unlink("server_socket"); //解除原有server_socket对象链接
    server_sockfd =
      socket(AF_UNIX, SOCK_STREAM, 0); //创建socket，指定通信协议为AF_UNIX,数据方式SOCK_STREAM
    //配置server_address
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, sname.c_str());
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);

    listen(server_sockfd, 5);

    printf("server waiting for  client connect\n");
    client_len = sizeof(client_address);
    //accept函数接收客户端求情，存储客户端地址信息、客户端地址大小
    client_sockfd =
      accept(server_sockfd, (struct sockaddr*)&client_address, (socklen_t*)&client_len);
    printf("the server wait form client data\n");

    return 0;
  }
  int recv_from_client() {
    int i, byte;
    char ch_send, recv_buf[128];
    for (i = 0, ch_send = '1'; i < 5; i++, ch_send++) {
      //从client_sockfd读取客户端发来的消息
      if ((byte = read(client_sockfd, recv_buf, sizeof(recv_buf))) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      printf("the massage receiver from client is: %s\n", recv_buf);
      sleep(1);
      //向客户端发送消息
      if ((byte = write(client_sockfd, &ch_send, 1)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }
    }
    return 0;
  }
  int uninit() {
    close(client_sockfd);
    unlink(sname.c_str());
    return 0;
  }
};
int serverx(int partyid) {
  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_un server_address;
  struct sockaddr_un client_address;
  int i, byte;
  char ch_send, recv_buf[128];

  string sname("server_socket" + to_string(partyid));
  unlink(sname.c_str());
  //unlink("server_socket"); //解除原有server_socket对象链接
  server_sockfd =
    socket(AF_UNIX, SOCK_STREAM, 0); //创建socket，指定通信协议为AF_UNIX,数据方式SOCK_STREAM
  //配置server_address
  server_address.sun_family = AF_UNIX;
  strcpy(server_address.sun_path, sname.c_str());
  server_len = sizeof(server_address);

  bind(server_sockfd, (struct sockaddr*)&server_address, server_len);

  listen(server_sockfd, 5);

  printf("server waiting for  client connect\n");
  client_len = sizeof(client_address);
  //accept函数接收客户端求情，存储客户端地址信息、客户端地址大小
  client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, (socklen_t*)&client_len);
  printf("the server wait form client data\n");

  for (i = 0, ch_send = '1'; i < 5; i++, ch_send++) {
    //从client_sockfd读取客户端发来的消息
    if ((byte = read(client_sockfd, recv_buf, sizeof(recv_buf))) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    printf("the massage receiver from client is: %s\n", recv_buf);
    sleep(1);
    //向客户端发送消息
    if ((byte = write(client_sockfd, &ch_send, 1)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  close(client_sockfd);
  unlink(sname.c_str());
}
