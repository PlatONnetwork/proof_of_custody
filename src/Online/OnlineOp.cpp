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
    default: throw bad_value(); break;
  }
}
// c = a + b (b is share)
void OnlineOp::add(Share& c, const Share& a, const Share& b) { c = a + b; }

void OnlineOp::add(
  vector<Share>& c, const vector<Share>& a, const vector<Share> b, unsigned int k) {
  if (a.size() != k || b.size() != k)
    throw invalid_size();

  /// cout << "add here 1" << endl;
  c.resize(k);
  for (int i = 0; i < k; i++) {
    add(c[i], a[i], b[i]);
  }
  /// cout << "add here 2" << endl;
}

void OnlineOp::add_plain(Share& c, const Share& a, const gfp& b) { c.add(a, b); }

void OnlineOp::add_plain(
  vector<Share>& c, const vector<Share>& a, const vector<gfp>& b, unsigned int k) {
  if (a.size() != k || b.size() != k)
    throw invalid_size();
  c.resize(k);
  for (int i = 0; i < k; i++) {
    add_plain(c[i], a[i], b[i]);
  }
}

void OnlineOp::add_inplace(Share& c, const Share& a) {
  Share tmp;
  add(tmp, c, a);
  c = tmp;
}

void OnlineOp::add_inplace(vector<Share>& c, const vector<Share>& a, unsigned int k) {
  if (a.size() != k)
    throw invalid_size();
  c.resize(k);
  Share tmp;
  for (int i = 0; i < k; i++) {
    add(tmp, c[i], a[i]);
    c[i] = tmp;
  }
}

void OnlineOp::add_plain_inplace(Share& c, const gfp& a) { add_plain(c, c, a); }

// c = a - b (b is share)
void OnlineOp::sub(Share& c, const Share& a, const Share& b) { c = a - b; }
// c = a - b (b is plain)
void OnlineOp::sub_plain(Share& c, const Share& a, const gfp& b) { c.sub(a, b); }

void OnlineOp::sub_inplace(Share& c, const Share& a) {
  Share tmp;
  sub(tmp, c, a);
  c = tmp;
}

void OnlineOp::sub_inplace(vector<Share>& c, const vector<Share>& a, unsigned int k) {
  if (a.size() != k)
    throw invalid_size();

  c.resize(k);
  Share tmp;
  for (int i = 0; i < k; i++) {
    sub(tmp, c[i], a[i]);
    c[i] = tmp;
  }
}

void OnlineOp::mul_plain(Share& c, const Share& a, const gfp& b) { c.mul(a, b); }

void OnlineOp::mul_plain(
  vector<Share>& c, const vector<Share>& a, const vector<gfp>& b, unsigned int k) {
  if (a.size() != k || b.size() != k)
    throw invalid_size();
  c.resize(k);
  for (int i = 0; i < k; i++) {
    mul_plain(c[i], a[i], b[i]);
  }
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

void OnlineOp::mul(
  vector<Share>& c, const vector<Share>& a, const vector<Share>& b, unsigned int k) {
  if (a.size() != k || b.size() != k)
    throw invalid_size();
  c.resize(k);

  vector<vector<Share>> sp(k, vector<Share>(3));
  vector<Share> sp0(k), sp1(k), sp2(k);

  for (int i = 0; i < k; i++) {
    getTuples(sp[i], TRIPLE);
    sp0[i] = sp[i][0];
    sp1[i] = sp[i][1];
    sp2[i] = sp[i][2];
  }

  vector<Share> epsilon(k), rho(k);
  vector<gfp> vc(2 * k);

  for (int i = 0; i < k; i++) {
    epsilon[i] = a[i] - sp[i][0];
    rho[i] = b[i] - sp[i][1];
  }

  epsilon.insert(epsilon.end(), rho.begin(), rho.end());
  open(epsilon, vc);
  vector<gfp> vc0, vc1;
  vc0.insert(vc0.begin(), vc.begin(), vc.begin() + k);
  vc1.insert(vc1.begin(), vc.begin() + k, vc.end());

  vector<Share> tmp;
  mul_plain(tmp, sp1, vc0, k);
  add_inplace(sp2, tmp, k);

  mul_plain(tmp, sp0, vc1, k);
  add_inplace(sp2, tmp, k);

  for (int i = 0; i < k; i++) {
    add_plain(c[i], sp2[i], vc0[i] * vc1[i]);
  }
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

// bit ops
void OnlineOp::XOR(Share& c, const Share& a, const Share& b) {
  Share tmp;
  add(c, a, b);
  mul(tmp, a, b);
  sub_inplace(c, tmp);
  sub_inplace(c, tmp);
}

void OnlineOp::XOR(
  vector<Share>& c, const vector<Share>& a, const vector<Share>& b, unsigned int k) {
  /// cout << "c: " << c.size() << endl;
  /// cout << "a: " << a.size() << endl;
  /// cout << "b: " << b.size() << endl;
  /// cout << "k: " << k << endl;
  if (a.size() != k || b.size() != k)
    throw invalid_size();

  /// cout << "xor here" << endl;

  vector<Share> tmp;
  add(c, a, b, k);

  /// cout << "xor add here 1" << endl;
  mul(tmp, a, b, k);
  /// cout << "xor mul here 2" << endl;
  sub_inplace(c, tmp, k);
  sub_inplace(c, tmp, k);
}

void OnlineOp::XOR_inplace(Share& c, const Share& a) {
  Share tmp;
  XOR(tmp, c, a);
  c = tmp;
}

void OnlineOp::XOR_plain(Share& c, const Share& a, const gfp& b) {
  Share tmp;
  add_plain(c, a, b);
  mul_plain(tmp, a, b);
  sub_inplace(c, tmp);
  sub_inplace(c, tmp);
}

void OnlineOp::XOR_inplace(vector<Share>& c, const vector<Share>& a, unsigned int k) {
  vector<Share> tmp;
  c.resize(k);
  XOR(tmp, c, a, k);
  c = tmp;
}

void OnlineOp::XOR_plain(
  vector<Share>& c, const vector<Share>& a, const vector<gfp>& b, unsigned int k) {
  if (a.size() != k || b.size() != k)
    throw invalid_size();
  vector<Share> tmp;
  add_plain(c, a, b, k);
  mul_plain(tmp, a, b, k);
  sub_inplace(c, tmp, k);
  sub_inplace(c, tmp, k);
}

void OnlineOp::OR(Share& c, const Share& a, const Share& b) {
  Share tmp;
  add(c, a, b);
  mul(tmp, a, b);
  sub_inplace(c, tmp);
}
void OnlineOp::OR(
  vector<Share>& c, const vector<Share>& a, const vector<Share>& b, unsigned int k) {
  if (a.size() != k || b.size() != k)
    throw invalid_size();
  vector<Share> tmp;
  add(c, a, b, k);
  mul(tmp, a, b, k);
  sub_inplace(c, tmp, k);
}

void OnlineOp::OR_inplace(Share& c, const Share& a) {
  Share tmp;
  OR(tmp, c, a);
  c = tmp;
}

void OnlineOp::OR_plain(Share& c, const Share& a, const gfp& b) {
  Share tmp;
  add_plain(c, a, b);
  mul_plain(tmp, a, b);
  sub_inplace(c, tmp);
}

void OnlineOp::OR_inplace(vector<Share>& c, const vector<Share>& a, unsigned int k) {
  vector<Share> tmp;
  OR(tmp, c, a, k);
  c = tmp;
}
void OnlineOp::OR_plain(
  vector<Share>& c, const vector<Share>& a, const vector<gfp>& b, unsigned int k) {
  vector<Share> tmp;
  add_plain(c, a, b, k);
  mul_plain(tmp, a, b, k);
  sub_inplace(c, tmp, k);
}

void OnlineOp::AND(Share& c, const Share& a, const Share& b) { mul(c, a, b); }

void OnlineOp::AND_inplace(Share& c, const Share& a) {
  Share tmp;
  AND(tmp, c, a);
  c = tmp;
}

void OnlineOp::AND_plain(Share& c, const Share& a, const gfp& b) { mul_plain(c, a, b); }

void OnlineOp::KXOR(Share& c, const vector<Share>& a, unsigned int k) {
  if (a.size() != k)
    throw invalid_size();

  if (k == 1) {
    c = a[0];
    return;
  }

  vector<Share> half_c, half_a;

  if (k % 2 == 0) {
    for (int i = 0; i < k / 2; i++) {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }

    XOR_inplace(half_c, half_a, k / 2);
    KXOR(c, half_c, k / 2);
  }

  if (k % 2 == 1) {
    for (int i = 0; i < (k - 1) / 2; i++) {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }
    /// cout << "here 4" << endl;
    /// cout << "half_c size: " << half_c.size() << endl;
    /// cout << "half_a size: " << half_a.size() << endl;
    /// cout << "k/2: " << k / 2 << endl;

    XOR_inplace(half_c, half_a, k / 2);

    half_c.push_back(a[k - 1]);
    /// cout << "here 5" << endl;
    KXOR(c, half_c, (k + 1) / 2);
  }
  /// cout << "here 3" << endl;
}

void OnlineOp::KOR(Share& c, const vector<Share>& a, unsigned int k) {
  if (a.size() != k)
    throw invalid_size();

  if (k == 1) {
    c = a[0];
    return;
  }

  vector<Share> half_c, half_a;

  if (k % 2 == 0) {
    for (int i = 0; i < k / 2; i++) {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }

    OR_inplace(half_c, half_a, k / 2);
    KOR(c, half_c, k / 2);
  }

  if (k % 2 == 1) {
    for (int i = 0; i < (k - 1) / 2; i++) {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }
    OR_inplace(half_c, half_a, k / 2);
    half_c.push_back(a[k - 1]);
    KOR(c, half_c, (k + 1) / 2);
  }
}

void OnlineOp::prefix_OR(vector<Share>& c, const vector<Share>& a, unsigned int k) {
  if (a.size() != k)
    throw invalid_size();

  c.resize(k);

  if (k == 1) {
    c[0] = a[0];
    return;
  }

  unsigned half = k / 2;
  vector<Share> half_a0, half_a1;
  vector<Share> half_c0, half_c1;
  half_a0.insert(half_a0.begin(), a.begin(), a.begin() + half);
  half_a1.insert(half_a1.begin(), a.begin() + half, a.end());

  prefix_OR(half_c0, half_a0, half);
  prefix_OR(half_c1, half_a1, k - half);

  vector<Share> tmp(k - half, half_c0[half - 1]);

  OR_inplace(tmp, half_c1, k - half);

  copy(half_c0.begin(), half_c0.end(), c.begin());
  copy(tmp.begin(), tmp.end(), c.begin() + half);
}

void OnlineOp::lt(Share& bit, vector<Share>& a, vector<Share>& b, unsigned int k) {
  vector<Share> c, d;
  XOR(c, a, b, k);
  reverse(c.begin(), c.end());
  prefix_OR(d, c, k);
  reverse(d.begin(), d.end());

  vector<Share> e(k);
  for (int i = 0; i < k - 1; i++) {
    sub(e[i], d[i], d[i + 1]);
  }
  e[k - 1] = d[k - 1];

  vector<Share> tmp;
  mul(tmp, e, b, k);
  bit = tmp[0];
  for (int i = 1; i < k; i++) {
    add_inplace(bit, tmp[i]);
  }
}

void OnlineOp::lt(Share& bit, vector<Share>& a, vector<gfp>& b, unsigned int k) {
  vector<Share> c, d;
  XOR_plain(c, a, b, k);
  reverse(c.begin(), c.end());
  prefix_OR(d, c, k);
  reverse(d.begin(), d.end());

  vector<Share> e(k);
  for (int i = 0; i < k - 1; i++) {
    sub(e[i], d[i], d[i + 1]);
  }
  e[k - 1] = d[k - 1];

  vector<Share> tmp;
  mul_plain(tmp, e, b, k);
  bit = tmp[0];
  for (int i = 1; i < k; i++) {
    add_inplace(bit, tmp[i]);
  }
}

void OnlineOp::pre_rand(Share& r, vector<Share>& bitr) {
  bitr.resize(PSIZE);
  vector<gfp> pbits(PSIZE);
  Share flag;
  vector<gfp> pflag(1);

  decompose(pbits, gfp::pr(), PSIZE);
  while (1) {
    vector<Share> tmp(1);
    for (int i = 0; i < PSIZE; i++) {
      getTuples(tmp, BIT);
      bitr[i] = tmp[0];
    }
    lt(flag, bitr, pbits, PSIZE);
    open({flag}, pflag);

    if (pflag[0] == 1) {
      B2A(r, bitr, PSIZE);
      return;
    }
  }
}

// s = x xor y xor ca_in, ca_out = ca_in xor ((x xor ca_in) and (y xor ca_in))
void OnlineOp::carry_sum(
  Share& ca_out, Share& s, const Share& x, const Share& y, const Share& ca_in) {
  XOR(s, x, y);
  XOR_inplace(s, ca_in);
  Share tmp;
  XOR(tmp, x, ca_in);
  XOR(ca_out, y, ca_in);
  AND_inplace(ca_out, tmp);
  XOR_inplace(ca_out, ca_in);
}

// s= x xor y xor ca_in, ca_out = (x and y) or ((x xor y) and ca_in) (the same as above, but only two mul needed)
void OnlineOp::carry_sum_plain(
  Share& ca_out, Share& s, const Share& x, const gfp& y, const Share& ca_in) {
  //  Share tmp_bit;
  XOR_plain(s, x, y);
  XOR_inplace(s, ca_in);

  Share tmp;
  AND_plain(ca_out, x, y);
  XOR_plain(tmp, x, y);
  AND_inplace(tmp, ca_in);
  OR_inplace(ca_out, tmp);
}

void OnlineOp::add_bit(vector<Share>& c, const vector<Share>& a, const vector<Share>& b) {
  if (a.size() != b.size())
    throw invalid_size();
  c.resize(a.size() + 1);

  Share ca_in, ca_out;
  AND(ca_in, a[0], b[0]);
  XOR(c[0], a[0], b[0]);

  for (int i = 1; i < c.size() - 1; i++) {
    carry_sum(ca_out, c[i], a[i], b[i], ca_in);
    ca_in = ca_out;
  }
  c.back() = ca_out;
}

void OnlineOp::add_bit_plain(vector<Share>& c, const vector<Share>& a, const vector<gfp>& b) {
  if (a.size() != b.size())
    throw invalid_size();
  c.resize(a.size() + 1);

  Share ca_in, ca_out;
  AND_plain(ca_in, a[0], b[0]);
  XOR_plain(c[0], a[0], b[0]);
  for (int i = 1; i < c.size() - 1; i++) {
    carry_sum_plain(ca_out, c[i], a[i], b[i], ca_in);
    ca_in = ca_out;
  }
  c.back() = ca_out;
}

void OnlineOp::decompose(vector<gfp>& bits, const gfp& x, unsigned int k) {
  bits.resize(k);
  gfp ONE;
  gfp tmp = x;
  ONE.assign_one();

  bits[0] = tmp & ONE;
  for (int i = 1; i < k; i++) {
    bits[i] = (tmp >> 1) & ONE;
    tmp = (tmp >> 1);
  }
}

void OnlineOp::decompose(vector<gfp>& bits, const bigint& x, unsigned int k) {
  bits.resize(k);
  bigint ONE = 1;
  bigint tmp(x);

  bits[0].assign(tmp & ONE);
  for (int i = 1; i < k; i++) {
    bits[i].assign((tmp >> 1) & ONE);
    tmp = tmp >> 1;
  }
}

/*
void OnlineOp::add_bit_plain(vector<Share>& c, const vector<Share>& a, const gfp& b, unsigned k) {
  if (a.size() != k)
    throw invalid_size();
  vector<gfp> bbits;
  decompose(bbits, b, k);
  add_bit_plain(c, a, bbits);
}
*/
void OnlineOp::B2A(Share& c, const vector<Share>& bits, unsigned int k) {
  gfp t, pow;
  t.assign(2);
  c = bits[0];
  Share tmp;
  for (int i = 1; i < k; i++) {
    pow = t;
    pow.power(i);
    mul_plain(tmp, bits[i], pow);
    add_inplace(c, tmp);
  }
}

void OnlineOp::B2A(gfp& c, const vector<gfp>& bits, unsigned int k) {
  gfp t, pow;
  t.assign(2);
  c = bits[0];
  gfp tmp;
  for (int i = 1; i < k; i++) {
    pow = t;
    pow.power(i);
    tmp = bits[i] * pow;
    c = c + tmp;
  }
}

void OnlineOp::A2B(vector<Share>& bits, const Share& a) {
  Share r, c;
  vector<Share> bitr;
  pre_rand(r, bitr);

  sub(c, a, r); // c = a-r
  vector<gfp> cp;
  open({c}, cp);

  bigint bcp;
  to_bigint(bcp, cp[0]); // reveal c

  Share factor, tmp;
  gfp ONE;

  ONE.assign(1);

  bigint TWO(2);
  vector<gfp> bound;

  if (bcp == 0) {
    bits = bitr;
    return;
  }
  decompose(bound, gfp::pr() - bcp, PSIZE);

  lt(factor, bitr, bound, PSIZE);

  //share of 1-factor
  mul_plain(tmp, factor, TWO);
  sub_inplace(factor, tmp);
  add_plain_inplace(factor, ONE);

  vector<gfp> f, tf;
  vector<Share> tg(PSIZE);

  decompose(f, bcp, PSIZE);
  decompose(tf, (TWO << PSIZE) + bcp - gfp::pr(), PSIZE);

  for (int i = 0; i < PSIZE; i++) {
    mul_plain(tg[i], factor, tf[i] - f[i]);
    add_plain_inplace(tg[i], f[i]);
  }

  add_bit(bits, tg, bitr);
  bits.pop_back();
}

void OnlineOp::uhf(Share& out, const Share& key, const vector<gfp>& in, unsigned int size) {
  if (in.size() != size) {
    throw invalid_size();
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

void OnlineOp::reveal(const vector<Share>& vs, vector<gfp>& vc) { open(vs, vc); }
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

void OnlineOp::test_bit_ops() {
  cout << "============================== BEG " << __FUNCTION__
       << " ==============================" << endl;
  vector<gfp> a(2), b(2);
  vector<Share> sa(2), sb(2);
  vector<Share> xor_share(4), or_share(4), and_share(4);
  vector<Share> xor_plain(4), or_plain(4), and_plain(4);

  vector<gfp> c(4);
  vector<Share> sc(4);

  c[0].assign_zero();
  c[1].assign_one();
  c[2].assign_one();
  c[3].assign_one();

  for (int i = 0; i < 4; i++) {
    get_inputs(0, sc[i], c[i]);
  }

  a[0].assign_one();
  a[1].assign_one();
  b[0].assign_zero();
  b[1].assign_one();

  cout << a[0] << endl;
  cout << a[1] << endl;
  cout << b[0] << endl;
  cout << b[1] << endl;
  cout << "----" << endl;

  for (int i = 0; i < 2; i++) {
    get_inputs(0, sa[i], a[i]);
    get_inputs(0, sb[i], b[i]);
  }

  reveal_and_print(sa);
  reveal_and_print(sb);

  Share ltbit;
  lt(ltbit, sb, sa, 2);
  cout << "less than test: \n";
  reveal_and_print({ltbit});

  gfp neg, neg2;
  vector<gfp> bits;
  neg.assign(-75);
  decompose(bits, neg, PSIZE);
  for (int i = 0; i < PSIZE; i++) {
    cout << bits[i];
  }
  cout << endl;

  B2A(neg2, bits, PSIZE);
  cout << neg2 << endl;

  //bigint bn = gfp::pr();
  bigint bn(10);
  decompose(bits, bn, 4);
  for (int i = 0; i < 4; i++) {
    cout << bits[i];
  }
  cout << endl;

  vector<Share> res;
  add_bit(res, sa, sb);
  cout << "res size: " << res.size() << endl;

  reveal_and_print(res);
  cout << "---" << endl;

  vector<Share> bbits;
  Share svalue;
  vector<Share> svaluebits;
  vector<gfp> pbits, sbits;
  pre_rand(svalue, svaluebits);

  open(svaluebits, pbits);
  cout << "---" << endl;

  A2B(bbits, svalue);
  open(bbits, sbits);
  cout << "A2B results:\n";
  for (int i = 0; i < sbits.size(); i++) {
    cout << sbits[i];
  }
  cout << endl;
  cout << "original bits:\n";
  for (int i = 0; i < pbits.size(); i++) {
    cout << pbits[i];
  }
  cout << endl;

  for (int i = 0; i < pbits.size(); i++) {
    cout << pbits[i] - sbits[i];
  }
  cout << endl;
  // for (int i = bbits.size() - 1; i >= 0; i++) {
  //   reveal_and_print({bbits[i]});
  // }

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      XOR(xor_share[2 * i + j], sa[i], sb[j]);
      OR(or_share[2 * i + j], sa[i], sb[j]);
      AND(and_share[2 * i + j], sa[i], sb[j]);

      XOR_plain(xor_plain[2 * i + j], sa[i], b[j]);
      OR_plain(or_plain[2 * i + j], sa[i], b[j]);
      AND_plain(and_plain[2 * i + j], sa[i], b[j]);
    }
  }

  cout << "XOR in Share :\n";
  reveal_and_print(xor_share);

  cout << "XOR in Plain :\n";
  reveal_and_print(xor_plain);

  cout << "OR in Share :\n";
  reveal_and_print(or_share);

  cout << "OR in Plain :\n";
  reveal_and_print(or_plain);

  cout << "AND in Share :\n";
  reveal_and_print(and_share);

  cout << "AND in Plain :\n";
  reveal_and_print(and_plain);

  Share kxor_share;
  KXOR(kxor_share, sc, sc.size());

  cout << "KXOR in Plain :\n";
  cout << (c[0] ^ c[1] ^ c[2] ^ c[3]) << endl;

  cout << "KXOR in Share :\n";
  reveal_and_print({kxor_share});

  vector<Share> pOR;
  prefix_OR(pOR, sc, sc.size());

  cout << "Prefix OR:\n";
  reveal_and_print(pOR);

  /*
  Share rand;
  vector<Share> rbits;
  vector<gfp> prand(1);
  vector<gfp> pbits;

  pre_rand(rand, rbits);

  cout << "random number :\n";
  reveal_and_print({rand});

  cout << "random bits :\n";
  reveal_and_print(rbits);

  open({rand}, prand);
  decompose(pbits, prand[0], PSIZE);
  for (int i = 0; i < PSIZE; i++) {
    cout << pbits[i];
  }

  vector<Share> bits;
  vector<Share> res;
  add(res, sa, sb, 2);
  cout << "vector add:\n";
  reveal_and_print(res);

  add_plain(res, sa, b, 2);
  cout << "vector add plain:\n";
  reveal_and_print(res);

  mul(res, sa, sb, 2);
  cout << "vector mul: \n";
  reveal_and_print(res);

  mul_plain(res, sa, b, 2);
  cout << "vector mul plain: \n";
  reveal_and_print(res);
*/
  cout << "============================== END " << __FUNCTION__
       << " ==============================" << endl;
}