#pragma once
#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include "Tools/Helper.h"
#include "Complex.h"

#define TT_FUNC_NOT_IMPLEMENTED()                                                    \
  cout << "THROW IN " << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << endl; \
  throw not_implemented()

#define PRINT_TEST_BEG() cout << "======== " << __FUNCTION__ << " ======== BEG " << time(0) << endl
#define PRINT_TEST_END() cout << "======== " << __FUNCTION__ << " ======== END " << time(0) << endl

static const unsigned int PSIZE = 381;
static const unsigned int QSIZE = 254;

#define PRINT_DEBUG_INFO() cout << "======== DEBUG " << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << " " << time(0) << endl

template <class T>
class OnlineOp
{
protected:
  Player &P;
  typename T::Protocol &protocol;
  typename T::LivePrep &preprocessing;
  SubProcessor<T> &processor;
  typename T::MAC_Check &output;
  typedef typename T::clear clear;

public:
  OnlineOp(Player &_P, typename T::Protocol &_protocol, typename T::LivePrep &_preprocessing,
           SubProcessor<T> &_processor, typename T::MAC_Check &_output)
      : P(_P), protocol(_protocol), preprocessing(_preprocessing), processor(_processor), output(_output)
  {
  }

public:
  void power(clear &a, int exp);
  void dotprod(const vector<T> &a, const vector<T> &b, T &c);

  /* Share ops */
  // c = a + b
  void add(T &c, const T &a, const T &b);
  void add(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k = -1);
  void add_plain(T &c, const T &a, const clear &b);
  void add_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k = -1);
  // c += a
  void add_inplace(T &c, const T &a);
  void add_inplace(vector<T> &c, const vector<T> &a, unsigned int k = -1);
  void add_plain_inplace(T &c, const clear &a);
  void add_plain_inplace(vector<T> &c, const vector<clear> &a, unsigned int k = -1);
  // c = a - b
  void sub(T &c, const T &a, const T &b);
  void sub(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k = -1);
  void sub_plain(T &c, const T &a, const clear &b);
  void sub_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k = -1);
  // c -= a
  void sub_inplace(T &c, const T &a);
  void sub_inplace(vector<T> &c, const vector<T> &a, unsigned int k = -1);
  void sub_plain_inplace(T &c, const clear &a);
  void sub_plain_inplace(vector<T> &c, const vector<clear> &a, unsigned int k = -1);

  // c = a * b
  void mul(T &c, const T &a, const T &b);
  void mul(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k = -1);
  void mul_plain(T &c, const T &a, const clear &b);
  void mul_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k = -1);
  // c *= a
  void mul_inplace(T &c, const T &a);
  void mul_inplace(vector<T> &c, const vector<T> &a, unsigned int k = -1);
  void mul_plain_inplace(T &c, const clear &a);
  void mul_plain_inplace(vector<T> &c, const vector<clear> &a, unsigned int k = -1);

  // aa = a^2
  void sqr(T &aa, const T &a);
  void sqr_inplace(T &a);
  //ia = a^{-1} mod q
  void inv(T &ia, const T &a);
  void inv_inplace(T &a);
  // c = a * b^{-1} mod q
  void div(T &c, const T &a, const T &b);
  void div_inplace(T &c, const T &a);

  /* Complex ops */
  // c = a + b (b is shared complex)
  void add(Complex<T> &c, const Complex<T> &a, const Complex<T> &b);
  // c = a + b (b is plain complex)
  void add_plain(Complex<T> &c, const Complex<T> &a, const Complex_Plain<T> &b);
  void add_inplace(Complex<T> &c, const Complex<T> &a);

  // c = a - b (b is shared complex)
  void sub(Complex<T> &c, const Complex<T> &a, const Complex<T> &b);
  // c = a - b (b is plain complex)
  void sub_plain(Complex<T> &c, const Complex<T> &a, const Complex_Plain<T> &b);
  void sub_inplace(Complex<T> &c, const Complex<T> &a);

  // c = a * b (b is plain complex)
  void mul_plain(Complex<T> &c, const Complex<T> &a, const Complex_Plain<T> &b);
  // c = a * b (b is shared complex)
  void mul(Complex<T> &c, const Complex<T> &a, const Complex<T> &b);
  void mul_inplace(Complex<T> &c, const Complex<T> &a);

  // aa = a^2
  void sqr(Complex<T> &aa, const Complex<T> &a);
  void sqr_inplace(Complex<T> &a);

  //ia = a^{-1} mod (q,x^2+1)
  void inv(Complex<T> &ia, const Complex<T> a);
  void inv_inplace(Complex<T> &a);
  // c = a * b^{-1} mod (q, x^2+1)
  void div(Complex<T> &c, const Complex<T> &a, const Complex<T> &b);
  void div_inplace(Complex<T> &c, const Complex<T> &a);

  /* bit ops */
  void XOR(T &c, const T &a, const T &b);
  void XOR(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k);
  void XOR_inplace(T &c, const T &a);
  void XOR_inplace(vector<T> &c, const vector<T> &a, unsigned int k);
  void XOR_plain(T &c, const T &a, const clear &b);
  void XOR_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k);

  void OR(T &c, const T &a, const T &b);
  void OR(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k);
  void OR_inplace(T &c, const T &a);
  void OR_inplace(vector<T> &c, const vector<T> &a, unsigned int k);
  void OR_plain(T &c, const T &a, const clear &b);
  void OR_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k);

  void AND(T &c, const T &a, const T &b);
  void AND_inplace(T &c, const T &a);
  void AND_plain(T &c, const T &a, const clear &b);

  void KXOR(T &c, const vector<T> &a, unsigned int k);
  void KOR(T &c, const vector<T> &a, unsigned int k);
  void prefix_XOR(vector<T> &c, const vector<T> &a, unsigned int k); /*not implemented*/
  void prefix_OR(vector<T> &c, const vector<T> &a, unsigned int k);

  /*return (a<b)*/
  void lt(T &bit, vector<T> &a, vector<T> &b, unsigned int k);
  void lt(T &bit, vector<T> &a, vector<clear> &b, unsigned int k);

  //output random shares of r and bits of r, with uniformly random r.
  void pre_rand(T &r, vector<T> &bitr);

  void carry_sum(T &ca_out, T &s, const T &x, const T &y, const T &ca_in);
  void carry_sum_plain(T &ca_out, T &s, const T &x, const clear &y, const T &ca_in);

  //Bit addition
  void add_bit(vector<T> &c, const vector<T> &a, const vector<T> &b);
  void add_bit_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b);

  void decompose(vector<clear> &bits, const clear &x, unsigned int k);
  void decompose(vector<clear> &bits, const bigint &x, unsigned int k);

  //Binary to Arithmetic
  void B2A(T &c, const vector<T> &bits, unsigned int k);
  void B2A(clear &c, const vector<clear> &bits, unsigned int k);

  //Arithmetic to Binary
  void A2B(vector<T> &bits, const T &c, unsigned int k);
  void A2B(vector<T> &bits, const T &a);

  // out = in[0] + in[1]*key + in[2]*key^2 +...+ in[size-1]*key^{size-1}
  void uhf(T &out, const T &key, const vector<clear> &in, unsigned int size);
  void legendre(int &out, const T &in);
  int legendre_prf(const T &key, const T &in);

  /* reveal */
  void reveal(const T &a, clear &r);
  void reveal(const vector<T> &a, vector<clear> &r);
  void reveal_and_print(const vector<T> &vs, vector<clear> &vc);
  void reveal_and_print(const vector<T> &vs);
  void reveal_and_print(const Complex<T> &s);
  void reveal_and_print(const vector<Complex<T>> &vs);

  /*inputs*/
  void get_inputs(unsigned int party, T &sa, const clear &inputs);
  void get_inputs(unsigned int party, Complex<T> &sa, const Complex_Plain<T> &inputs);

  // the following apis for testing
  void test_add();
  void test_sub();
  void test_mul();
  void test_div();
  void test_sqr();
  void test_inv();

  void test_uhf();
  void test_legendre();
  void test_get_inputs();
  void test_bit_ops();
};
