#pragma once

#include "Online/OnlineOp.h"
#include "Online/bls.h"
#include <string>

using namespace std;

#define CHUNK_NUM 128 * 1024 / 48 + 1

class Config_Info
{
public:
    unsigned int my_number;
    unsigned int no_online_threads;
    SSL_CTX *ctx;
    vector<unsigned int> portnum;
    SystemData SD;
    Machine machine;
    offline_control_data OCD;
    Processor Proc;
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
void poc_Setup(BLS &bls, Player &P);

/*
Ephermeral key for UHF and legendre prf.
It is essentially the distributed signing algorithms.
*/
void poc_EnpherKey(G2_Affine_Coordinates &ac, BLS &bls, const string &msg,
                   int online_num, Player &P, Config_Info &CI);

/*
void poc_EphemKey(G2_Affine_Coordinates &ac, BLS &bls, const string msg,
                  Processor &Proc, int online_num, Player &P,
                  offline_control_data &OCD, Machine &machine);
*/
/*
Compute UHF and legendre prf.
*/
int poc_GenProof(const vector<Share> &keys, const vector<gfp> &msg,
                 int online_num, Player &P, Config_Info &CI);

int poc_GenProof(const vector<Share> keys, const vector<gfp> msg,
                 Processor &Proc, int online_num, Player &P,
                 offline_control_data &OCD, Machine &machine);