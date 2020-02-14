#include "poc.h"

void poc_Setup(BLS &bls, Player &P)
{
    bls.dstb_keygen(P);
}

void poc_EphemKey(G2_Affine_Coordinates &ac, BLS &bls, const string msg,
                 Processor &Proc, int online_num, Player &P,
                 offline_control_data &OCD, Machine &machine)
{
    bls.dstb_sign(ac, msg, Proc, online_num, P, OCD, machine);
}

int poc_GenProof(const vector<Share> keys, const vector<gfp> msg,
                Processor &Proc, int online_num, Player &P,
                offline_control_data &OCD, Machine &machine)
{
    if (keys.size() != 2)
    {
        throw bad_value();
    }

    Share uhf_out;
    int res;
    OnlineOp online_op(Proc, online_num, P, OCD, machine);
    online_op.uhf(uhf_out, keys[0], msg, CHUNK_NUM);
    res = online_op.legendre_prf(keys[1], uhf_out);
    return res;
}