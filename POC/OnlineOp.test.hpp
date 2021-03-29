#include "OnlineOp.h"

// the following apis for testing

template <class T>
void OnlineOp<T>::test_add()
{
  PRINT_TEST_BEG();

  T a, b, c;
  clear ca = 4, cb = 2, cc = 0;
  get_inputs(0, a, ca);
  get_inputs(1, b, cb);
  add(c, a, b);

  clear result;
  reveal(c, result);
  cout << "add result: " << result << endl;

  add_inplace(c, a);
  reveal(c, result);
  cout << "add_inplace result: " << result << endl;

  add_plain(c, a, cb);
  reveal(c, result);
  cout << "add_plain result: " << result << endl;

  add_plain_inplace(c, cb);
  reveal(c, result);
  cout << "add_plain_inplace result: " << result << endl;

  PRINT_TEST_END();
}
template <class T>
void OnlineOp<T>::test_sub()
{
  PRINT_TEST_BEG();

  T a, b, c;
  clear ca = 4, cb = 2, cc = 0;
  get_inputs(0, a, ca);
  get_inputs(1, b, cb);
  sub(c, a, b);

  clear result;
  reveal(c, result);
  cout << "sub result: " << result << endl;

  sub_inplace(c, a);
  reveal(c, result);
  cout << "sub_inplace result: " << result << endl;

  sub_plain(c, a, cb);
  reveal(c, result);
  cout << "sub_plain result: " << result << endl;

  sub_plain_inplace(c, cb);
  reveal(c, result);
  cout << "sub_plain_inplace result: " << result << endl;

  PRINT_TEST_END();
}

template <class T>
void OnlineOp<T>::test_mul()
{
  PRINT_TEST_BEG();

  T a, b, c;
  clear ca = 4, cb = 2, cc = 0;
  get_inputs(0, a, ca);
  get_inputs(1, b, cb);
  mul(c, a, b);

  clear result;
  reveal(c, result);
  cout << "mul result: " << result << endl;

  mul_inplace(c, a);
  reveal(c, result);
  cout << "mul_inplace result: " << result << endl;

  mul_plain(c, a, cb);
  reveal(c, result);
  cout << "mul_plain result: " << result << endl;

  mul_plain_inplace(c, cb);
  reveal(c, result);
  cout << "mul_plain_inplace result: " << result << endl;

  PRINT_TEST_END();
}

template <class T>
void OnlineOp<T>::test_div()
{
  PRINT_TEST_BEG();

  T a, b, c;
  clear ca = 33, cb = 3, cc = 0;
  get_inputs(0, a, ca);
  get_inputs(1, b, cb);
  div(c, a, b);

  clear result;
  reveal(c, result);
  cout << "div result: " << result << endl;

  div_inplace(c, a);
  reveal(c, result);
  cout << "div_inplace result: " << result << endl;

  // div_plain(c, a, cb);
  // reveal(c, result);
  // cout << "div_plain result: " << result << endl;

  // div_plain_inplace(c, cb);
  // reveal(c, result);
  // cout << "div_plain_inplace result: " << result << endl;

  PRINT_TEST_END();
}
template <class T>
void OnlineOp<T>::test_sqr()
{
  PRINT_TEST_BEG();

  T s, r;
  clear c = 3;
  clear result;

  get_inputs(0, s, c);

  sqr(r, s);
  reveal(r, result);
  cout << "sqr result: " << result << endl;

  sqr_inplace(r);
  reveal(r, result);
  cout << "sqr_inplace result: " << result << endl;

  PRINT_TEST_END();
}

template <class T>
void OnlineOp<T>::test_inv()
{
  PRINT_TEST_BEG();

  T s, r;
  clear c = 3;
  clear result;

  get_inputs(0, s, c);

  inv(r, s);
  reveal(r, result);
  cout << "inv result: " << result << endl;

  inv_inplace(r);
  reveal(r, result);
  cout << "inv_inplace result: " << result << endl;

  PRINT_TEST_END();
}

//
//
//

template <class T>
void OnlineOp<T>::test_uhf()
{
  PRINT_TEST_BEG();
  clear k = 2;
  T key;
  get_inputs(0, key, k);
  vector<clear> in(4);
  for (int i = 0; i < in.size(); i++)
  {
    in[i] = i + 1;
  }

  T out;
  uhf(out, key, in, in.size());

  reveal_and_print({out});

  PRINT_TEST_END();
}
template <class T>
void OnlineOp<T>::test_legendre()
{
  PRINT_TEST_BEG();

  string k("123"), d("-12456789");

  mpz_class mc_k(k, 10);
  mpz_class mc_d(d, 10);
  bigint key(mc_k);
  bigint data(mc_d);

  clear key_p, data_p;
  to_gfp(key_p, key);
  to_gfp(data_p, data);

  T key_s, data_s;
  get_inputs(0, key_s, key_p);
  get_inputs(0, data_s, data_p);

  int res_s = legendre_prf(key_s, data_s);

  bigint in = key + data;

  int res = mpz_legendre(in.get_mpz_t(), gfp::pr().get_mpz_t());
  res = ceil(double(res + 1) / 2);

  cout << "mpc leg value: " << res_s << endl;
  cout << "plain leg value: " << res << endl;

  PRINT_TEST_END();
}
template <class T>
void OnlineOp<T>::test_get_inputs()
{
  PRINT_TEST_BEG();

  clear a(10 + P.my_num()), b(20 + P.my_num());
  Complex_Plain<T> cp(a, b);
  Complex<T> cc0, cc1;
  get_inputs(0, cc0, cp);
  get_inputs(1, cc1, cp);

  reveal_and_print(cc0);
  reveal_and_print(cc1);

  PRINT_TEST_END();
}
template <class T>
void OnlineOp<T>::test_bit_ops()
{
  PRINT_TEST_BEG();

  vector<clear> a(2), b(2);
  vector<T> sa(2), sb(2);
  vector<T> xor_share(4), or_share(4), and_share(4);
  vector<T> xor_plain(4), or_plain(4), and_plain(4);

  vector<clear> c(4);
  vector<T> sc(4);

  c[0].assign_zero();
  c[1].assign_one();
  c[2].assign_one();
  c[3].assign_one();

  for (int i = 0; i < 4; i++)
  {
    get_inputs(0, sc[i], c[i]);
  }

  a[0].assign_one();
  a[1].assign_one();
  b[0].assign_zero();
  b[1].assign_one();

  cout << "plain a[0]:" << a[0] << endl;
  cout << "plain a[1]:" << a[1] << endl;
  cout << "plain b[0]:" << b[0] << endl;
  cout << "plain b[1]:" << b[1] << endl;
  cout << "---------------" << endl;

  for (int i = 0; i < 2; i++)
  {
    get_inputs(0, sa[i], a[i]);
    get_inputs(0, sb[i], b[i]);
  }

  reveal_and_print(sa);
  reveal_and_print(sb);

  T ltbit;
  lt(ltbit, sb, sa, 2);
  cout << "(sb < sa)? less than test: \n";
  reveal_and_print({ltbit});
  cout << "---------------" << endl;

  clear neg, neg2;
  vector<clear> bits;
  // neg.assign(-75);
  neg = -75;
  decompose(bits, neg, PSIZE);
  for (int i = 0; i < PSIZE; i++)
  {
    cout << bits[i];
  }
  cout << endl;

  B2A(neg2, bits, PSIZE);
  cout << "neg2:" << neg2 << endl;
  cout << "---------------" << endl;

  //bigint bn = gfp::pr();
  bigint bn(10);
  decompose(bits, bn, 4);
  for (int i = 0; i < 4; i++)
  {
    cout << bits[i];
  }
  cout << endl;

  vector<T> res;
  add_bit(res, sa, sb);
  cout << "res size: " << res.size() << endl;

  reveal_and_print(res);
  cout << "---------------" << endl;

  vector<T> bbits;
  T svalue;
  vector<T> svaluebits;
  vector<clear> pbits, sbits;
  pre_rand(svalue, svaluebits);

  reveal(svaluebits, pbits);

  A2B(bbits, svalue);
  reveal(bbits, sbits);
  cout << "A2B results:\n";
  for (int i = 0; i < sbits.size(); i++)
  {
    cout << sbits[i];
  }
  cout << endl;

  cout << "original bits:\n";
  for (int i = 0; i < pbits.size(); i++)
  {
    cout << pbits[i];
  }
  cout << endl;

  for (int i = 0; i < pbits.size(); i++)
  {
    cout << pbits[i] - sbits[i];
  }
  cout << endl;
  // for (int i = bbits.size() - 1; i >= 0; i++) {
  //   reveal_and_print({bbits[i]});
  // }
  cout << "---------------" << endl;

  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
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
  cout << "---------------" << endl;

  T kxor_share;
  KXOR(kxor_share, sc, sc.size());

  cout << "KXOR in Plain :\n";
  cout << (c[0] ^ c[1] ^ c[2] ^ c[3]) << endl;

  cout << "KXOR in Share :\n";
  reveal_and_print({kxor_share});

  vector<T> pOR;
  prefix_OR(pOR, sc, sc.size());

  cout << "Prefix OR:\n";
  reveal_and_print(pOR);

  PRINT_TEST_END();
}

template <class T>
void OnlineOp<T>::test_performance()
{
  PRINT_TEST_BEG();

  Timer timer;
  timer.start();

  int times = 10;
  int size = 97;
  vector<T> a(size), b(size), c(size);
  vector<clear> ca(size), cb(size), cc(size);
  get_inputs(0, a, ca);
  get_inputs(1, b, cb);
  cout << "mul get_inputs:" << timer.elapsed_then_reset() << endl;
  usleep(10);
  P.comm_stats.print();

  timer.start();

  mul(c, a, b);
  cout << "mul perf:" << timer.elapsed_then_reset() << endl;

  mul_inplace(c, a);
  cout << "mul_inplace perf:" << timer.elapsed_then_reset() << endl;

  mul_plain(c, a, cb);
  cout << "mul_plain perf:" << timer.elapsed_then_reset() << endl;

  mul_plain_inplace(c, cb);
  cout << "mul_plain_inplace perf:" << timer.elapsed_then_reset() << endl;

  usleep(10);
  P.comm_stats.print();

  PRINT_TEST_END();
}