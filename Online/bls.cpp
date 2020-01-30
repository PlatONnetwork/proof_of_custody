#include "bls.h"
#include "vss.h"
#include "Tools/Crypto.h"

void BLS::keygen()
{
    mclBnG1 basePoint;
    getBasePointG1(basePoint);
    if (mclBnFr_setByCSPRNG(&sk) == -1)
    {
        throw invalid_keygen();
    }
    mclBnG1_mul(&vk, &basePoint, &sk);
}

void BLS::sign(const string msg)
{
    mclBnG2_hashAndMapTo(&sigma, (const char *)msg.c_str(), msg.size());
    mclBnG2_mul(&sigma, &sigma, &sk);
}

int BLS::verify(const bls_sigma _sigma, const string msg)
{
    mclBnG1 basePoint;
    getBasePointG1(basePoint);

    mclBnGT e1, e2;
    mclBnG2 h;
    mclBnG2_hashAndMapTo(&h, (const char *)msg.c_str(), msg.size());
    mclBn_pairing(&e1, &vk, &h);
    mclBn_pairing(&e2, &basePoint, &_sigma);

    int ret = mclBnGT_isEqual(&e1, &e2);
    return ret;
}

void BLS::d_keygen(Player &P)
{
    VSS v(nparty, threshold);
    vector<bls_sk> shs;
    vector<bls_vk> aux;
    v.rnd_secret();
    v.gen_share(shs, aux);

    sk = shs[P.whoami()];
    vk = aux[0];

    cout<<aux.size()<<endl;

    vector<bls_sk> tmp_shares(P.nplayers());

    vector<vector<bls_vk>> aux_tmp(P.nplayers(), vector<bls_vk>(aux.size()));

    string ss;

    vector<string> CommAux(P.nplayers());
    vector<string> OpenAux(P.nplayers());

    mclBnG1_to_str(ss, aux[0]);
    Commit(CommAux[P.whoami()], OpenAux[P.whoami()], ss, P.G);

    //send my commitment
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            P.send_to_player(i, CommAux[P.whoami()], 1);
        }
    }

    //receive other commitments
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            P.receive_from_player(i, CommAux[i], 1, false);
        }
    }

    //send my opening
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            P.send_to_player(i, OpenAux[P.whoami()], 1);
        }
    }

    //receive other openings
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            P.receive_from_player(i, OpenAux[i], 1, false);
        }
    }

    //run Feldman VSS

    //send shares
    tmp_shares[P.whoami()] = shs[P.whoami()];
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            mclBnFr_to_str(ss, shs[i]);
            P.send_to_player(i, ss, 1);
        }
    }

    //receive shares
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            P.receive_from_player(i, ss, 1, false);
            str_to_mclBnFr(tmp_shares[i], ss);
        }
    }

    //send aux
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            for (int k = 0; k < aux.size(); k++)
            {
                mclBnG1_to_str(ss, aux[k]);
                P.send_to_player(i, ss, 1);
            }
        }
    }

    //receive aux
    aux_tmp[P.whoami()] = aux;
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            for (int k = 0; k < aux.size(); k++)
            {
                ss.clear();
                P.receive_from_player(i, ss, 1, false);
                str_to_mclBnG1(aux_tmp[i][k], ss);
            }
        }
    }

    //check commitments,verify shares and set sk,vk
    for (int i = 0; i < P.nplayers(); i++)
    {
        if (i != P.whoami())
        {
            bool res1 = Open(ss, CommAux[i], OpenAux[i]);
            bool res2 = v.verify_share(tmp_shares[i], aux_tmp[i], P.whoami() + 1);
            bls_vk a0;
            str_to_mclBnG1(a0, ss);
            bool res3 = mclBnG1_isEqual(&a0, &aux_tmp[i][0]) ? true : false;
            if (res1 && res2 && res3)
            {
                mclBnFr_add(&sk, &sk, &tmp_shares[i]);
                mclBnG1_add(&vk, &vk, &aux_tmp[i][0]);
            }
            else
            {
                throw invalid_share();
            }
        }
    }
/*
    for (int i = 0; i < P.nplayers(); i++)
    {
        cout << "print aux_tmp " << i << endl;
        for (int k = 0; k < aux.size(); k++)
        {
            print_mclBnG1(aux_tmp[i][k]);
        }
        cout<< "==================\n";
    }
*/
}

void BLS::d_sign(const string msg)
{
    sign(msg);
}

void BLS::combine_sigma()
{
}
