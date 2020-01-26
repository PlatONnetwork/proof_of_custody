#pragma once
#include "Math/bls12_381.h"
#include "System/Player.h"

typedef mclBnFr bls_sk;
typedef mclBnG1 bls_vk;
typedef mclBnG2 bls_sigma;

class BLS
{
    bls_sk sk;

public:
    bls_vk vk;
    bls_sigma sigma;
    Player P;
    uint32_t nparty;
    uint32_t threshold;
    bls_vk basePoint;


    BLS()
    {
        mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
        mclBnG1_setStr(&basePoint, (char *)G1_P.c_str(), G1_P.size(), 10);
    }


    BLS(Player _P, uint32_t np, uint32_t th) : P(_P),nparty(np),threshold(th)
    {
        mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
        mclBnG1_setStr(&basePoint, (char *)G1_P.c_str(), G1_P.size(), 10);
    }

    void set_vk(const bls_vk _vk)
    {
        vk = _vk;
    }

    void set_sigma(const bls_sigma _sigma)
    {
        sigma = _sigma;
    }

    //normal keygen,sign,verify algorithms
    void gen_keypair();
    void sign(const string msg);
    int verify(const bls_sigma _sigma, const string msg);

    //distributed keygen,sign algorithms
    void dis_gen_keypair();
    void dis_sign(const string msg);

    void combine_sigma();
};