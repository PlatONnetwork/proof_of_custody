/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include <unistd.h>

#include "Online.h"
#include "Processor/Processor.h"
#include "OnlineOp.h"

void OnlineOp::getTuples(vector<Share> &sp, int opcode)
{
  int thread = Proc.get_thread_num();
  Wait_For_Preproc(opcode, 100, thread, OCD); // temp set 100
  for (int i = 0; i < sp.size(); i++)
  {
    sp[i].set_player(P.whoami());
  }
  switch (opcode)
  {
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
void OnlineOp::add(Share &c, const Share &a, const Share &b)
{
  c = a + b;
}

void OnlineOp::add_plain(Share &c, const Share &a, const gfp &b)
{
  c.add(a, b);
}
// c = a - b (b is share)
void OnlineOp::sub(Share &c, const Share &a, const Share &b)
{
  c = a - b;
}
void OnlineOp::mul_plain(Share &c, const Share &a, const gfp &b)
{
  c.mul(a, b);
}
// a * b = c
void OnlineOp::mul(Share &c, const Share &a, const Share &b)
{
  //phase 0: get triples
  vector<Share> sp(3);
  getTuples(sp, TRIPLE);

  //phase 1: sub and open

  Share epsilon = a - sp[0]; // epsilon = x - a
  Share rho = b - sp[1];     // rho = y - b
  vector<gfp> vc;
  open({epsilon, rho}, vc);
  if (verbose > 1)
  {
    cout << "mul vc[0]:" << vc[0] << endl;
    cout << "mul vc[1]:" << vc[1] << endl;
  }

  //phase 2 get mul share

  Share tmp;
  mul_plain(tmp, sp[1], vc[0]);
  add(sp[2], sp[2], tmp);

  mul_plain(tmp, sp[0], vc[1]);
  add(sp[2], sp[2], tmp);

  gfp cp0 = vc[0] * vc[1];
  add_plain(c, sp[2], cp0);
}
// aa = a^2
void OnlineOp::sqr(Share &aa, const Share &a)
{

  //phase 0: get triples
  vector<Share> sp(2);
  getTuples(sp, SQUARE);

  //phase 1: sub and open

  Share epsilon = a - sp[0]; // epsilon = x - a
  vector<gfp> vc;
  open({epsilon}, vc);
  if (verbose > 1)
  {
    cout << "sqr vc[0]:" << vc[0] << endl;
  }

  //phase 2 get squre share

  Share tmp;
  mul_plain(tmp, sp[0], vc[0]);
  add(sp[1], sp[1], tmp);
  add(sp[1], sp[1], tmp);

  gfp cp0 = vc[0] * vc[0];
  add_plain(aa, sp[1], cp0);
}

void OnlineOp::inv(Share &ia, const Share &a)
{
  Share rshare;
  //get random share r
  rshare = prss.next_share(P);

  if (verbose > 1)
  {
    cout << "random share\n";
    reveal_and_print({rshare});
  }
  Share c;
  //get share of r*a;
  mul(c, rshare, a);
  vector<gfp> ra;

  //open ra;
  open({c}, ra);
  if (ra[0].is_zero())
  {
    throw Processor_Error("Division by zero");
  }

  //invert ra;
  gfp ira;
  ira.invert(ra[0]);
  mul_plain(ia, rshare, ira);
}

// c = a * b^{-1} mod q
void OnlineOp::div(Share &c, const Share &a, const Share &b)
{
  Share ib;
  inv(ib, b);
  mul(c, a, ib);
}

/*Complex ops*/
// c = a + b (b is shared complex)
void OnlineOp::add(Complex &c, const Complex &a, const Complex &b)
{
  add(c.real, a.real, b.real);
  add(c.imag, a.imag, b.imag);
}

// c = a + b (b is plain complex)
void OnlineOp::add_plain(Complex &c, const Complex &a, const Complex_Plain &b)
{
  add_plain(c.real, a.real, b.real);
  add_plain(c.imag, a.imag, b.imag);
}
// c = a - b (b is shared complex)
void OnlineOp::sub(Complex &c, const Complex &a, const Complex &b)
{
  sub(c.imag, a.imag, b.imag);
  sub(c.real, a.real, b.real);
}
// c = a * b (b is plain complex)
void OnlineOp::mul_plain(Complex &c, const Complex &a, const Complex_Plain &b)
{
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
void OnlineOp::mul(Complex &c, const Complex &a, const Complex &b)
{
  Share tmpimag, tmpreal, creal;
  mul(tmpreal, a.real, b.real);
  mul(tmpimag, a.imag, b.imag);
  sub(creal, tmpreal, tmpimag);

  mul(tmpreal, a.real, b.imag);
  mul(tmpimag, a.imag, b.real);
  add(c.imag, tmpreal, tmpimag);
  c.real = creal;
}
// aa = a^2
void OnlineOp::sqr(Complex &aa, const Complex &a)
{
  Share tmpr2, tmpi2, tmpcross;
  sqr(tmpr2, a.real);
  sqr(tmpi2, a.imag);

  mul(tmpcross, a.real, a.imag);
  add(aa.imag, tmpcross, tmpcross);
  sub(aa.real, tmpr2, tmpi2);
}

//ia = a^{-1} mod (q,x^2+1)
void OnlineOp::inv(Complex &ia, const Complex a)
{
  Share rr, ii;
  sqr(rr, a.real);
  sqr(ii, a.imag);

  add(rr, rr, ii);

  inv(rr, rr); // rr = (real^2+imag^2)^{-1}

  Share tmpimag = a.imag;
  tmpimag.negate(); // -imag

  mul(ia.real, a.real, rr);
  mul(ia.imag, tmpimag, rr);
}

// c = a * b^{-1} mod (q, x^2+1)
void OnlineOp::div(Complex &c, const Complex &a, const Complex &b)
{
  Complex ib;
  inv(ib, b);
  mul(c, a, ib);
}
void OnlineOp::open(const vector<Share> &vs, vector<gfp> &vc)
{
  vector<int> start;
  size_t size = 1;
  vc.clear();
  vc.resize(vs.size());

  Proc.POpen_Start(start, vs, size, P);
  Proc.POpen_Stop(start, vc, size, P);
}

void OnlineOp::reveal(const vector<Share> &vs, vector<gfp> &vc)
{
  open(vs, vc);
}
void OnlineOp::reveal_and_print(const vector<Share> &vs, vector<gfp> &vc)
{
  reveal(vs, vc);
  for (int i = 0; i < vc.size(); i++)
  {
    cout << __FUNCTION__ << " " << vc[i] << endl;
  }
}
void OnlineOp::reveal_and_print(const vector<Share> &vs)
{
  vector<gfp> vc;
  reveal_and_print(vs, vc);
}

void OnlineOp::reveal_and_print(const vector<Complex> &vs)
{
  for (int i = 0; i < vs.size(); i++)
  {
    cout << "real and imag are: \n";
    reveal_and_print({vs[i].real, vs[i].imag});
  }
}

void OnlineOp::get_inputs(vector<Share> &inputs) {}
void OnlineOp::get_inputs_dumy(vector<Share> &inputs)
{
  inputs.resize(P.nplayers());
  for (int i = 0; i < inputs.size(); i++)
  {
    inputs[i].set_player(P.whoami());
  }

  vector<int64_t> inputs_dumy = {176, 16, -5, 3, -9, 3, 7, 2, 3};
  inputs_dumy.resize(P.nplayers());
  for (int i = 0; i < inputs.size(); i++)
  {
    Proc.iop.private_input2(i, inputs[i], 1, Proc, P, machine, OCD, inputs_dumy);
  }
}

// the following apis for testing
void OnlineOp::test_add()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs;
  get_inputs_dumy(inputs);

  if (inputs.size() < 1)
    throw invalid_size();

  Share res = inputs[0];
  reveal_and_print({res});
  for (int i = 1; i < P.nplayers(); i++)
  {
    res += inputs[i]; // 176 + 16 + 5 + ...
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_add_plain()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs;
  get_inputs_dumy(inputs);

  if (inputs.size() < 1)
    throw invalid_size();

  gfp tmp;
  tmp.assign(10);
  Share res;
  res.set_player(P.whoami());
  for (int i = 0; i < P.nplayers(); i++)
  {
    add_plain(res, inputs[i], tmp);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}

void OnlineOp::test_mul_plain()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs;
  get_inputs_dumy(inputs);

  if (inputs.size() < 1)
    throw invalid_size();

  gfp tmp;
  tmp.assign(10);
  Share res;
  res.set_player(P.whoami());
  for (int i = 0; i < P.nplayers(); i++)
  {
    mul_plain(res, inputs[i], tmp);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_mul()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs;
  get_inputs_dumy(inputs);

  if (inputs.size() < 1)
    throw invalid_size();

  Share res = inputs[0];
  reveal_and_print({res});
  for (int i = 1; i < P.nplayers(); i++)
  {
    mul(res, inputs[i], res); // 176 * 16 * 5 * ...
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_sqr()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs;
  get_inputs_dumy(inputs);

  if (inputs.size() < 1)
    throw invalid_size();

  Share res;
  res.set_player(P.whoami());
  for (int i = 0; i < P.nplayers(); i++)
  {
    sqr(res, inputs[i]);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_div()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs;
  get_inputs_dumy(inputs);

  if (inputs.size() < 1)
    throw invalid_size();

  Share res = inputs[0];
  reveal_and_print({res});
  for (int i = 1; i < P.nplayers(); i++)
  {
    div(res, inputs[i], res); // 176 / 16 / 5 / ...
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_complex_add()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  Complex res = input_c[0];
  reveal_and_print({res});
  for (int i = 1; i < P.nplayers(); i++)
  {
    add(res, input_c[i], res);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_complex_sub()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  Complex res = input_c[0];
  reveal_and_print({res});
  for (int i = 1; i < P.nplayers(); i++)
  {
    sub(res, input_c[i], res);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_complex_mul_plain()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  vector<gfp> tmp(input_c.size());
  tmp[0].assign(176);
  tmp[1].assign(16);
  tmp[2].assign(-5);

  vector<Complex_Plain> cnstnt(input_c.size());
  for (int i = 0; i < cnstnt.size(); i++)
  {
    cnstnt[i].setValue(tmp[i], tmp[i]);
  }

  Complex res;
  for (int i = 0; i < P.nplayers(); i++)
  {
    mul_plain(res, input_c[i], cnstnt[i]);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}
void OnlineOp::test_complex_mul()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  reverse(inputs2.begin(), inputs2.end());

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  vector<Complex> input_c2 = input_c;
  reverse(input_c2.begin(), input_c2.end());

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  Complex res;
  for (int i = 0; i < P.nplayers(); i++)
  {
    mul(res, input_c[i], input_c2[i]);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}

void OnlineOp::test_complex_sqr()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  Complex res;
  for (int i = 0; i < P.nplayers(); i++)
  {
    sqr(res, input_c[i]);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}

void OnlineOp::test_complex_inv()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  Complex res = input_c[0];
  reveal_and_print({res});

//  reveal_and_print({input_c[1]});
//  reveal_and_print({input_c[2]});
  for (int i = 0; i < P.nplayers(); i++)
  {
    inv(res, input_c[i]);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}

void OnlineOp::test_complex_div()
{
  cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;

  vector<Share> inputs1, inputs2;
  get_inputs_dumy(inputs1);
  get_inputs_dumy(inputs2);

  vector<Complex> input_c(inputs1.size());
  for (int i = 0; i < input_c.size(); i++)
  {
    input_c[i].setValue(inputs1[i], inputs2[i]);
  }

  if (inputs1.size() < 1 || inputs2.size() < 1)
    throw invalid_size();

  Complex res = input_c[0];
  reveal_and_print({res});
  //  reveal_and_print({input_c[1]});
  //  reveal_and_print({input_c[2]});
  for (int i = 1; i < P.nplayers(); i++)
  {
    div(res, input_c[i], res);
    reveal_and_print({res});
  }
  cout << "============================== END ==============================" << endl;
}