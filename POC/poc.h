#pragma once

#include "bls.h"
#include <string>
#include <iomanip>
using namespace std;

//#define CHUNK_NUM (128 * 1024 / 48 + 1)
#define CHUNK_NUM (256 * 1024 / 32) //8192

class Config_Info
{
public:
    int version = 0;
    Config_Info() {}
};

template <class T>
class POC
{
protected:
    Player &P;
    typename T::Protocol &protocol;
    typename T::LivePrep &preprocessing;
    SubProcessor<T> &processor;
    typename T::MAC_Check &output;
    typedef typename T::clear clear;

public:
    POC(Player &_P, typename T::Protocol &_protocol, typename T::LivePrep &_preprocessing,
        SubProcessor<T> &_processor, typename T::MAC_Check &_output)
        : P(_P), protocol(_protocol), preprocessing(_preprocessing), processor(_processor), output(_output)
    {
    }

public:
    /*
    The distributed setup stage to generate private signing key 
    shares of BLS along with the public key.
    It is essentially the distrbuted key generation algorithm.
    */
    void poc_setup(BLS<T> &bls, Player &P);

    /*
    Ephermeral key for UHF and legendre prf.
    It is essentially the distributed signing algorithms.
    */
    void poc_compute_ephem_key(G2_Affine_Coordinates<T> &out, BLS<T> &bls, const string &msg, int online_num, Player &P, Config_Info &CI);

    /*
    Compute UHF and legendre prf.
    */
    void poc_compute_custody_bit_offline(
        vector<T> &pre_key, const vector<T> &keys, int online_num, Player &P, Config_Info &CI);

    int poc_compute_custody_bit_online(
        const vector<T> pre_key, const vector<clear> &msg, int online_num, Player &P, Config_Info &CI);

    //run offline and online in one step
    int poc_compute_custody_bit(
        const vector<T> &keys, const vector<clear> &msg, int online_num, Player &P, Config_Info &CI);

    //the 2-primes version
    void shared_rand_bits_phase_one(
        vector<T> &shared_bits, vector<bigint> &local_bits, int online_num, Player &P,
        Config_Info &CI);

    void decompose_and_reveal(
        vector<bigint> &reveal_bits, const vector<T> &keys, const vector<T> &shared_bits,
        int online_num, Player &P, Config_Info &CI);

    void shared_rand_bits_phase_two(
        vector<T> &shared_bits, const vector<bigint> &local_bits, int online_num, Player &P,
        Config_Info &CI);

    void xor_and_combine(
        vector<T> &keys, const vector<T> &shared_bits, const vector<bigint> &reveal_bits,
        int online_num, Player &P, Config_Info &CI);

    void poc_compute_custody_bit_offline_2primes(
        vector<T> &pre_key, const vector<T> &keys, int online_num, Player &P, Config_Info &CI);

    int poc_compute_custody_bit_online_2primes(
        const vector<T> pre_key, const T key, const vector<clear> &msg, int online_num, Player &P,
        Config_Info &CI);
};

#include "poc.hpp"
