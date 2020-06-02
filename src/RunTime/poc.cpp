#include "poc.h"

void poc_setup(BLS& bls, Player& P) { bls.dstb_keygen(P); }

void poc_compute_ephem_key(
  G2_Affine_Coordinates& ac, BLS& bls, const string& msg, int online_num, Player& P,
  Config_Info& CI) {
  Processor Proc(online_num, P.nplayers(), P);
  bls.dstb_sign(ac, msg, Proc, online_num, P, CI.OCD, CI.machine);
}

// pre_key = (s_1, s_0^2, s_1^3, s_0^4,...,)
void poc_compute_custody_bit_offline(
  vector<Share>& pre_key, const vector<Share>& keys, int online_num, Player& P, Config_Info& CI) {
  if (keys.size() != 2) {
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

  for (int i = 2; i < CHUNK_NUM; i++) {
    if (i % 2 == 0) {
      online_op.mul(pre_key[i], pre_key[i - 2], s1_square);
    } else {
      online_op.mul(pre_key[i], pre_key[i - 2], s0_square);
    }
  }

  cout << "used triple: " << online_op.UT.UsedTriples << endl;
  cout << "used square: " << online_op.UT.UsedSquares << endl;
  cout << "used bit: " << online_op.UT.UsedBit << endl;
  cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

int poc_compute_custody_bit_online(
  const vector<Share> pre_key, const vector<gfp>& msg, int online_num, Player& P, Config_Info& CI) {
  if (msg.size() != CHUNK_NUM) {
    throw bad_value();
  }
  Processor Proc(online_num, P.nplayers(), P);
  OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

  Share uhf_out;

  online_op.mul_plain(uhf_out, pre_key[0], msg[1]);
  online_op.add_plain_inplace(uhf_out, msg[0]);

  Share tmp;

  for (int i = 2; i < msg.size(); i++) {
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

int poc_compute_custody_bit(
  const vector<Share>& keys, const vector<gfp>& msg, int online_num, Player& P, Config_Info& CI) {
  if (keys.size() != 2) {
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

void shared_rand_bits_phase_one(
  vector<Share>& shared_bits, vector<bigint>& local_bits, int online_num, Player& P,
  Config_Info& CI) {
  local_bits.resize(2 * PSIZE);
  shared_bits.resize(2 * PSIZE);

  vector<uint8_t> rnd(96); // 2*381/8
  P.G.get_random_bytes(rnd);
  for (int i = 0; i < local_bits.size(); i++) {
    local_bits[i] = (rnd[i / 8] >> (i % 8)) & 1;
  }

  Processor Proc(online_num, P.nplayers(), P);
  OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

  vector<Share> sbit(P.nplayers());
  for (int j = 0; j < 2 * PSIZE; j++) {
    for (int i = 0; i < sbit.size(); i++) {
      gfp tmp;
      tmp.assign(local_bits[j]);
      sbit[i].set_player(P.whoami());
      if (i == P.whoami()) {
        online_op.get_inputs(i, sbit[i], tmp);
      }
    }
    online_op.KXOR(shared_bits[j], sbit, sbit.size());
  }

  cout << "used triple: " << online_op.UT.UsedTriples << endl;
  cout << "used square: " << online_op.UT.UsedSquares << endl;
  cout << "used bit: " << online_op.UT.UsedBit << endl;
  cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

void decompose_and_reveal(
  vector<bigint>& reveal_bits, const vector<Share>& keys, const vector<Share>& shared_bits,
  int online_num, Player& P, Config_Info& CI) {
  if (keys.size() != 2 || shared_bits.size() != 2 * PSIZE) {
    throw invalid_size();
  }

  reveal_bits.resize(2 * PSIZE);

  Processor Proc(online_num, P.nplayers(), P);
  OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

  vector<Share> deco_bits, tmp;
  online_op.A2B(deco_bits, keys[0]);
  online_op.A2B(tmp, keys[1]);
  deco_bits.insert(deco_bits.end(), tmp.begin(), tmp.end());

  for (int i = 0; i < shared_bits.size(); i++) {
    online_op.XOR_inplace(deco_bits[i], shared_bits[i]);
  }

  vector<gfp> out;
  online_op.open(deco_bits, out);
  for (int i = 0; i < reveal_bits.size(); i++) {
    to_bigint(reveal_bits[i], out[i]);
  }

  cout << "used triple: " << online_op.UT.UsedTriples << endl;
  cout << "used square: " << online_op.UT.UsedSquares << endl;
  cout << "used bit: " << online_op.UT.UsedBit << endl;
  cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

void shared_rand_bits_phase_two(
  vector<Share>& shared_bits, const vector<bigint>& local_bits, int online_num, Player& P,
  Config_Info& CI) {
  shared_bits.resize(2 * PSIZE);
  Processor Proc(online_num, P.nplayers(), P);
  OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

  vector<Share> sbit(P.nplayers());
  for (int j = 0; j < 2 * PSIZE; j++) {
    for (int i = 0; i < sbit.size(); i++) {
      gfp tmp;
      tmp.assign(local_bits[j]);
      sbit[i].set_player(P.whoami());
      if (i == P.whoami()) {
        online_op.get_inputs(i, sbit[i], tmp);
      }
    }
    online_op.KXOR(shared_bits[j], sbit, sbit.size());
  }

  cout << "used triple: " << online_op.UT.UsedTriples << endl;
  cout << "used square: " << online_op.UT.UsedSquares << endl;
  cout << "used bit: " << online_op.UT.UsedBit << endl;
  cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

void xor_and_combine(
  vector<Share>& keys, const vector<Share>& shared_bits, const vector<bigint>& reveal_bits,
  int online_num, Player& P, Config_Info& CI) {
  if (shared_bits.size() != 3 * QSIZE || reveal_bits.size() != 3 * QSIZE) {
    throw invalid_size();
  }

  keys.resize(3);
  Processor Proc(online_num, P.nplayers(), P);
  OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

  vector<Share> tmp(shared_bits.size());
  for (int i = 0; i < shared_bits.size(); i++) {
    online_op.XOR_plain(tmp[i], shared_bits[i], reveal_bits[i]);
  }

  vector<Share> out;
  for (int i = 0; i < keys.size(); i++) {
    out.insert(out.begin(), tmp.begin() + i * QSIZE, tmp.begin() + (i + 1) * QSIZE);
    online_op.B2A(keys[i], out, QSIZE);
    out.clear();
  }
  tmp.clear();

  cout << "used triple: " << online_op.UT.UsedTriples << endl;
  cout << "used square: " << online_op.UT.UsedSquares << endl;
  cout << "used bit: " << online_op.UT.UsedBit << endl;
  cout << "used input mask: " << online_op.UT.UsedInputMask << endl;
}

int poc_compute_custody_bit_online_2primes(
  const vector<Share> pre_key, const Share key, const vector<gfp>& msg, int online_num, Player& P,
  Config_Info& CI) {
  if (msg.size() != CHUNK_NUM) {
    throw bad_value();
  }
  Processor Proc(online_num, P.nplayers(), P);
  OnlineOp online_op(Proc, online_num, P, CI.OCD, CI.machine);

  Share uhf_out;

  online_op.mul_plain(uhf_out, pre_key[0], msg[1]);
  online_op.add_plain_inplace(uhf_out, msg[0]);

  Share tmp;

  for (int i = 2; i < msg.size(); i++) {
    online_op.mul_plain(tmp, pre_key[i - 1], msg[i]);
    online_op.add_inplace(uhf_out, tmp);
  }

  online_op.add_inplace(uhf_out, pre_key.back());

  int res;
  res = online_op.legendre_prf(key, uhf_out);

  cout << "used triple: " << online_op.UT.UsedTriples << endl;
  cout << "used square: " << online_op.UT.UsedSquares << endl;
  cout << "used bit: " << online_op.UT.UsedBit << endl;
  cout << "used input mask: " << online_op.UT.UsedInputMask << endl;

  return res;
}