/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _OnlineOP
#define _OnlineOP

#include "Offline/offline_data.h"
#include "Online/Machine.h"
#include "System/Player.h"
#include "Processor/Processor.h"
#include "Processor/Processor_IO.h"
#include "LSSS/PRSS.h"

extern vector<sacrificed_data> SacrificeD;

enum { TRIPLE = 0x50, BIT = 0x51, SQUARE = 0x52, INPUT_MASK = 0x53 };

class UsedTuples {
 public:
  unsigned int UsedTriples = 0;
  unsigned int UsedSquares = 0;
  unsigned int UsedBit = 0;
  unsigned int UsedInputMask = 0;
};

class Complex {
 public:
  Share real;
  Share imag;
  Complex() {}
  Complex(Share& _real, Share& _imag) : real(_real), imag(_imag) {}
  void setValue(Share& _real, Share& _imag) {
    real = _real;
    imag = _imag;
  }
};

class Complex_Plain {
 public:
  gfp real;
  gfp imag;
  Complex_Plain() {}
  Complex_Plain(gfp& _real, gfp& _imag) : real(_real), imag(_imag) {}
  void setValue(gfp& _real, gfp& _imag) {
    real = _real;
    imag = _imag;
  }
};

class OnlineOp {
 public:
  UsedTuples UT;
  Processor& Proc;
  int online_num;
  Player& P;
  offline_control_data& OCD;
  Machine& machine;
  PRSS prss;
  explicit OnlineOp(
    Processor& Proc_, int online_num_, Player& P_, offline_control_data& OCD_, Machine& machine_)
      : Proc(Proc_), online_num(online_num_), P(P_), OCD(OCD_), machine(machine_), prss(P) {}

  void getTuples(vector<Share>& sp, int opcode);

  /*Share ops*/
  // c = a + b (b is share)
  void add(Share& c, const Share& a, const Share& b);
  // c = a + b (b is plain)
  void add_plain(Share& c, const Share& a, const gfp& b);
  // c = c + a;
  void add_inplace(Share& c, const Share& a);
  void add_plain_inplace(Share& c, const gfp& a);

  // c = a - b (b is share)
  void sub(Share& c, const Share& a, const Share& b);
  // c = a - b (b is plain)
  void sub_plain(Share& c, const Share& a, const gfp& b);
  // c = c - a
  void sub_inplace(Share& c, const Share& a);

  // c = a * b (b is plain)
  void mul_plain(Share& c, const Share& a, const gfp& b);
  // c = a * b (b is share)
  void mul(Share& c, const Share& a, const Share& b);
  void mul_inplace(Share& c, const Share& a);

  // aa = a^2
  void sqr(Share& aa, const Share& a);
  void sqr_inplace(Share& a);
  //ia = a^{-1} mod q
  void inv(Share& ia, const Share& a);
  void inv_inplace(Share& a);
  // c = a * b^{-1} mod q
  void div(Share& c, const Share& a, const Share& b);
  void div_inplace(Share& c, const Share& a);

  /*Complex ops*/
  // c = a + b (b is shared complex)
  void add(Complex& c, const Complex& a, const Complex& b);
  // c = a + b (b is plain complex)
  void add_plain(Complex& c, const Complex& a, const Complex_Plain& b);
  void add_inplace(Complex& c, const Complex& a);

  // c = a - b (b is shared complex)
  void sub(Complex& c, const Complex& a, const Complex& b);
  // c = a - b (b is plain complex)
  void sub_plain(Complex& c, const Complex& a, const Complex_Plain& b);
  void sub_inplace(Complex& c, const Complex& a);

  // c = a * b (b is plain complex)
  void mul_plain(Complex& c, const Complex& a, const Complex_Plain& b);
  // c = a * b (b is shared complex)
  void mul(Complex& c, const Complex& a, const Complex& b);
  void mul_inplace(Complex& c, const Complex& a);

  // aa = a^2
  void sqr(Complex& aa, const Complex& a);
  void sqr_inplace(Complex& a);

  //ia = a^{-1} mod (q,x^2+1)
  void inv(Complex& ia, const Complex a);
  void inv_inplace(Complex& a);
  // c = a * b^{-1} mod (q, x^2+1)
  void div(Complex& c, const Complex& a, const Complex& b);
  void div_inplace(Complex& c, const Complex& a);

  //bit ops

  void XOR(Share& c, const Share& a, const Share& b);
  void XOR_plain(Share& c, const Share& a, const gfp& b);

  void OR(Share& c, const Share& a, const Share& b);
  void OR_plain(Share& c, const Share& a, const gfp& b);

  void AND(Share& c, const Share& a, const Share& b);
  void AND_plain(Share& c, const Share& a, const gfp& b);

  void KXOR(Share& c, const vector<Share>& a, unsigned int k);

  //output random shares of r and bits of r, with uniformly random r.
  void pre_rand(Share& r, vector<Share>& bitr, unsigned int k);

  void carry_sum(Share& ca_out, Share& s, const Share& x, const Share& y, const Share& ca_in);
  void carry_sum_plain(Share& ca_out, Share& s, const Share& x, const gfp& y, const Share& ca_in);

  //Bit addition
  void add_bit(vector<Share>& c, const vector<Share>& a, const vector<Share>& b);
  void add_bit_plain(vector<Share>& c, const vector<Share>& a, const vector<gfp>& b);
  void add_bit_plain(vector<Share>& c, const vector<Share>& a, const gfp& b, unsigned int k);

  //Binary to Arithmetic
  void B2A(Share& c, const vector<Share>& bits, unsigned int k);

  //Arithmetic to Binary
  void A2B(vector<Share>& bits, const Share& c, unsigned int k);

  // out = in[0] + in[1]*key + in[2]*key^2 +...+ in[size-1]*key^{size-1}
  void uhf(Share& out, const Share& key, const vector<gfp>& in, unsigned int size);
  void legendre(int& out, const Share& in);
  int legendre_prf(const Share& key, const Share& in);

  /*open and reveal*/
  // vs --> vc
  void open(const vector<Share>& vs, vector<gfp>& vc);

  // secrets, clears
  void reveal(const vector<Share>& vs, vector<gfp>& vc);
  void reveal_and_print(const vector<Share>& vs, vector<gfp>& vc);
  void reveal_and_print(const vector<Share>& vs);
  void reveal_and_print(const vector<Complex>& vs);

  /*inputs*/
  void get_inputs(unsigned int party, Share& sa, gfp& inputs);
  void get_inputs(unsigned int party, Complex& sa, Complex_Plain& inputs);

  // the following apis for testing

  void test_uhf();
  void test_legendre();
  void test_get_inputs();
  void test_mul();

  void test_bit_ops();
};

#endif
