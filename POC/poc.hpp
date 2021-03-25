#include "poc.h"

template <class T>
void POC<T>::poc_setup(BLS<T> &bls, Player &P)
{
    bls.dstb_keygen(P);
}

template <class T>
void POC<T>::poc_compute_ephem_key(G2_Affine_Coordinates<T> &out, BLS<T> &bls, const string &msg, int online_num, Player &P, Config_Info &CI)
{
    bls.dstb_sign(out, msg, P, protocol, preprocessing, processor, output);
}

// pre_key = (s_1, s_0^2, s_1^3, s_0^4,...,)
template <class T>
void POC<T>::poc_compute_custody_bit_offline(vector<T> &pre_key, const vector<T> &keys, int online_num, Player &P, Config_Info &CI)
{
    if (keys.size() != 2)
    {
        throw invalid_length();
    }

    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    pre_key.resize(CHUNK_NUM);
    T s0_square, s1_square;

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

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

template <class T>
int POC<T>::poc_compute_custody_bit_online(const vector<T> pre_key, const vector<clear> &msg, int online_num, Player &P, Config_Info &CI)
{
    if (msg.size() != CHUNK_NUM)
    {
        throw invalid_length();
    }
    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    T uhf_out;

    online_op.mul_plain(uhf_out, pre_key[0], msg[1]);
    online_op.add_plain_inplace(uhf_out, msg[0]);

    T tmp;

    for (int i = 2; i < msg.size(); i++)
    {
        online_op.mul_plain(tmp, pre_key[i - 1], msg[i]);
        online_op.add_inplace(uhf_out, tmp);
    }

    online_op.add_inplace(uhf_out, pre_key.back());

    int res;
    res = online_op.legendre_prf(pre_key[0], uhf_out);

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;

    return res;
}

template <class T>
int POC<T>::poc_compute_custody_bit(const vector<T> &keys, const vector<clear> &msg, int online_num, Player &P, Config_Info &CI)
{
    if (keys.size() != 2)
    {
        throw invalid_length();
    }

    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    T uhf_out;
    int res;
    online_op.uhf(uhf_out, keys[0], msg, CHUNK_NUM);
    res = online_op.legendre_prf(keys[1], uhf_out);

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;

    return res;
}

template <class T>
void POC<T>::shared_rand_bits_phase_one(vector<T> &shared_bits, vector<bigint> &local_bits, int online_num, Player &P,
                                        Config_Info &CI)
{
    local_bits.resize(2 * PSIZE);
    shared_bits.resize(2 * PSIZE);

    vector<uint8_t> rnd(96); // 2*381/8
    //*RC*// P.G.get_random_bytes(rnd);
    PRNG prng;
    prng.ReSeed();
    prng.get_octets((octet *)rnd.data(), sizeof(uint8_t) * rnd.size());

    for (int i = 0; i < local_bits.size(); i++)
    {
        local_bits[i] = (rnd[i / 8] >> (i % 8)) & 1;
    }

    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    vector<T> sbit(P.num_players());
    for (int j = 0; j < 2 * PSIZE; j++)
    {
        for (int i = 0; i < sbit.size(); i++)
        {
            clear tmp;
            // tmp.assign(local_bits[j]);
            tmp = local_bits[j];
            //sbit[i].set_player(P.whoami());
            //if (i == P.whoami()) {
            online_op.get_inputs(i, sbit[i], tmp);
            //}
        }
        online_op.KXOR(shared_bits[j], sbit, sbit.size());
    }

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}
template <class T>
void POC<T>::decompose_and_reveal(
    vector<bigint> &reveal_bits, const vector<T> &keys, const vector<T> &shared_bits,
    int online_num, Player &P, Config_Info &CI)
{
    if (keys.size() != 2 || shared_bits.size() != 2 * PSIZE)
    {
        throw invalid_length();
    }

    PRINT_DEBUG_INFO();
    reveal_bits.resize(2 * PSIZE);

    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);
    PRINT_DEBUG_INFO();

    vector<T> deco_bits, tmp;
    PRINT_DEBUG_INFO();
    online_op.A2B(deco_bits, keys[0]);
    PRINT_DEBUG_INFO();
    online_op.A2B(tmp, keys[1]);
    PRINT_DEBUG_INFO();
    deco_bits.insert(deco_bits.end(), tmp.begin(), tmp.end());

    PRINT_DEBUG_INFO();
    for (int i = 0; i < shared_bits.size(); i++)
    {
        online_op.XOR_inplace(deco_bits[i], shared_bits[i]);
    }

    PRINT_DEBUG_INFO();
    vector<clear> out;
    online_op.reveal(deco_bits, out);
    for (int i = 0; i < reveal_bits.size(); i++)
    {
        to_bigint(reveal_bits[i], out[i]);
    }
    PRINT_DEBUG_INFO();

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

template <class T>
void POC<T>::shared_rand_bits_phase_two(
    vector<T> &shared_bits, const vector<bigint> &local_bits, int online_num, Player &P,
    Config_Info &CI)
{
    shared_bits.resize(2 * PSIZE);
    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    vector<T> sbit(P.num_players());
    for (int j = 0; j < 2 * PSIZE; j++)
    {
        for (int i = 0; i < sbit.size(); i++)
        {
            clear tmp;
            // tmp.assign(local_bits[j]);
            tmp = local_bits[j];
            //sbit[i].set_player(P.whoami());
            //if (i == P.whoami()) {
            online_op.get_inputs(i, sbit[i], tmp);
            //}
        }
        online_op.KXOR(shared_bits[j], sbit, sbit.size());
    }

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

template <class T>
void POC<T>::xor_and_combine(
    vector<T> &keys, const vector<T> &shared_bits, const vector<bigint> &reveal_bits,
    int online_num, Player &P, Config_Info &CI)
{
    if (shared_bits.size() != 3 * QSIZE || reveal_bits.size() != 3 * QSIZE)
    {
        throw invalid_length();
    }

    keys.resize(3);
    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    vector<T> tmp(shared_bits.size());
    for (int i = 0; i < shared_bits.size(); i++)
    {
        online_op.XOR_plain(tmp[i], shared_bits[i], reveal_bits[i]);
    }

    vector<T> out;
    for (int i = 0; i < keys.size(); i++)
    {
        out.insert(out.begin(), tmp.begin() + i * QSIZE, tmp.begin() + (i + 1) * QSIZE);
        online_op.B2A(keys[i], out, QSIZE);
        out.clear();
    }
    tmp.clear();

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

template <class T>
void POC<T>::poc_compute_custody_bit_offline_2primes(
    vector<T> &pre_key, const vector<T> &keys, int online_num, Player &P, Config_Info &CI)
{
    if (keys.size() != 3)
    {
        throw bad_value();
    }

    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);
    PRINT_DEBUG_INFO();

    pre_key.resize(CHUNK_NUM);
    T s0_cube, s1_cube, s2_cube;
    online_op.sqr(s0_cube, keys[0]);
    online_op.mul_inplace(s0_cube, keys[0]);

    online_op.sqr(s1_cube, keys[1]);
    online_op.mul_inplace(s1_cube, keys[1]);

    online_op.sqr(s2_cube, keys[2]);
    pre_key[1] = s2_cube;
    online_op.mul_inplace(s2_cube, keys[2]);

    pre_key[0] = keys[1];
    pre_key[2] = s0_cube;
    // s1, s2^2, s0^3, s1^4, s2^5, s0^6, constant not considered

    PRINT_DEBUG_INFO();
    for (int i = 3; i < CHUNK_NUM; i++)
    {
        if (i % 1000 == 0)
            PRINT_DEBUG_INFO();
        if (i % 3 == 0)
        {
            online_op.mul(pre_key[i], pre_key[i - 3], s1_cube);
        }
        else if (i % 3 == 1)
        {
            online_op.mul(pre_key[i], pre_key[i - 3], s2_cube);
        }
        else if (i % 3 == 2)
        {
            online_op.mul(pre_key[i], pre_key[i - 3], s0_cube);
        }
    }

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

template <class T>
int POC<T>::poc_compute_custody_bit_online_2primes(
    const vector<T> pre_key, const T key, const vector<clear> &msg, int online_num, Player &P,
    Config_Info &CI)
{
    if (msg.size() != CHUNK_NUM)
    {
        throw bad_value();
    }
    OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);

    T uhf_out;

    online_op.mul_plain(uhf_out, pre_key[0], msg[1]);
    online_op.add_plain_inplace(uhf_out, msg[0]); // m[0] + m[1]*s1

    T tmp;

    for (int i = 2; i < msg.size(); i++)
    {
        online_op.mul_plain(tmp, pre_key[i - 1], msg[i]);
        online_op.add_inplace(uhf_out, tmp);
    }

    online_op.add_inplace(uhf_out, pre_key.back());

    int res = 0;
    for (int i = 0; i < 10; i++)
    {
        clear count(i);
        T in;
        online_op.add_plain(in, uhf_out, count);
        res |= online_op.legendre_prf(key, in);
    }

    //*RC*// cout << "used triple: " << online_op.UT.UsedTriples << endl;
    //*RC*// cout << "used square: " << online_op.UT.UsedSquares << endl;
    //*RC*// cout << "used bit: " << online_op.UT.UsedBit << endl;
    //*RC*// cout << "used input mask: " << online_op.UT.UsedInputMask << endl;

    return res;
}
