#include "OnlineOp.h"

template <class T>
void OnlineOp<T>::str_to_gfp(clear &ans, const string &str)
{
  // bigint bn(str, 10);
  mpz_class bnx(str, 10);
  bigint bn(bnx);
  to_gfp(ans, bn);
}

template <class T>
void OnlineOp<T>::power(clear &a, int exp)
{
  if (exp == 0)
    a = 0;

  clear tmp = a;
  for (int i = 1; i < exp; i++)
    a *= tmp;
}

template <class T>
void OnlineOp<T>::dotprod(const vector<T> &a, const vector<T> &b, T &c)
{
  assert(a.size() == b.size());
  size_t n = a.size();

  protocol.init_dotprod(&processor);
  for (size_t i = 0; i < n; i++)
    protocol.prepare_dotprod(a[i], b[i]);

  protocol.next_dotprod();
  protocol.exchange();
  c = protocol.finalize_dotprod(n);
}

// add
template <class T>
void OnlineOp<T>::add(T &c, const T &a, const T &b)
{
  c = a + b;
}
template <class T>
void OnlineOp<T>::add(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k /* = -1*/)
{
  assert(a.size() == b.size());
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    add(c[i], a[i], b[i]);
}
template <class T>
void OnlineOp<T>::add_plain(T &c, const T &a, const clear &b)
{
  //yyltodo? no Share::add(Share, clear)
  T sb;
  get_inputs(0, sb, b);

  c = a + sb;
}
template <class T>
void OnlineOp<T>::add_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k /* = -1*/)
{
  assert(a.size() == b.size());
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    add_plain(c[i], a[i], b[i]);
}
template <class T>
void OnlineOp<T>::add_inplace(T &c, const T &a)
{
  c += a;
}
template <class T>
void OnlineOp<T>::add_inplace(vector<T> &c, const vector<T> &a, unsigned int k /* = -1*/)
{
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    add_inplace(c[i], a[i]);
}
template <class T>
void OnlineOp<T>::add_plain_inplace(T &c, const clear &a)
{
  //yyltodo? no Share::add(Share, clear)
  T sa;
  get_inputs(0, sa, a);

  c += sa;
}
template <class T>
void OnlineOp<T>::add_plain_inplace(vector<T> &c, const vector<clear> &a, unsigned int k /* = -1*/)
{
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    add_plain_inplace(c[i], a[i]);
}

// sub
template <class T>
void OnlineOp<T>::sub(T &c, const T &a, const T &b)
{
  c = a - b;
}
template <class T>
void OnlineOp<T>::sub(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k /* = -1*/)
{
  assert(a.size() == b.size());
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    sub(c[i], a[i], b[i]);
}
template <class T>
void OnlineOp<T>::sub_plain(T &c, const T &a, const clear &b)
{
  //yyltodo? no Share::sub(Share, clear)
  T sb;
  get_inputs(0, sb, b);

  c = a - sb;
}
template <class T>
void OnlineOp<T>::sub_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k /* = -1*/)
{
  assert(a.size() == b.size());
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    sub_plain(c[i], a[i], b[i]);
}
template <class T>
void OnlineOp<T>::sub_inplace(T &c, const T &a)
{
  c -= a;
}
template <class T>
void OnlineOp<T>::sub_inplace(vector<T> &c, const vector<T> &a, unsigned int k /* = -1*/)
{
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    sub_inplace(c[i], a[i]);
}
template <class T>
void OnlineOp<T>::sub_plain_inplace(T &c, const clear &a)
{
  //yyltodo? no Share::sub(Share, clear)
  T sa;
  get_inputs(0, sa, a);

  c -= sa;
}
template <class T>
void OnlineOp<T>::sub_plain_inplace(vector<T> &c, const vector<clear> &a, unsigned int k /* = -1*/)
{
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    sub_plain_inplace(c[i], a[i]);
}

// mul
template <class T>
void OnlineOp<T>::mul(T &c, const T &a, const T &b)
{
  vector<T> vc;
  mul(vc, {a}, {b});
  c = vc[0];
}
template <class T>
void OnlineOp<T>::mul(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k /* = -1*/)
{
  assert(a.size() == b.size());
  size_t n = a.size();
  c.resize(n);

  protocol.init_mul(&processor);
  for (size_t i = 0; i < n; i++)
    protocol.prepare_mul(a[i], b[i]);

  protocol.exchange();
  for (int i = 0; i < n; i++)
    c[i] = protocol.finalize_mul();
}
template <class T>
void OnlineOp<T>::mul_plain(T &c, const T &a, const clear &b)
{
  c = a * b;
}
template <class T>
void OnlineOp<T>::mul_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k /* = -1*/)
{
  assert(a.size() == b.size());
  size_t n = a.size();
  c.resize(n);
  for (size_t i = 0; i < n; i++)
    mul_plain(c[i], a[i], b[i]);
}
template <class T>
void OnlineOp<T>::mul_inplace(T &c, const T &a)
{
  vector<T> vc;
  mul(vc, {a}, {c});
  c = vc[0];
}
template <class T>
void OnlineOp<T>::mul_inplace(vector<T> &c, const vector<T> &a, unsigned int k /* = -1*/)
{
  size_t n = a.size();
  c.resize(n);

  for (size_t i = 0; i < n; i++)
    mul_inplace(c[i], a[i]);
}
template <class T>
void OnlineOp<T>::mul_plain_inplace(T &c, const clear &a)
{
  c *= a;
}
template <class T>
void OnlineOp<T>::mul_plain_inplace(vector<T> &c, const vector<clear> &a, unsigned int k /* = -1*/)
{
  size_t n = a.size();
  c.resize(n);
  for (size_t i = 0; i < n; i++)
    mul_plain_inplace(c[i], a[i]);
}

template <class T>
void OnlineOp<T>::sqr(T &aa, const T &a)
{
  //phase 0: get triples
  vector<T> sp(2);
  preprocessing.get_two(DATA_SQUARE, sp[0], sp[1]); // [OR] preprocessing.get(DATA_SQUARE, &sp.data());
  // reveal_and_print(sp);

  //phase 1: sub and open
  T epsilon = a - sp[0]; // epsilon = x - a
  vector<clear> vc;
  reveal({epsilon}, vc);

  //phase 2 get squre share
  T tmp;
  mul_plain(tmp, sp[0], vc[0]);
  add_inplace(sp[1], tmp);
  add_inplace(sp[1], tmp);

  clear cp0 = vc[0] * vc[0];
  add_plain(aa, sp[1], cp0);
}

template <class T>
void OnlineOp<T>::sqr_inplace(T &a)
{
  T tmp;
  sqr(tmp, a);
  a = tmp;
}
template <class T>
void OnlineOp<T>::inv(T &ia, const T &a)
{
  // get random share r
  T rshare;
  {
    vector<T> tmp(2);
    preprocessing.get_two(DATA_SQUARE, tmp[0], tmp[1]);
    rshare = tmp[0];
  }
  // reveal_and_print({rshare});

  //get share of r*a;
  T c;
  mul(c, rshare, a);

  //open ra;
  vector<clear> ra;
  reveal({c}, ra);
  // reveal_and_print({c});
  if (ra[0].is_zero())
    throw division_by_zero();

  //invert ra;
  clear ira = ra[0];
  ira = ira.invert();

  mul_plain(ia, rshare, ira);
}

template <class T>
void OnlineOp<T>::inv_inplace(T &a)
{
  T tmp;
  inv(tmp, a);
  a = tmp;
}
template <class T>
void OnlineOp<T>::div(T &c, const T &a, const T &b)
{
  inv(c, b);
  mul_inplace(c, a);
}
template <class T>
void OnlineOp<T>::div_inplace(T &c, const T &a)
{
  T tmp;
  div(tmp, c, a);
  c = tmp;
}

/* Complex ops */
template <class T>
void OnlineOp<T>::add(Complex<T> &c, const Complex<T> &a, const Complex<T> &b)
{
  add(c.real, a.real, b.real);
  add(c.imag, a.imag, b.imag);
}
template <class T>
void OnlineOp<T>::add_plain(Complex<T> &c, const Complex<T> &a, const Complex_Plain<T> &b)
{
  add_plain(c.real, a.real, b.real);
  add_plain(c.imag, a.imag, b.imag);
}
template <class T>
void OnlineOp<T>::add_inplace(Complex<T> &c, const Complex<T> &a)
{
  Complex<T> tmp;
  add(tmp, c, a);
  c = tmp;
}

template <class T>
void OnlineOp<T>::sub(Complex<T> &c, const Complex<T> &a, const Complex<T> &b)
{
  sub(c.imag, a.imag, b.imag);
  sub(c.real, a.real, b.real);
}

template <class T>
void OnlineOp<T>::sub_plain(Complex<T> &c, const Complex<T> &a, const Complex_Plain<T> &b)
{
  sub_plain(c.imag, a.imag, b.imag);
  sub_plain(c.real, a.real, b.real);
}
template <class T>
void OnlineOp<T>::sub_inplace(Complex<T> &c, const Complex<T> &a)
{
  Complex<T> tmp;
  sub(tmp, c, a);
  c = tmp;
}
template <class T>
void OnlineOp<T>::mul_plain(Complex<T> &c, const Complex<T> &a, const Complex_Plain<T> &b)
{
  T tmpimag, tmpreal, creal;
  mul_plain(tmpreal, a.real, b.real);
  mul_plain(tmpimag, a.imag, b.imag);
  sub(creal, tmpreal, tmpimag);

  mul_plain(tmpreal, a.real, b.imag);
  mul_plain(tmpimag, a.imag, b.real);
  add(c.imag, tmpreal, tmpimag);
  c.real = creal;
}
template <class T>
void OnlineOp<T>::mul(Complex<T> &c, const Complex<T> &a, const Complex<T> &b)
{
  T tmp;
  mul(c.real, a.real, b.real);
  mul(tmp, a.imag, b.imag);
  sub_inplace(c.real, tmp);

  mul(c.imag, a.real, b.imag);
  mul(tmp, a.imag, b.real);
  add_inplace(c.imag, tmp);
}
template <class T>
void OnlineOp<T>::mul_inplace(Complex<T> &c, const Complex<T> &a)
{
  Complex<T> tmp;
  mul(tmp, c, a);
  c = tmp;
}
template <class T>
void OnlineOp<T>::sqr(Complex<T> &aa, const Complex<T> &a)
{
  sqr(aa.real, a.real);
  sqr(aa.imag, a.imag);
  sub_inplace(aa.real, aa.imag);

  mul(aa.imag, a.real, a.imag);
  add_inplace(aa.imag, aa.imag);
}
template <class T>
void OnlineOp<T>::sqr_inplace(Complex<T> &a)
{
  Complex<T> tmp;
  sqr(tmp, a);
  a = tmp;
}
template <class T>
void OnlineOp<T>::inv(Complex<T> &ia, const Complex<T> a)
{
  T rr;
  sqr(ia.real, a.real);
  sqr(ia.imag, a.imag);

  add_inplace(ia.real, ia.imag);
  rr = ia.real;

  inv_inplace(rr); // rr = (real^2+imag^2)^{-1}

  mul(ia.real, a.real, rr);
  //rr.negate();
  clear neg(-1);
  rr *= neg;

  mul(ia.imag, a.imag, rr);
}
template <class T>
void OnlineOp<T>::inv_inplace(Complex<T> &a)
{
  Complex<T> tmp;
  inv(tmp, a);
  a = tmp;
}
template <class T>
void OnlineOp<T>::div(Complex<T> &c, const Complex<T> &a, const Complex<T> &b)
{
  inv(c, b);
  mul_inplace(c, a);
}
template <class T>
void OnlineOp<T>::div_inplace(Complex<T> &c, const Complex<T> &a)
{
  Complex<T> tmp;
  div(tmp, c, a);
  c = tmp;
}

/* bit ops */
template <class T>
void OnlineOp<T>::XOR(T &c, const T &a, const T &b)
{
  T tmp;
  add(c, a, b);
  mul(tmp, a, b);
  sub_inplace(c, tmp);
  sub_inplace(c, tmp);
}
template <class T>
void OnlineOp<T>::XOR(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k)
{
  /// cout << "c: " << c.size() << endl;
  /// cout << "a: " << a.size() << endl;
  /// cout << "b: " << b.size() << endl;
  /// cout << "k: " << k << endl;
  if (a.size() != k || b.size() != k)
    throw invalid_length();

  /// cout << "xor here" << endl;

  vector<T> tmp;
  add(c, a, b, k);

  /// cout << "xor add here 1" << endl;
  mul(tmp, a, b, k);
  /// cout << "xor mul here 2" << endl;
  sub_inplace(c, tmp, k);
  sub_inplace(c, tmp, k);
}
template <class T>
void OnlineOp<T>::XOR_inplace(T &c, const T &a)
{
  T tmp;
  XOR(tmp, c, a);
  c = tmp;
}
template <class T>
void OnlineOp<T>::XOR_inplace(vector<T> &c, const vector<T> &a, unsigned int k)
{
  vector<T> tmp;
  c.resize(k);
  XOR(tmp, c, a, k);
  c = tmp;
}
template <class T>
void OnlineOp<T>::XOR_plain(T &c, const T &a, const clear &b)
{
  T tmp;
  add_plain(c, a, b);
  mul_plain(tmp, a, b);
  sub_inplace(c, tmp);
  sub_inplace(c, tmp);
}
template <class T>
void OnlineOp<T>::XOR_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k)
{
  if (a.size() != k || b.size() != k)
    throw invalid_length();
  vector<T> tmp;
  add_plain(c, a, b, k);
  mul_plain(tmp, a, b, k);
  sub_inplace(c, tmp, k);
  sub_inplace(c, tmp, k);
}
template <class T>
void OnlineOp<T>::OR(T &c, const T &a, const T &b)
{
  T tmp;
  add(c, a, b);
  mul(tmp, a, b);
  sub_inplace(c, tmp);
}
template <class T>
void OnlineOp<T>::OR(vector<T> &c, const vector<T> &a, const vector<T> &b, unsigned int k)
{
  if (a.size() != k || b.size() != k)
    throw invalid_length();
  vector<T> tmp;
  add(c, a, b, k);
  mul(tmp, a, b, k);
  sub_inplace(c, tmp, k);
}
template <class T>
void OnlineOp<T>::OR_inplace(T &c, const T &a)
{
  T tmp;
  OR(tmp, c, a);
  c = tmp;
}
template <class T>
void OnlineOp<T>::OR_inplace(vector<T> &c, const vector<T> &a, unsigned int k)
{
  vector<T> tmp;
  OR(tmp, c, a, k);
  c = tmp;
}
template <class T>
void OnlineOp<T>::OR_plain(T &c, const T &a, const clear &b)
{
  T tmp;
  add_plain(c, a, b);
  mul_plain(tmp, a, b);
  sub_inplace(c, tmp);
}
template <class T>
void OnlineOp<T>::OR_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b, unsigned int k)
{
  vector<T> tmp;
  add_plain(c, a, b, k);
  mul_plain(tmp, a, b, k);
  sub_inplace(c, tmp, k);
}

template <class T>
void OnlineOp<T>::AND(T &c, const T &a, const T &b)
{
  mul(c, a, b);
}
template <class T>
void OnlineOp<T>::AND_inplace(T &c, const T &a)
{
  T tmp;
  AND(tmp, c, a);
  c = tmp;
}
template <class T>
void OnlineOp<T>::AND_plain(T &c, const T &a, const clear &b)
{
  mul_plain(c, a, b);
}
template <class T>
void OnlineOp<T>::KXOR(T &c, const vector<T> &a, unsigned int k)
{
  if (a.size() != k)
    throw invalid_length();

  if (k == 1)
  {
    c = a[0];
    return;
  }

  vector<T> half_c, half_a;

  if (k % 2 == 0)
  {
    for (int i = 0; i < k / 2; i++)
    {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }

    XOR_inplace(half_c, half_a, k / 2);
    KXOR(c, half_c, k / 2);
  }

  if (k % 2 == 1)
  {
    for (int i = 0; i < (k - 1) / 2; i++)
    {
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
template <class T>
void OnlineOp<T>::KOR(T &c, const vector<T> &a, unsigned int k)
{
  if (a.size() != k)
    throw invalid_length();

  if (k == 1)
  {
    c = a[0];
    return;
  }

  vector<T> half_c, half_a;

  if (k % 2 == 0)
  {
    for (int i = 0; i < k / 2; i++)
    {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }

    OR_inplace(half_c, half_a, k / 2);
    KOR(c, half_c, k / 2);
  }

  if (k % 2 == 1)
  {
    for (int i = 0; i < (k - 1) / 2; i++)
    {
      half_c.push_back(a[2 * i]);
      half_a.push_back(a[2 * i + 1]);
    }
    OR_inplace(half_c, half_a, k / 2);
    half_c.push_back(a[k - 1]);
    KOR(c, half_c, (k + 1) / 2);
  }
}
template <class T>
void OnlineOp<T>::prefix_XOR(vector<T> &c, const vector<T> &a, unsigned int k) { TT_FUNC_NOT_IMPLEMENTED(); }
template <class T>
void OnlineOp<T>::prefix_OR(vector<T> &c, const vector<T> &a, unsigned int k)
{
  if (a.size() != k)
    throw invalid_length();

  c.resize(k);

  if (k == 1)
  {
    c[0] = a[0];
    return;
  }

  unsigned half = k / 2;
  vector<T> half_a0, half_a1;
  vector<T> half_c0, half_c1;
  half_a0.insert(half_a0.begin(), a.begin(), a.begin() + half);
  half_a1.insert(half_a1.begin(), a.begin() + half, a.end());

  prefix_OR(half_c0, half_a0, half);
  prefix_OR(half_c1, half_a1, k - half);

  vector<T> tmp(k - half, half_c0[half - 1]);

  OR_inplace(tmp, half_c1, k - half);

  copy(half_c0.begin(), half_c0.end(), c.begin());
  copy(tmp.begin(), tmp.end(), c.begin() + half);
}

/*return (a<b)*/
template <class T>
void OnlineOp<T>::lt(T &bit, vector<T> &a, vector<T> &b, unsigned int k)
{
  vector<T> c, d;
  XOR(c, a, b, k);
  reverse(c.begin(), c.end());
  prefix_OR(d, c, k);
  reverse(d.begin(), d.end());

  vector<T> e(k);
  for (int i = 0; i < k - 1; i++)
  {
    sub(e[i], d[i], d[i + 1]);
  }
  e[k - 1] = d[k - 1];

  vector<T> tmp;
  mul(tmp, e, b, k);
  bit = tmp[0];
  for (int i = 1; i < k; i++)
  {
    add_inplace(bit, tmp[i]);
  }
}
template <class T>
void OnlineOp<T>::lt(T &bit, vector<T> &a, vector<clear> &b, unsigned int k)
{
  vector<T> c, d;
  XOR_plain(c, a, b, k);
  reverse(c.begin(), c.end());
  prefix_OR(d, c, k);
  reverse(d.begin(), d.end());

  vector<T> e(k);
  for (int i = 0; i < k - 1; i++)
  {
    sub(e[i], d[i], d[i + 1]);
  }
  e[k - 1] = d[k - 1];

  vector<T> tmp;
  mul_plain(tmp, e, b, k);
  bit = tmp[0];
  for (int i = 1; i < k; i++)
  {
    add_inplace(bit, tmp[i]);
  }
}

//output random shares of r and bits of r, with uniformly random r.
template <class T>
void OnlineOp<T>::pre_rand(T &r, vector<T> &bitr)
{
  PRINT_DEBUG_INFO();
  bitr.resize(PSIZE);
  vector<clear> pbits(PSIZE);
  T flag;
  vector<clear> pflag(1);
  // vector<clear> ptest(PSIZE);

  PRINT_DEBUG_INFO();
  decompose(pbits, clear::pr(), PSIZE);
  PRINT_DEBUG_INFO();
  while (1)
  {
    vector<T> tmp(1);
    for (int i = 0; i < PSIZE; i++)
    {
      preprocessing.get_one(DATA_BIT, tmp[0]);
      // getTuples(tmp, BIT);
      bitr[i] = tmp[0];
    }

    // cout << "pflag pbits[0]:" << pbits[0] << endl;
    // reveal(bitr, ptest);
    // cout << "pflag bitr[0]:" << ptest[0] << endl;
    // reveal_and_print(bitr);
    // for (int ii = 0; ii < 10; ii++)
    // {
    //   cout << " " << ptest[ii];
    // }
    // cout << endl;

    lt(flag, bitr, pbits, PSIZE);
    reveal({flag}, pflag);
    cout << "pflag[0]:" << pflag[0] << endl;
    // sleep(1);

    if (pflag[0] == 1)
    {
      B2A(r, bitr, PSIZE);
      return;
    }
  }
  PRINT_DEBUG_INFO();
}
template <class T>
void OnlineOp<T>::carry_sum(T &ca_out, T &s, const T &x, const T &y, const T &ca_in)
{
  XOR(s, x, y);
  XOR_inplace(s, ca_in);
  T tmp;
  XOR(tmp, x, ca_in);
  XOR(ca_out, y, ca_in);
  AND_inplace(ca_out, tmp);
  XOR_inplace(ca_out, ca_in);
}
template <class T>
void OnlineOp<T>::carry_sum_plain(T &ca_out, T &s, const T &x, const clear &y, const T &ca_in)
{ //  T tmp_bit;
  XOR_plain(s, x, y);
  XOR_inplace(s, ca_in);

  T tmp;
  AND_plain(ca_out, x, y);
  XOR_plain(tmp, x, y);
  AND_inplace(tmp, ca_in);
  OR_inplace(ca_out, tmp);
}

//Bit addition
template <class T>
void OnlineOp<T>::add_bit(vector<T> &c, const vector<T> &a, const vector<T> &b)
{
  if (a.size() != b.size())
    throw invalid_length();
  c.resize(a.size() + 1);

  T ca_in, ca_out;
  AND(ca_in, a[0], b[0]);
  XOR(c[0], a[0], b[0]);

  for (int i = 1; i < c.size() - 1; i++)
  {
    carry_sum(ca_out, c[i], a[i], b[i], ca_in);
    ca_in = ca_out;
  }
  c.back() = ca_out;
}
template <class T>
void OnlineOp<T>::add_bit_plain(vector<T> &c, const vector<T> &a, const vector<clear> &b)
{
  if (a.size() != b.size())
    throw invalid_length();
  c.resize(a.size() + 1);

  T ca_in, ca_out;
  AND_plain(ca_in, a[0], b[0]);
  XOR_plain(c[0], a[0], b[0]);
  for (int i = 1; i < c.size() - 1; i++)
  {
    carry_sum_plain(ca_out, c[i], a[i], b[i], ca_in);
    ca_in = ca_out;
  }
  c.back() = ca_out;
}

template <class T>
void OnlineOp<T>::decompose(vector<clear> &bits, const clear &x, unsigned int k)
{
  bits.resize(k);
  clear ONE;
  clear tmp = x;
  ONE.assign_one();

  bits[0] = tmp & ONE;
  for (int i = 1; i < k; i++)
  {
    bits[i] = (tmp >> 1) & ONE;
    tmp = (tmp >> 1);
  }
}
template <class T>
void OnlineOp<T>::decompose(vector<clear> &bits, const bigint &x, unsigned int k)
{
  bits.resize(k);
  bigint ONE = 1;
  bigint tmp(x);

  // bits[0].assign(tmp & ONE);
  bits[0] = clear(tmp & ONE); // [or] clear xx(tmp & ONE); bits[0] = xx;
  for (int i = 1; i < k; i++)
  {
    // bits[i].assign((tmp >> 1) & ONE);
    bits[i] = clear((tmp >> 1) & ONE);
    tmp = tmp >> 1;
  }
}

//Binary to Arithmetic
template <class T>
void OnlineOp<T>::B2A(T &c, const vector<T> &bits, unsigned int k)
{
  clear t, pow;
  // t.assign(2);
  t = 2;
  c = bits[0];
  T tmp;
  for (int i = 1; i < k; i++)
  {
    pow = t;
    //pow.power(i); // yyltodo? T::clear no .power()
    power(pow, i);
    mul_plain(tmp, bits[i], pow);
    add_inplace(c, tmp);
  }
}
template <class T>
void OnlineOp<T>::B2A(clear &c, const vector<clear> &bits, unsigned int k)
{
  clear t, pow;
  // t.assign(2);
  t = 2;
  c = bits[0];
  clear tmp;
  for (int i = 1; i < k; i++)
  {
    pow = t;
    //pow.power(i); // yyltodo? T::clear no .power()
    power(pow, i);
    tmp = bits[i] * pow;
    c = c + tmp;
  }
}
//Arithmetic to Binary
template <class T>
void OnlineOp<T>::A2B(vector<T> &bits, const T &c, unsigned int k) { TT_FUNC_NOT_IMPLEMENTED(); }
template <class T>
void OnlineOp<T>::A2B(vector<T> &bits, const T &a)
{
  PRINT_DEBUG_INFO();
  T r, c;
  vector<T> bitr;
  pre_rand(r, bitr);
  PRINT_DEBUG_INFO();

  sub(c, a, r); // c = a-r
  vector<clear> cp;
  reveal({c}, cp);
  PRINT_DEBUG_INFO();

  bigint bcp;
  to_bigint(bcp, cp[0]); // reveal c
  PRINT_DEBUG_INFO();

  T factor, tmp;
  clear ONE = 1;
  // ONE.assign(1);
  PRINT_DEBUG_INFO();

  bigint TWO(2);
  vector<clear> bound;
  PRINT_DEBUG_INFO();

  if (bcp == 0)
  {
    bits = bitr;
    return;
  }
  decompose(bound, clear::pr() - bcp, PSIZE);
  PRINT_DEBUG_INFO();

  lt(factor, bitr, bound, PSIZE);

  //share of 1-factor
  mul_plain(tmp, factor, TWO);
  sub_inplace(factor, tmp);
  add_plain_inplace(factor, ONE);
  PRINT_DEBUG_INFO();

  vector<clear> f, tf;
  vector<T> tg(PSIZE);

  decompose(f, bcp, PSIZE);
  decompose(tf, (TWO << PSIZE) + bcp - clear::pr(), PSIZE);

  PRINT_DEBUG_INFO();
  for (int i = 0; i < PSIZE; i++)
  {
    mul_plain(tg[i], factor, tf[i] - f[i]);
    add_plain_inplace(tg[i], f[i]);
  }
  PRINT_DEBUG_INFO();

  add_bit(bits, tg, bitr);
  bits.pop_back();
}

// out = in[0] + in[1]*key + in[2]*key^2 +...+ in[size-1]*key^{size-1}
template <class T>
void OnlineOp<T>::uhf(T &out, const T &key, const vector<clear> &in, unsigned int size)
{
  if (in.size() != size)
  {
    throw invalid_length();
  }

  mul_plain(out, key, in[size - 1]);
  add_plain_inplace(out, in[size - 2]);
  for (int i = size - 3; i >= 0; i--)
  {
    mul_inplace(out, key);
    add_plain_inplace(out, in[i]);
  }
}
template <class T>
void OnlineOp<T>::legendre(int &out, const T &in)
{
  vector<T> s(2);
  // getTuples(s, SQUARE);
  preprocessing.get_two(DATA_SQUARE, s[0], s[1]);

  mul_inplace(s[1], in);

  vector<clear> c;
  reveal({s[1]}, c);

  bigint bn;
  to_bigint(bn, c[0]);
  out = mpz_legendre(bn.get_mpz_t(), clear::pr().get_mpz_t());
}
template <class T>
int OnlineOp<T>::legendre_prf(const T &key, const T &in)
{
  T tmp;
  add(tmp, key, in);
  int res = 0;
  legendre(res, tmp);
  res = ceil(double(res + 1) / 2);
  return res;
}

/* reveal */
template <class T>
void OnlineOp<T>::reveal(const T &a, clear &r)
{
  output.init_open(P);
  output.prepare_open(a);
  output.exchange(P);
  r = output.finalize_open();
}
template <class T>
void OnlineOp<T>::reveal(const vector<T> &a, vector<clear> &r)
{
  size_t n = a.size();
  r.resize(n);

  output.init_open(P, n);
  for (auto &s : a)
    output.prepare_open(s);
  output.exchange(P);
  for (size_t i = 0; i < n; i++)
    r[i] = output.finalize_open();
}
template <class T>
void OnlineOp<T>::reveal_and_print(const vector<T> &vs, vector<clear> &vc)
{
  reveal(vs, vc);

  size_t n = vc.size();
  for (size_t i = 0; i < n; i++)
    cout << vc[i] << endl;
}
template <class T>
void OnlineOp<T>::reveal_and_print(const vector<T> &vs)
{
  vector<clear> vc;
  reveal_and_print(vs, vc);
}
template <class T>
void OnlineOp<T>::reveal_and_print(const Complex<T> &s) { reveal_and_print({s.real, s.imag}); }
template <class T>
void OnlineOp<T>::reveal_and_print(const vector<Complex<T>> &vs) { TT_FUNC_NOT_IMPLEMENTED(); }

/*inputs*/
template <class T>
void OnlineOp<T>::get_inputs(unsigned int party, T &sa, const clear &inputs)
{
  typename T::Input input0(processor, output);
  input0.reset_all(P);
  input0.add_from_all(inputs);
  input0.exchange();
  sa = input0.finalize(party);
}
template <class T>
void OnlineOp<T>::get_inputs(unsigned int party, Complex<T> &sa, const Complex_Plain<T> &inputs)
{
  get_inputs(party, sa.real, inputs.real);
  get_inputs(party, sa.imag, inputs.imag);
}

