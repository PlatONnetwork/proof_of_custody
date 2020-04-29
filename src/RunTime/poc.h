#pragma once

#include "Online/OnlineOp.h"
#include "Online/bls.h"
#include <string>
#include <iomanip>

using namespace std;

#define CHUNK_NUM (128 * 1024 / 48 + 1)

class Config_Info {
 public:
  int version = 0;
  unsigned int my_number;
  unsigned int no_online_threads;
  SSL_CTX* ctx;
  vector<unsigned int> portnum;
  SystemData SD;
  Machine machine;
  offline_control_data OCD;
  int ssocket;
  vector<vector<vector<int>>> csockets;
  unsigned int tnthreads;
  int verbose;
  Config_Info() {}
};

/*
The distributed setup stage to generate private signing key 
shares of BLS along with the public key.
It is essentially the distrbuted key generation algorithm.
*/
void poc_setup(BLS& bls, Player& P);

/*
Ephermeral key for UHF and legendre prf.
It is essentially the distributed signing algorithms.
*/
void poc_compute_enphem_key(
  G2_Affine_Coordinates& ac, BLS& bls, const string& msg, int online_num, Player& P,
  Config_Info& CI);

/*
Compute UHF and legendre prf.
*/
void poc_compute_custody_bit_offline(
  vector<Share>& pre_key, const vector<Share>& keys, int online_num, Player& P, Config_Info& CI);

int poc_compute_custody_bit_online(
  const vector<Share> pre_key, const vector<gfp>& msg, int online_num, Player& P, Config_Info& CI);

//run offline and online in one step
int poc_compute_custody_bit(
  const vector<Share>& keys, const vector<gfp>& msg, int online_num, Player& P, Config_Info& CI);
