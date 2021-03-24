#pragma once

#include "Math/bls12_381.h"
#include <vector>
#include <iostream>

using namespace std;
using std::vector;

class VSS
{
    mclBnFr s; //secret in Fr;

public:
    //number of parties
    uint32_t nparty = 0;

    //number of threshold
    uint32_t threshold = 0;

    //mclBnG1 basePoint;

    //set secet
    void set_secret(mclBnFr _s, uint32_t pn, uint32_t tn);
    void set_secret(mclBnFr _s);
    mclBnFr get_secret();

    //generate random secret
    void rnd_secret();

    //generate shares
    void gen_share(vector<mclBnFr> &shares, vector<mclBnG1> &aux);

    //verfiy shares
    bool verify_share(mclBnFr share, vector<mclBnG1> aux, uint32_t n);

    VSS(uint32_t np, uint32_t thre) : nparty(np), threshold(thre)
    {
        mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
        if (threshold > nparty)
        {
            throw "invalid threshold & party number setting!";
        }
    }

    VSS(mclBnFr _s, uint32_t n, uint32_t tn) : s(_s), nparty(n), threshold(tn)
    {
        mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
        if (threshold > nparty)
        {
            throw "invalid threshold & party number setting!";
        }
    }
    ~VSS() {}
};
