#include "bls.h"
#include "vss.h"
// #include "Tools/Crypto.h"
#include "Tools/Commit.h"
#include "Math/Lagrange.h"

void BLS_::keygen()
{
    mclBnG1 basePoint;
    getBasePointG1(basePoint);
    if (mclBnFr_setByCSPRNG(&sk) == -1)
    {
        throw invalid_program(); // invalid_keygen
    }
    mclBnG1_mul(&vk, &basePoint, &sk);
}

void BLS_::sign(const string msg)
{
    mclBnG2_hashAndMapTo(&sigma, (const char *)msg.c_str(), msg.size());
    mclBnG2_mul(&sigma, &sigma, &sk);
}

int BLS_::verify(const bls_sigma _sigma, const string msg)
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

void BLS_::sign_scale(const bls_sk scale, const string msg)
{
    mclBnG2_hashAndMapTo(&sigma, (const char *)msg.c_str(), msg.size());
    bls_sk sign_sk;
    mclBnFr_mul(&sign_sk, &sk, &scale);
    mclBnG2_mul(&sigma, &sigma, &sign_sk);
}

void BLS_::dstb_keygen(Player &P)
{
    VSS v(nparty, threshold);
    vector<bls_sk> shs;
    vector<bls_vk> aux;
    v.rnd_secret();
    v.gen_share(shs, aux);

    sk = shs[P.my_num()];
    vk = aux[0];

    vector<bls_sk> tmp_shares(P.num_players());
    vector<vector<bls_vk>> aux_tmp(P.num_players(), vector<bls_vk>(aux.size()));
    string ss;
    mclBnG1_to_str(ss, aux[0]);

    vector<string> CommAux(P.num_players());
    vector<string> OpenAux(P.num_players());
    octetStream os_comm;
    octetStream os_open;
    octetStream os_data(ss.size(), (const octet *)ss.data());
    Commit(os_comm, os_open, os_data, 1); //P.my_num());

    // send my commitment
    P.send_all(os_comm);

    // receive other commitments
    vector<octetStream> os_comms(P.num_players()); // CommAux(P.num_players());
    P.receive_all(os_comms);

    // send my opening
    P.send_all(os_open);

    // receive other opening
    vector<octetStream> os_opens(P.num_players()); // OpenAux(P.num_players());
    P.receive_all(os_opens);

    //run Feldman VSS

    // send shares
    tmp_shares[P.my_num()] = shs[P.my_num()];
    for (int i = 0; i < P.num_players(); i++)
    {
        if (i != P.my_num())
        {
            mclBnFr_to_str(ss, shs[i]);
            octetStream os_ss(ss.size(), (const octet *)ss.data());
            P.send_to(i, os_ss);
        }
    }
    // receive shares
    for (int i = 0; i < P.num_players(); i++)
    {
        if (i != P.my_num())
        {
            octetStream os_ss;
            P.receive_player(i, os_ss);
            ss.assign((char *)os_ss.get_data(), os_ss.get_length()); // opt. yyltodo
            str_to_mclBnFr(tmp_shares[i], ss);
        }
    }

    // send aux
    for (int i = 0; i < P.num_players(); i++)
    {
        if (i != P.my_num())
        {
            for (int k = 0; k < aux.size(); k++)
            {
                mclBnG1_to_str(ss, aux[k]);
                octetStream os_ss(ss.size(), (const octet *)ss.data());
                P.send_to(i, os_ss);
            }
        }
    }

    //receive aux
    aux_tmp[P.my_num()] = aux;
    for (int i = 0; i < P.num_players(); i++)
    {
        if (i != P.my_num())
        {
            for (int k = 0; k < aux.size(); k++)
            {
                ss.clear();
                octetStream os_ss;
                P.receive_player(i, os_ss);
                ss.assign((char *)os_ss.get_data(), os_ss.get_length()); // opt. yyltodo
                str_to_mclBnG1(aux_tmp[i][k], ss);
            }
        }
    }

#if 1
    //check commitments,verify shares and set sk,vk
    for (int i = 0; i < P.num_players(); i++)
    {
        if (i != P.my_num())
        {
            octetStream os_ss;
            bool res1 = Open(os_ss, os_comms[i], os_opens[i], 1); //P.my_num());
            ss.clear();
            ss.assign((char *)os_ss.get_data(), os_ss.get_length()); // opt. yyltodo

            bool res2 = v.verify_share(tmp_shares[i], aux_tmp[i], P.my_num() + 1);
            bls_vk a0;
            str_to_mclBnG1(a0, ss);
            bool res3 = mclBnG1_isEqual(&a0, &aux_tmp[i][0]) ? true : false;
            //*RC*// cout << " res1:" << res1 << " res2:" << res2 << " res3:" << res3 << endl;
            if (res1 && res2 && res3)
            {
                mclBnFr_add(&sk, &sk, &tmp_shares[i]);
                mclBnG1_add(&vk, &vk, &aux_tmp[i][0]);
            }
            else
            {
                throw invalid_commitment(); // invalid_share();
            }
        }
    }
#endif
}
