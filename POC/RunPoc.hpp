#include "RunPoc.h"

template <class T>
void RunPOC<T>::run_init(int argc, char *argv[], Config_Info &CI)
{
}

template <class T>
void RunPOC<T>::run_poc_setup(BLS<T> &bls, Config_Info &CI)
{
  cout << "----------Begin of setup-----------------------------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocSetup].player);

  Timer timer;
  timer.start();
  poc.poc_setup(bls, P);
  timer.stop();
  // stats[ThreadPlayer::TP_PocSetup].set(P, timer.elapsed()).print(CI.verbose);

  // if (CI.verbose > 0)
  // {
  //   cout << "secre key share: " << endl;
  //   print_mclBnFr(bls.get_sk());

  //   cout << "public key: " << endl;
  //   print_mclBnG1(bls.vk);
  // }
  cout << "----------End of setup-------------------------------" << endl;
}

template <class T>
void RunPOC<T>::run_poc_compute_ephem_key(vector<T> &ek, BLS<T> &bls, const string &msg, Config_Info &CI)
{
  cout << "----------Begin of compute_enphem_key----------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);

  Timer timer;
  timer.start();

  G2_Affine_Coordinates<T> ac;
  poc_compute_ephem_key(ac, bls, msg, 0, P, CI);
  ek.resize(4);
  ek[0] = ac.x.real;
  ek[1] = ac.x.imag;
  ek[2] = ac.y.real;
  ek[3] = ac.y.imag;

  timer.stop();
  // stats[ThreadPlayer::TP_PocEphemKey].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_enphem_key------------" << endl;
}

template <class T>
void RunPOC<T>::run_poc_compute_custody_bit_offline(
    vector<T> &pre_key, const vector<T> &keys, Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit_offline-------------------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocGenProofPre].player);

  Timer timer;
  timer.start();
  poc_compute_custody_bit_offline(pre_key, keys, 0, P, CI);
  timer.stop();
  // stats[ThreadPlayer::TP_PocGenProofPre].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_offline-------------------" << endl;
}

template <class T>
int RunPOC<T>::run_poc_compute_custody_bit_online(
    const vector<T> &pre_key, const vector<clear> &msg, Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit_online-------------------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  Timer timer;
  timer.start();
  int res = poc_compute_custody_bit_online(pre_key, msg, 0, P, CI);
  timer.stop();
  // stats[ThreadPlayer::TP_PocGenProof].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_online-------------------" << endl;

  return res;
}

template <class T>
int RunPOC<T>::run_poc_compute_custody_bit(
    const vector<T> &keys, const vector<clear> &msg, Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit-------------------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  Timer timer;
  timer.start();
  int res = poc_compute_custody_bit(keys, msg, 0, P, CI);
  timer.stop();
  // stats[ThreadPlayer::TP_PocGenProof].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit---------------------" << endl;
  return res;
}

//======================
//======================
// the 2 primes version

template <class T>
void RunPOC<T>::run_poc_compute_ephem_key_2primes_phase_one(
    vector<bigint> &local_bits, vector<bigint> &reveal_bits, BLS<T> &bls, const string &msg,
    Config_Info &CI)
{
  cout << "----------Begin of compute_enphem_key in 2 primes version phase_one----------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);

  Timer timer;
  timer.start();

  G2_Affine_Coordinates<T> ac;
  poc.poc_compute_ephem_key(ac, bls, msg, 0, P, CI);
  vector<T> ek_tmp(2);
  ek_tmp[0] = ac.x.real;
  ek_tmp[1] = ac.x.imag;

  vector<T> shared_bits;
  PRINT_DEBUG_INFO();
  poc.shared_rand_bits_phase_one(shared_bits, local_bits, 0, P, CI);
  PRINT_DEBUG_INFO();
  poc.decompose_and_reveal(reveal_bits, ek_tmp, shared_bits, 0, P, CI);
  PRINT_DEBUG_INFO();

  timer.stop();
  // stats[ThreadPlayer::TP_PocEphemKey].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_enphem_key in 2 primes version phase_one------------" << endl;
}

template <class T>
void RunPOC<T>::run_poc_compute_ephem_key_2primes_phase_two(
    vector<T> &ek, const vector<bigint> &local_bits, const vector<bigint> &reveal_bits,
    Config_Info &CI)
{
  cout << "----------Begin of compute_enphem_key in 2 primes version phase_two----------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);
  Timer timer;
  timer.start();

  vector<T> shared_bits;
  poc.shared_rand_bits_phase_two(shared_bits, local_bits, 0, P, CI);
  poc.xor_and_combine(ek, shared_bits, reveal_bits, 0, P, CI);

  timer.stop();
  // stats[ThreadPlayer::TP_PocEphemKey].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_enphem_key in 2 primes version phase_two------------" << endl;
}

template <class T>
void RunPOC<T>::run_poc_compute_custody_bit_offline_2primes(
    vector<T> &pre_key, const vector<T> &keys, Config_Info &CI)
{
  //  run_poc_compute_custody_bit_offline(pre_key, keys, CI);
  cout << "----------Begin of compute_custody_bit_offline_2primes-------------------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocGenProofPre].player);

  Timer timer;
  timer.start();
  poc.poc_compute_custody_bit_offline_2primes(pre_key, keys, 0, P, CI);
  timer.stop();
  // stats[ThreadPlayer::TP_PocGenProofPre].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_offline_2primes-------------------" << endl;
}

template <class T>
int RunPOC<T>::run_poc_compute_custody_bit_online_2primes(
    const vector<T> &pre_key, const T &key, const vector<clear> &msg, Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit_online_2primes-------------------" << endl;
  // Player &P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  Timer timer;
  timer.start();
  int res = poc.poc_compute_custody_bit_online_2primes(pre_key, key, msg, 0, P, CI);
  timer.stop();
  // stats[ThreadPlayer::TP_PocGenProof].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_online_primes-------------------" << endl;

  return res;
}
