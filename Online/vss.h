#pragma once

#include "Math/bls12_381.h"
#include <vector>
#include <iostream>

using namespace std;
using std::vector;

class VSS
{
    mclBnFr s;//secret in Fr;

    public:

    //number of parties
    uint32_t nparty = 0;

    //number of threshold
    uint32_t threshold = 0;

    mclBnG1 basePoint;

    //set secet
    void set_secret(mclBnFr _s, uint32_t pn, uint32_t tn);
    void set_secret(mclBnFr _s);
    mclBnFr get_secret();

    //generate random secret
    void rnd_secret();

    //generate shares
    void gen_share(vector<mclBnFr> &shares, vector<mclBnG1> &aux);

    //verfiy shares
    int verify_share(mclBnFr share, vector<mclBnG1> aux, uint32_t n);

    //recover share, for test (not test yet)
    void recover_share(vector<mclBnFr> shares);

    //compute lagrange coeffecient (not test yet)
    void lagrange_coeff(vector<mclBnFr> &lag_coeff, vector<int> val);

    VSS(uint32_t np, uint32_t thre): nparty(np), threshold(thre)
    {
        mclBn_init(MCL_BLS12_381,MCLBN_COMPILED_TIME_VAR);
        mclBnG1_setStr(&basePoint, (char *) G1_P.c_str(), G1_P.size(), 10);
    }
    
    VSS(mclBnFr _s, uint32_t n, uint32_t tn):s(_s),nparty(n),threshold(tn)
    {
        mclBnG1_setStr(&basePoint, (char *) G1_P.c_str(), G1_P.size(), 10);

        if (threshold > nparty)
        {
            throw "invalid threshold & party number setting!";
        }
    }
    ~VSS(){}
};