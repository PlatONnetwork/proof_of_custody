#pragma once

#include "poc.h"
#include <openssl/ssl.h>

template <class T>
class RunPOC
{
protected:
  POC<T> &poc;
  Player &P;
  typename T::Protocol &protocol;
  typename T::LivePrep &preprocessing;
  SubProcessor<T> &processor;
  typename T::MAC_Check &output;
  typedef typename T::clear clear;

public:
  RunPOC(POC<T> &_poc, Player &_P, typename T::Protocol &_protocol, typename T::LivePrep &_preprocessing,
         SubProcessor<T> &_processor, typename T::MAC_Check &_output)
      : poc(_poc), P(_P), protocol(_protocol), preprocessing(_preprocessing), processor(_processor), output(_output)
  {
  }

public:
  /*
  ** Call chain:
  ** init -> setup -> offline -> ephemkey/genproof/online -> wait -> clear
  */

  /*
  * Initate the configure information
  */
  void run_init(int argc, char *argv[], Config_Info &CI);
  void run_poc_setup(BLS<T> &bls, Config_Info &CI);
  void run_offline(Config_Info &CI);

  void run_poc_compute_ephem_key(vector<T> &ek, BLS<T> &bls, const string &msg, Config_Info &CI);
  void run_poc_compute_custody_bit_offline(
      vector<T> &pre_key, const vector<T> &keys, Config_Info &CI);
  int run_poc_compute_custody_bit_online(
      const vector<T> &pre_key, const vector<clear> &msg, Config_Info &CI);

  //run offline and online in one step
  int run_poc_compute_custody_bit(const vector<T> &keys, const vector<clear> &msg, Config_Info &CI);
  //===========================================
  //===========================================

  // the 2 primes version
  void run_poc_compute_ephem_key_2primes_phase_one(
      vector<bigint> &local_bits, vector<bigint> &reveal_bits, BLS<T> &bls, const string &msg,
      Config_Info &CI);

  void run_poc_compute_ephem_key_2primes_phase_two(
      vector<T> &ek, const vector<bigint> &local_bits, const vector<bigint> &reveal_bits,
      Config_Info &CI);

  void run_poc_compute_custody_bit_offline_2primes(
      vector<T> &pre_key, const vector<T> &keys, Config_Info &CI);
  int run_poc_compute_custody_bit_online_2primes(
      const vector<T> &pre_key, const T &key, const vector<clear> &msg, Config_Info &CI);
};

#include "RunPoc.hpp"

