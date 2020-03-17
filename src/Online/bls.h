#pragma once
#include "Math/bls12_381.h"
#include "System/Player.h"
#include "Group.h"

typedef mclBnFr bls_sk;
typedef mclBnG1 bls_vk;
typedef mclBnG2 bls_sigma;

class BLS
{
    bls_sk sk;

public:
    bls_vk vk;
    bls_sigma sigma;
    unsigned int nparty;
    unsigned int threshold;

    BLS()
    {
        mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    }

    BLS(unsigned int np, unsigned int th) : nparty(np), threshold(th)
    {
        mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    }

    void set_vk(const bls_vk _vk)
    {
        vk = _vk;
    }

    void set_sigma(const bls_sigma _sigma)
    {
        sigma = _sigma;
    }

    bls_sk get_sk()
    {
        return sk;
    }

    //normal keygen,sign,verify algorithms
    void keygen();
    void sign(const string msg);
    int verify(const bls_sigma _sigma, const string msg);

    //sign with scale*sk on msg
    void sign_scale(const bls_sk scale, const string msg);


    //distributed keygen,sign algorithms
    void dstb_keygen(Player &P);
    void dstb_sign(G2_Affine_Coordinates &out, const string msg,
                   Processor &Proc, int online_num, Player &P,
                   offline_control_data &OCD, Machine &machine);
};