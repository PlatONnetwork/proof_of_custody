#include "bls.h"
#include "vss.h"

void BLS::gen_keypair()
{
    if (mclBnFr_setByCSPRNG(&sk) == -1)
    {
        throw "Errors in generating signing key!";
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
    char buff[1024];

    mclBnGT e1, e2;
    mclBnG2 h;
    mclBnG2_hashAndMapTo(&h, (const char *)msg.c_str(), msg.size());
    mclBn_pairing(&e1, &vk, &h);
    mclBn_pairing(&e2, &basePoint, &_sigma);

    int ret = mclBnGT_isEqual(&e1, &e2);
    return ret;
}

void BLS::dis_gen_keypair()
{
    VSS v(nparty, threshold);
    vector<bls_sk> shs;
    vector<bls_vk> aux, aux_verify;
    v.rnd_secret();
    v.gen_share(shs, aux);

    mclBnFr_setInt32(&sk, 0);
    vk = aux[0];

    bls_sk tmp_share;
    aux_verify.resize(aux.size());
    string ss;
    cout << "reach here 1\n";
    for (int i = 0; i < P.nplayers(); i++)
    {
        cout << "i: " << i << endl;
        cout << "Who am i: " << P.whoami() << endl;
        if (i == P.whoami())
        {
            cout << "reach here 2\n";
            mclBnFr_add(&sk, &sk, &shs[i]);
            cout << "reach here 3\n";

            int j = 0;
            while (j < P.nplayers())
            {
                cout << "reach here 4\n";
                if (j != i)
                {
                    cout << "j: " << j << endl;
                    mclBnFr_to_str(ss, shs[j]);
                    cout<<ss.size()<<endl;
                    cout<<"test here\n";
                    P.send_to_player(j, ss, 1);
                    cout<<"test here here\n";
                }
                j++;
            }

            for (int k = 0; k < aux.size(); k++)
            {
                ss.clear();
                mclBnG1_to_str(ss, aux[k]);
                P.send_all(ss, 1, false);
            }
        }
        else
        {
            cout << "reach here 5\n";

            ss.clear();
            P.receive_from_player(i, ss, 1, false);
            cout << "reach here 6\n";

            str_to_mclBnFr(tmp_share, ss);
            mclBnFr_add(&sk, &sk, &tmp_share);
            cout << "reach here 7\n";

            for (int k = 0; k < aux.size(); k++)
            {
                ss.clear();
                P.receive_from_player(i, ss, 1, false);
                str_to_mclBnG1(aux_verify[k], ss);
            }
            if (!v.verify_share(tmp_share, aux_verify, P.whoami() + 1))
            {
                throw "shares not verified!";
            }
            mclBnG1_add(&vk, &vk, &aux_verify[0]);
        }
    }
}

void BLS::dis_sign(const string msg)
{
    sign(msg);
}

void BLS::combine_sigma()
{
}
