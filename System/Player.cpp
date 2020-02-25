/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include <errno.h>
#if !defined(__MACH__)
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <resolv.h>

#include <arpa/inet.h>

#include <fstream>
#include <mutex>
using namespace std;

#include "Exceptions/Exceptions.h"
#include "Networking.h"
#include "Player.h"

Player::Player(
    int mynumber, const SystemData &SD, int thread, SSL_CTX *ctx, vector<vector<int>> &csockets,
    const vector<gfp> &MacK, int verbose)
{
  clocks.resize(10);
  G.ReSeed(thread);
  sha256.resize(3);
  SHA256_Init(&sha256[0]);
  SHA256_Init(&sha256[1]);
  SHA256_Init(&sha256[2]);

  me = mynumber;
  ssl.resize(SD.n);

  br_messages_sent = 0;
  pp_messages_sent = 0;
  pp_messages_recv = 0;
  data_sent = 0;
  data_received = 0;

  // When communicating with player i, player me acts as server when i<me
  for (unsigned int i = 0; i < SD.n; i++)
  {
    ssl[i].resize(3);
    for (unsigned int j = 0; j < 3; j++)
    {
      if (i != me)
      {
        ssl[i][j] = SSL_new(ctx); /* get new SSL state with context */
        if (i < me)
        { /* set connection socket to SSL state */
          int ret = SSL_set_fd(ssl[i][j], csockets[i][j]);
          if (ret == 0)
          {
            printf(
                "S: Player %d failed to SSL_set_fd with player %d on connection %d in thread %d\n",
                mynumber, i, j, thread);
            throw SSL_error("SSL_set_fd");
          }
          if (verbose > 0)
          {
            printf(
                "S: Player %d going SSL with player %d on connction %d at %s in thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
          }
          /* do SSL-protocol accept */
          ret = SSL_accept(ssl[i][j]);
          if (ret <= 0)
          {
            printf(
                "S: Error in player %d accepting to player %d on connection %d at address %s "
                "in thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
            ERR_print_errors_fp(stdout);
            throw SSL_error("SSL_accept");
          }
          if (verbose > 0)
          {
            printf(
                "S: Player %d connected to player %d on connection %d in thread %d with %s "
                "encryption\n",
                mynumber, i, j, thread, SSL_get_cipher(ssl[i][j]));
          }
        }
        else
        { // Now client side stuff
          int ret = SSL_set_fd(ssl[i][j], csockets[i][j]);
          if (ret == 0)
          {
            printf(
                "C: Player %d failed to SSL_set_fd with player %d on connection% d in thread %d\n",
                mynumber, i, j, thread);
            throw SSL_error("SSL_set_fd");
          }
          if (verbose > 0)
          {
            printf(
                "C: Player %d going SSL with player %d on connection %d at %s in thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
          }
          /* do SSL-protocol connect */
          ret = SSL_connect(ssl[i][j]);
          if (ret <= 0)
          {
            printf(
                "C: Error player %d connecting to player %d on connection %d at address %s in "
                "thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
            ERR_print_errors_fp(stdout);
            throw SSL_error("SSL_connect");
          }
          if (verbose > 0)
          {
            printf(
                "C: Player %d connected to player %d on connection %d in thread %d with %s "
                "encryption\n",
                mynumber, i, j, thread, SSL_get_cipher(ssl[i][j]));
          }
        }
        ShowCerts(ssl[i][j], SD.PlayerCN[i], verbose - 1); /* get cert and test common name */
      }
    }
  }

  mac_keys = MacK;
}

Player::~Player()
{
//  print_network_data();
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i != me)
    {
      SSL_free(ssl[i][0]);
      SSL_free(ssl[i][1]);
      SSL_free(ssl[i][2]);
    }
  }
}
void Player::Init(
    int mynumber, const SystemData &SD, int thread, SSL_CTX *ctx, vector<vector<int>> &csockets,
    int verbose)
{
  clocks.resize(10);
  G.ReSeed(thread);
  sha256.resize(3);
  SHA256_Init(&sha256[0]);
  SHA256_Init(&sha256[1]);
  SHA256_Init(&sha256[2]);

  me = mynumber;
  ssl.resize(SD.n);

  br_messages_sent = 0;
  pp_messages_sent = 0;
  pp_messages_recv = 0;
  data_sent = 0;
  data_received = 0;

  // When communicating with player i, player me acts as server when i<me
  for (unsigned int i = 0; i < SD.n; i++)
  {
    ssl[i].resize(3);
    for (unsigned int j = 0; j < 3; j++)
    {
      if (i != me)
      {
        ssl[i][j] = SSL_new(ctx); /* get new SSL state with context */
        if (i < me)
        { /* set connection socket to SSL state */
          int ret = SSL_set_fd(ssl[i][j], csockets[i][j]);
          if (ret == 0)
          {
            printf(
                "S: Player %d failed to SSL_set_fd with player %d on connection %d in thread %d\n",
                mynumber, i, j, thread);
            throw SSL_error("SSL_set_fd");
          }
          if (verbose > 0)
          {
            printf(
                "S: Player %d going SSL with player %d on connction %d at %s in thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
          }
          /* do SSL-protocol accept */
          ret = SSL_accept(ssl[i][j]);
          if (ret <= 0)
          {
            printf(
                "S: Error in player %d accepting to player %d on connection %d at address %s "
                "in thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
            ERR_print_errors_fp(stdout);
            throw SSL_error("SSL_accept");
          }
          if (verbose > 0)
          {
            printf(
                "S: Player %d connected to player %d on connection %d in thread %d with %s "
                "encryption\n",
                mynumber, i, j, thread, SSL_get_cipher(ssl[i][j]));
          }
        }
        else
        { // Now client side stuff
          int ret = SSL_set_fd(ssl[i][j], csockets[i][j]);
          if (ret == 0)
          {
            printf(
                "C: Player %d failed to SSL_set_fd with player %d on connection% d in thread %d\n",
                mynumber, i, j, thread);
            throw SSL_error("SSL_set_fd");
          }
          if (verbose > 0)
          {
            printf(
                "C: Player %d going SSL with player %d on connection %d at %s in thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
          }
          /* do SSL-protocol connect */
          ret = SSL_connect(ssl[i][j]);
          if (ret <= 0)
          {
            printf(
                "C: Error player %d connecting to player %d on connection %d at address %s in "
                "thread %d\n",
                mynumber, i, j, SD.IP[i].c_str(), thread);
            ERR_print_errors_fp(stdout);
            throw SSL_error("SSL_connect");
          }
          if (verbose > 0)
          {
            printf(
                "C: Player %d connected to player %d on connection %d in thread %d with %s "
                "encryption\n",
                mynumber, i, j, thread, SSL_get_cipher(ssl[i][j]));
          }
        }
        ShowCerts(ssl[i][j], SD.PlayerCN[i], verbose - 1); /* get cert and test common name */
      }
    }
  }
}
void Player::send_all(const string &o, int connection, bool verbose) const
{
  uint8_t buff[4];
  br_messages_sent++;
  int len_buff = 4;
  encode_length(buff, o.length());
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i != me)
    {
      if (SSL_write(ssl[i][connection], buff, 4) != 4)
      {
        throw sending_error();
      }
      if (SSL_write(ssl[i][connection], o.c_str(), o.length()) != (int)o.length())
      {
        throw sending_error();
      }
      data_sent += len_buff + o.length();
      pp_messages_sent++;
      if (verbose)
      {
        printf("Sent to player %d : ", i);
        for (unsigned int j = 0; j < 4; j++)
        {
          printf("%.2X", (uint8_t)buff[j]);
        }
        printf(" : ");
        for (unsigned int j = 0; j < o.size(); j++)
        {
          printf("%.2X", (uint8_t)o.c_str()[j]);
        }
        printf("\n");
      }
    }
  }
}

void Player::send_to_player(int player, const string &o, int connection) const
{
  uint8_t buff[4];
  int len_buff = 4;
  encode_length(buff, o.length());
  if (SSL_write(ssl[player][connection], buff, len_buff) != len_buff)
  {
    throw sending_error();
  }
  if (SSL_write(ssl[player][connection], o.c_str(), o.length()) != (int)o.length())
  {
    throw sending_error();
  }
  pp_messages_sent++;
  data_sent += len_buff + o.length();
}

void receive(SSL *ssl, uint8_t *data, int len)
{
  int i = 0, j;
  while (len - i > 0)
  {
    j = SSL_read(ssl, data + i, len - i);
    if (j <= 0)
    {
      /*
             int e0=SSL_get_error(ssl,j);
             int e1=ERR_get_error();
             printf("SSL_READ error : %d : %d % d : Was trying to read % d bytes out
             of % d bytes \n",j,e0,e1,len-i,len);
             perror("Some Error" );
          */
      throw receiving_error();
    }
    i = i + j;
  }
  if (len - i != 0)
  {
    throw receiving_error();
  }
}

void Player::receive_from_player(int i, string &o, int connection, bool verbose) const
{
  uint8_t buff[4];
  unsigned int len_buff = 4;
  receive(ssl[i][connection], buff, len_buff);
  int nlen = decode_length(buff);
  uint8_t *sbuff = new uint8_t[nlen];
  receive(ssl[i][connection], sbuff, nlen);
  o.assign((char *)sbuff, nlen);
  data_received += len_buff + nlen;
  pp_messages_recv++;
  if (verbose)
  {
    printf("Received from player %d : ", i);
    for (unsigned int j = 0; j < len_buff; j++)
    {
      printf("%.2X", (uint8_t)buff[j]);
    }
    printf(" : ");
    for (unsigned int j = 0; j < o.size(); j++)
    {
      printf("%.2X", (uint8_t)o.c_str()[j]);
    }
    printf("\n");
    for (int j = 0; j < nlen; j++)
    {
      printf("%.2X", (uint8_t)sbuff[j]);
    }
    printf("\n");
  }
  delete[] sbuff;
}

/* This is deliberately weird to avoid problems with OS max buffer
 * size getting in the way
 */
void Player::Broadcast_Receive(vector<string> &o, bool check, int connection)
{
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i > me)
    {
      send_to_player(i, o[me], connection);
    }
    else if (i < me)
    {
      receive_from_player(i, o[i], connection);
    }
  }
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i < me)
    {
      send_to_player(i, o[me], connection);
    }
    else if (i > me)
    {
      receive_from_player(i, o[i], connection);
    }
  }

  if (check)
  {
    for (unsigned int i = 0; i < ssl.size(); i++)
    {
      SHA256_Update(&sha256[connection], o[i].c_str(), o[i].size());
    }
  }
}

void Player::Check_Broadcast(int connection)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_Final(hash, &sha256[connection]);
  string ss((char *)hash, SHA256_DIGEST_LENGTH);
  send_all(ss, connection);
  for (unsigned int i = 0; i < nplayers(); i++)
  {
    if (i != whoami())
    {
      string is;
      receive_from_player(i, is, connection);
      if (is != ss)
      {
        throw hash_fail();
      }
    }
  }
  SHA256_Init(&sha256[connection]);
}

/* Again this is deliberately weird to avoid problems with OS max buffer
 * size getting in the way
 */
void Player::Send_Distinct_And_Receive(vector<string> &o, int connection) const
{
  vector<string> rec(ssl.size());
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i > me)
    {
      send_to_player(i, o[i], connection);
    }
    else if (i < me)
    {
      receive_from_player(i, rec[i], connection);
    }
  }
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i < me)
    {
      send_to_player(i, o[i], connection);
    }
    else if (i > me)
    {
      receive_from_player(i, rec[i], connection);
    }
  }
  for (unsigned int i = 0; i < ssl.size(); i++)
  {
    if (i != me)
    {
      o[i] = move(rec[i]);
    }
  }
}

void Player::print_network_data()
{
  cout << "------------------------------------ my id:" << me << endl;
  cout << "data_sent:" << data_sent << endl;
  cout << "data_received:" << data_received << endl;
  cout << "pp_messages_sent:" << pp_messages_sent << endl;
}
