#include "poc.h"

void poc_setup(BLS &bls, Player &P)
{
    bls.dstb_keygen(P);
}

void poc_compute_enphem_key(G2_Affine_Coordinates &ac,
                            BLS &bls, const string &msg,
                            int online_num,
                            Player &P,
                            Config_Info &CI)
{
    Processor Proc(online_num, P.nplayers(), P);
    bls.dstb_sign(ac, msg, Proc, online_num, P, CI.OCD, CI.machine);
}

// pre_key = (s_1, s_0^2, s_1^3, s_0^4,...,)
void poc_compute_custody_bit_offline(vector<Share> &pre_key,
                                     const vector<Share> &keys,
                                     int online_num,
                                     Player &P,
                                     Config_Info &CI)
{
    if (keys.size() != 2)
    {
        throw bad_value();
    }

    Processor Proc(online_num, P.nplayers(), P);
    OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

    pre_key.resize(CHUNK_NUM);    
    Share s0_square, s1_square;

    online_op.sqr(s0_square, keys[0]);
    online_op.sqr(s1_square, keys[1]);

    pre_key[0] = keys[1];
    pre_key[1] = s0_square;

    for (int i = 2; i < CHUNK_NUM; i++)
    {
        if (i % 2 == 0)
        {
            online_op.mul(pre_key[i], pre_key[i - 2], s1_square);
        }
        else
        {
            online_op.mul(pre_key[i], pre_key[i - 2], s0_square);
        }
    }

    cout << "used triple: " << online_op.UT.UsedTriples << endl;
    cout << "used square: " << online_op.UT.UsedSquares << endl;
    cout << "used bit: " << online_op.UT.UsedBit << endl;
    cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

int poc_compute_custody_bit_online(const vector<Share> pre_key,
                                   const vector<gfp> &msg,
                                   int online_num,
                                   Player &P,
                                   Config_Info &CI)
{
    if (msg.size() != CHUNK_NUM)
    {
        throw bad_value();
    }
    Processor Proc(online_num, P.nplayers(), P);
    OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

    Share uhf_out;

    online_op.mul_plain(uhf_out, pre_key[0], msg[1]);
    online_op.add_plain_inplace(uhf_out, msg[0]);

    Share tmp;

    for (int i = 2; i < msg.size(); i++)
    {
        online_op.mul_plain(tmp, pre_key[i - 1], msg[i]);
        online_op.add_inplace(uhf_out, tmp);
    }

    online_op.add_inplace(uhf_out, pre_key.back());

    int res;
    res = online_op.legendre_prf(pre_key[0], uhf_out);

    cout << "used triple: " << online_op.UT.UsedTriples << endl;
    cout << "used square: " << online_op.UT.UsedSquares << endl;
    cout << "used bit: " << online_op.UT.UsedBit << endl;
    cout << "used input mask: " << online_op.UT.UsedInputMask << endl;

    return res;
}

int poc_compute_custody_bit(const vector<Share> &keys,
                            const vector<gfp> &msg,
                            int online_num,
                            Player &P,
                            Config_Info &CI)
{
    if (keys.size() != 2)
    {
        throw bad_value();
    }

    Processor Proc(online_num, P.nplayers(), P);
    Share uhf_out;
    int res;
    OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);
    online_op.uhf(uhf_out, keys[0], msg, CHUNK_NUM);
    res = online_op.legendre_prf(keys[1], uhf_out);

    cout << "used triple: " << online_op.UT.UsedTriples << endl;
    cout << "used square: " << online_op.UT.UsedSquares << endl;
    cout << "used bit: " << online_op.UT.UsedBit << endl;
    cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
    return res;
}