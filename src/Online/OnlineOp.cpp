/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include <unistd.h>
#include <math.h>

//#include "Online.h"
#include "Processor/Processor.h"
#include "OnlineOp.h"

void OnlineOp::getTuples(vector<Share>& sp, int opcode) {
  int thread = Proc.get_thread_num();
  Wait_For_Preproc(opcode, 1, thread, OCD);
  for (int i = 0; i < sp.size(); i++) {
    sp[i].set_player(P.whoami());
  }
  switch (opcode) {
    case TRIPLE:
      if (sp.size() != 3)
        throw invalid_size();
      OCD.mul_mutex[thread].lock();
      sp[0] = SacrificeD[thread].TD.ta.front();
      SacrificeD[thread].TD.ta.pop_front();
      sp[1] = SacrificeD[thread].TD.tb.front();
      SacrificeD[thread].TD.tb.pop_front();
      sp[2] = SacrificeD[thread].TD.tc.front();
      SacrificeD[thread].TD.tc.pop_front();
      OCD.mul_mutex[thread].unlock();
      UT.UsedTriples++;
      break;
    case SQUARE:
      if (sp.size() != 2)
        throw invalid_size();
      OCD.sqr_mutex[thread].lock();
      sp[0] = SacrificeD[thread].SD.sa.front();
      SacrificeD[thread].SD.sa.pop_front();
      sp[1] = SacrificeD[thread].SD.sb.front();
      SacrificeD[thread].SD.sb.pop_front();
      OCD.sqr_mutex[thread].unlock();
      UT.UsedSquares++;
      break;
    case BIT:
      if (sp.size() != 1)
        throw invalid_size();
      OCD.bit_mutex[thread].lock();
      sp[0] = SacrificeD[thread].BD.bb.front();
      SacrificeD[thread].BD.bb.pop_front();
      OCD.bit_mutex[thread].unlock();
      UT.UsedBit++;
      break;
    default:
      throw bad_value();
      break;
  }
}
// c = a + b (b is share)
void OnlineOp::add(Share& c, const Share& a, const Share& b) {
  c = a + b;
}

void OnlineOp::add_plain(Share& c, const Share& a, const gfp& b) {
  c.add(a, b);
}

void OnlineOp::add_inplace(Share& c, const Share& a) {
  Share tmp;
  add(tmp, c, a);
  c = tmp;
}

void OnlineOp::add_plain_inplace(Share& c, const gfp& a) {
  add_plain(c, c, a);
}

// c = a - b (b is share)
void OnlineOp::sub(Share& c, const Share& a, const Share& b) {
  c = a - b;
}
// c = a - b (b is plain)
void OnlineOp::sub_plain(Share& c, const Share& a, const gfp& b) {
  c.sub(a, b);
}

void OnlineOp::sub_inplace(Share& c, const Share& a) {
  Share tmp;
  sub(tmp, c, a);
  c = tmp;
}

void OnlineOp::mul_plain(Share& c, const Share& a, const gfp& b) {
  c.mul(a, b);
}
// a * b = c
void OnlineOp::mul(Share& c, const Share& a, const Share& b) {
  //phase 0: get triples
  vector<Share> sp(3);
  getTuples(sp, TRIPLE);

  //phase 1: sub and open

  Share epsilon = a - sp[0]; // epsilon = x - a
  Share rho = b - sp[1]; // rho = y - b
  vector<gfp> vc;
  open({epsilon, rho}, vc);

  //phase 2 get mul share

  Share tmp;
  mul_plain(tmp, sp[1], vc[0]);
  add_inplace(sp[2], tmp);

  mul_plain(tmp, sp[0], vc[1]);
  add_inplace(sp[2], tmp);

  gfp cp0 = vc[0] * vc[1];
  add_plain(c, sp[2], cp0);
}

void OnlineOp::mul_inplace(Share& c, const Share& a) {
  Share tmp;
  mul(tmp, c, a);
  c = tmp;
}
// aa = a^2
void OnlineOp::sqr(Share& aa, const Share& a) {
  //phase 0: get triples
  vector<Share> sp(2);
  getTuples(sp, SQUARE);

  //phase 1: sub and open

  Share epsilon = a - sp[0]; // epsilon = x - a
  vector<gfp> vc;
  open({epsilon}, vc);

  //phase 2 get squre share

  Share tmp;
  mul_plain(tmp, sp[0], vc[0]);
  add_inplace(sp[1], tmp);
  add_inplace(sp[1], tmp);

  gfp cp0 = vc[0] * vc[0];
  add_plain(aa, sp[1], cp0);
}

void OnlineOp::sqr_inplace(Share& a) {
  Share tmp;
  sqr(tmp, a);
  a = tmp;
}

void OnlineOp::inv(Share& ia, const Share& a) {
  Share rshare;
  // get random share r
  if (prss.asets_size() != 0) {
    rshare = prss.next_share(P);
  } else {
    vector<Share> tmp(2);
    getTuples(tmp, SQUARE);
    rshare = tmp[0];
  }
  //reveal_and_print({rshare});

  Share c;
  //get share of r*a;
  mul(c, rshare, a);
  vector<gfp> ra;

  //open ra;
  open({c}, ra);
  if (ra[0].is_zero()) {
    throw Processor_Error("Division by zero");
  }

  //invert ra;
  gfp ira;
  ira.invert(ra[0]);
  mul_plain(ia, rshare, ira);
}

void OnlineOp::inv_inplace(Share& a) {
  Share tmp;
  inv(tmp, a);
  a = tmp;
}

// c = a * b^{-1} mod q
void OnlineOp::div(Share& c, const Share& a, const Share& b) {
  inv(c, b);
  mul_inplace(c, a);
}

void OnlineOp::div_inplace(Share& c, const Share& a) {
  Share tmp;
  div(tmp, c, a);
  c = tmp;
}

/*Complex ops*/
// c = a + b (b is shared complex)
void OnlineOp::add(Complex& c, const Complex& a, const Complex& b) {
  add(c.real, a.real, b.real);
  add(c.imag, a.imag, b.imag);
}

// c = a + b (b is plain complex)
void OnlineOp::add_plain(Complex& c, const Complex& a, const Complex_Plain& b) {
  add_plain(c.real, a.real, b.real);
  add_plain(c.imag, a.imag, b.imag);
}

void OnlineOp::add_inplace(Complex& c, const Complex& a) {
  Complex tmp;
  add(tmp, c, a);
  c = tmp;
}

// c = a - b (b is shared complex)
void OnlineOp::sub(Complex& c, const Complex& a, const Complex& b) {
  sub(c.imag, a.imag, b.imag);
  sub(c.real, a.real, b.real);
}
// c = a - b (b is plain complex)
void OnlineOp::sub_plain(Complex& c, const Complex& a, const Complex_Plain& b) {
  sub_plain(c.imag, a.imag, b.imag);
  sub_plain(c.real, a.real, b.real);
}

void OnlineOp::sub_inplace(Complex& c, const Complex& a) {
  Complex tmp;
  sub(tmp, c, a);
  c = tmp;
}

// c = a * b (b is plain complex)
void OnlineOp::mul_plain(Complex& c, const Complex& a, const Complex_Plain& b) {
  Share tmpimag, tmpreal, creal;
  mul_plain(tmpreal, a.real, b.real);
  mul_plain(tmpimag, a.imag, b.imag);
  sub(creal, tmpreal, tmpimag);

  mul_plain(tmpreal, a.real, b.imag);
  mul_plain(tmpimag, a.imag, b.real);
  add(c.imag, tmpreal, tmpimag);
  c.real = creal;
}
// c = a * b (b is shared complex)
void OnlineOp::mul(Complex& c, const Complex& a, const Complex& b) {
  Share tmp;
  mul(c.real, a.real, b.real);
  mul(tmp, a.imag, b.imag);
  sub_inplace(c.real, tmp);

  mul(c.imag, a.real, b.imag);
  mul(tmp, a.imag, b.real);
  add_inplace(c.imag, tmp);
}

void OnlineOp::mul_inplace(Complex& c, const Complex& a) {
  Complex tmp;
  mul(tmp, c, a);
  c = tmp;
}
// aa = a^2
void OnlineOp::sqr(Complex& aa, const Complex& a) {
  sqr(aa.real, a.real);
  sqr(aa.imag, a.imag);
  sub_inplace(aa.real, aa.imag);

  mul(aa.imag, a.real, a.imag);
  add_inplace(aa.imag, aa.imag);
}

void OnlineOp::sqr_inplace(Complex& a) {
  Complex tmp;
  sqr(tmp, a);
  a = tmp;
}

//ia = a^{-1} mod (q,x^2+1)
void OnlineOp::inv(Complex& ia, const Complex a) {
  Share rr;
  sqr(ia.real, a.real);
  sqr(ia.imag, a.imag);

  add_inplace(ia.real, ia.imag);
  rr = ia.real;

  inv_inplace(rr); // rr = (real^2+imag^2)^{-1}

  mul(ia.real, a.real, rr);
  rr.negate();
  mul(ia.imag, a.imag, rr);
}

void OnlineOp::inv_inplace(Complex& a) {
  Complex tmp;
  inv(tmp, a);
  a = tmp;
}

// c = a * b^{-1} mod (q, x^2+1)
void OnlineOp::div(Complex& c, const Complex& a, const Complex& b) {
  inv(c, b);
  mul_inplace(c, a);
}

void OnlineOp::div_inplace(Complex& c, const Complex& a) {
  Complex tmp;
  div(tmp, c, a);
  c = tmp;
}

void OnlineOp::uhf(Share& out, const Share& key, const vector<gfp>& in, unsigned int size) {
  if (in.size() != size) {
    throw bad_value();
  }

  mul_plain(out, key, in[size - 1]);
  add_plain_inplace(out, in[size - 2]);
  for (int i = size - 3; i >= 0; i--) {
    mul_inplace(out, key);
    add_plain_inplace(out, in[i]);
  }
}

void OnlineOp::legendre(int& out, const Share& in) {
  vector<Share> s(2);
  getTuples(s, SQUARE);

  mul_inplace(s[1], in);

  vector<gfp> c;
  open({s[1]}, c);

  bigint bn;
  to_bigint(bn, c[0]);
  out = mpz_legendre(bn.get_mpz_t(), gfp::pr().get_mpz_t());
}

int OnlineOp::legendre_prf(const Share& key, const Share& in) {
  Share tmp;
  add(tmp, key, in);
  int res;
  legendre(res, tmp);
  res = ceil(double(res + 1) / 2);
  return res;
}

void OnlineOp::open(const vector<Share>& vs, vector<gfp>& vc) {
  vector<int> start;
  size_t size = 1;
  vc.clear();
  vc.resize(vs.size());

  Proc.POpen_Start(start, vs, size, P);
  Proc.POpen_Stop(start, vc, size, P);
}

void OnlineOp::reveal(const vector<Share>& vs, vector<gfp>& vc) {
  open(vs, vc);
}
void OnlineOp::reveal_and_print(const vector<Share>& vs, vector<gfp>& vc) {
  reveal(vs, vc);
  for (int i = 0; i < vc.size(); i++) {
    cout << vc[i] << endl;
  }
}
void OnlineOp::reveal_and_print(const vector<Share>& vs) {
  vector<gfp> vc;
  reveal_and_print(vs, vc);
}

void OnlineOp::reveal_and_print(const vector<Complex>& vs) {
  for (int i = 0; i < vs.size(); i++) {
    cout << "real and imag are: \n";
    reveal_and_print({vs[i].real, vs[i].imag});
  }
}

void OnlineOp::get_inputs(unsigned int party, Share& sa, gfp& inputs) {
  Proc.iop.private_input(party, sa, 1, Proc, P, machine, OCD, inputs);
  UT.UsedInputMask++;
}

void OnlineOp::get_inputs(unsigned int party, Complex& sa, Complex_Plain& inputs) {
  get_inputs(party, sa.real, inputs.real);
  get_inputs(party, sa.imag, inputs.imag);
}

// the following apis for testing

void OnlineOp::test_uhf() {
  cout << "============================== BEG " << __FUNCTION__
       << " ==============================" << endl;
  gfp k;
  k.assign(2);
  Share key;
  get_inputs(0, key, k);
  vector<gfp> in(4);
  for (int i = 0; i < in.size(); i++) {
    in[i].assign(i + 1);
  }

  Share out;
  uhf(out, key, in, in.size());

  reveal_and_print({out});
}

void OnlineOp::test_legendre() {
  cout << "============================== BEG " << __FUNCTION__
       << " ==============================" << endl;
  string k("123"), d("-12456789");

  bigint key(k, 10);
  bigint data(d, 10);

  gfp key_p, data_p;
  to_gfp(key_p, key);
  to_gfp(data_p, data);

  Share key_s, data_s;
  get_inputs(0, key_s, key_p);
  get_inputs(0, data_s, data_p);

  int res_s = legendre_prf(key_s, data_s);

  bigint in = key + data;

  int res = mpz_legendre(in.get_mpz_t(), gfp::pr().get_mpz_t());
  res = ceil(double(res + 1) / 2);

  cout << "mpc leg value: " << res_s << endl;
  cout << "plain leg value: " << res << endl;
}

void OnlineOp::test_get_inputs() {
  cout << "============================== BEG " << __FUNCTION__
       << " ==============================" << endl;

  gfp a, b;
  a.assign(10);
  b.assign(20);
  Complex_Plain cp(a, b);
  Complex cc;
  get_inputs(0, cc, cp);

  reveal_and_print({cc});
  cout << "============================== END " << __FUNCTION__
       << " ==============================" << endl;
}
void OnlineOp::test_mul() {
  cout << "============================== BEG " << __FUNCTION__
       << " ==============================" << endl;

  gfp a, b;
  Share aa, bb, cc;
  if (P.whoami() == 0) {
    a.assign(3);
  }
  if (P.whoami() == 1) {
    b.assign(4);
  }
  get_inputs(0, aa, a);
  get_inputs(1, bb, b);
  mul(cc, aa, bb);
  reveal_and_print({aa, bb, cc});

  cout << "============================== END " << __FUNCTION__
       << " ==============================" << endl;
}