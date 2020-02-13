#include "OnlineOP.h"
#include "bls.h"
#include <string>

using namespace std;

#define CHUNK_NUM 128*1024/48+1

/*
The distributed setup stage to generate private signing key 
shares of BLS along with the public key.
It is essentially the distrbuted key generation algorithm.
*/
void pocSetup(BLS &bls, Player &P);

/*
Ephermeral key for UHF and legendre prf.
It is essentially the distributed signing algorithms.
*/
void pocEphemKey(G2_Affine_Coordinates &ac, BLS &bls, const string msg,
                 Processor &Proc, int online_num, Player &P,
                 offline_control_data &OCD, Machine &machine);

/*
Compute UHF and legendre prf.
*/
int pocGenProof(const vector<Share> keys, const vector<gfp> msg,
                Processor &Proc, int online_num, Player &P,
                offline_control_data &OCD, Machine &machine);