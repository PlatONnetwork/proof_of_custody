#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string>
using namespace std;

#define MAXSIZE 2048

struct msgbuf_
{
  long mtype = 0;
  int mlen = 0;
  char mtext[MAXSIZE] = {0};
};
struct ipcmsg
{
  string pathname = "/tmp/ipc.test";
  key_t key;
  int id;
  msgbuf_ buf;
};

static inline int ipc_msg_init(ipcmsg &msg)
{
  ::mkdir(msg.pathname.c_str(), 0644);
  msg.key = ftok(msg.pathname.c_str(), 'a');
  if (-1 == msg.key)
  {
    perror("ftok failed!");
    return -1;
  }

  msg.id = msgget(msg.key, IPC_CREAT | 0666);
  if (-1 == msg.id)
  {
    perror("msgget failed!");
    return -1;
  }
  return 0;
}

static inline int ipc_msg_send(ipcmsg &msg)
{
  return msgsnd(msg.id, &msg.buf, sizeof(msgbuf_), 0);
}

static inline int ipc_msg_recv(ipcmsg &msg)
{
  return msgrcv(msg.id, &msg.buf, sizeof(msgbuf_), msg.buf.mtype, 0);
}

static inline int ipc_msg_uninit(ipcmsg &msg)
{
  int ret = msgctl(msg.id, IPC_RMID, NULL);
  if (-1 == ret)
  {
    perror("delete msg failed!");
    return -1;
  }
  return 0;
}
