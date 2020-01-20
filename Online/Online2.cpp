/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include <unistd.h>

#include "Online.h"
#include "Processor/Processor.h"

void getTriples(Processor &Proc, vector<Share> &sp, offline_control_data &OCD, int opcode)
{
  int thread = Proc.get_thread_num();
  Wait_For_Preproc(opcode, 100, thread, OCD); // temp set 100

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
    break;
  case BIT:
    if (sp.size() != 1)
      throw invalid_size();
    OCD.bit_mutex[thread].lock();
    sp[0] = SacrificeD[thread].BD.bb.front();
    SacrificeD[thread].BD.bb.pop_front();
    OCD.bit_mutex[thread].unlock();
    break;
  default:
    throw bad_value();
    break;
  }
}

// a + b = c
void OnlineOp::add(Share &a, Share &b, Share &c)
{
  c = a + b;
}
// a * b = c
void OnlineOp::mul(Share &a, Share &b, Share &c)
{
  /*
  phase 0 triples
  */
  vector<Share> sp(3);
  for (int i = 0; i < sp.size(); i++)
  {
    sp[i].set_player(P.whoami());
  }

  getTriples(Proc, sp, OCD, TRIPLE);

  /*
  phase 1
  00 00 00 50 00 00 00 01 00 00 00 00 00 00 00 02     TRIPLE[a,b,c] --> Sp[1,0,2]
  00 00 00 02 00 00 00 05 00 00 00 A3                 (y) A3 --> Sp[5]
  00 00 00 26 00 00 00 04 00 00 00 05 00 00 00 00     (y-b) Sp[5] - Sp[0] --> Sp[4]
  00 00 00 02 00 00 00 05 00 00 00 A6                 (x) A3 --> Sp[5]
  00 00 00 26 00 00 00 03 00 00 00 05 00 00 00 01     (x-a) Sp[5] - Sp[1] --> Sp[3]
  00 00 00 A0 00 00 00 02 00 00 00 03 00 00 00 04     (x-a,y-b) Sp[3,4] --> Open
  00 00 00 A1 00 00 00 02 00 00 00 01 00 00 00 02     Open --> Cp[1,2]
  */
  auto xa = a - sp[0]; // x - a
  auto yb = b - sp[1]; // y - b
  vector<gfp> vc;
  open({xa, yb}, vc);
  if (verbose > 1)
  {
    cout << "mul vc[0]:" << vc[0] << endl;
    cout << "mul vc[1]:" << vc[1] << endl;
  }

  /*
  phase 2
  00 00 00 31 00 00 00 03 00 00 00 00 00 00 00 01     Sp[0] * Cp[1] --> Sp[3]
  00 00 00 21 00 00 00 00 00 00 00 02 00 00 00 03     Sp[2] + Sp[3] --> Sp[0]
  00 00 00 31 00 00 00 02 00 00 00 01 00 00 00 02     Sp[1] * Cp[2] --> Sp[2]
  00 00 00 21 00 00 00 01 00 00 00 00 00 00 00 02     Sp[0] + Sp[2] --> Sp[1]
  00 00 00 30 00 00 00 00 00 00 00 01 00 00 00 02     Cp[1] * Cp[2] --> Cp[0]
  00 00 00 22 00 00 00 00 00 00 00 01 00 00 00 00     Sp[1] + Cp[0] --> Sp[0]
  00 00 00 A0 00 00 00 01 00 00 00 00                 Sp[0] --> Open
  00 00 00 A1 00 00 00 01 00 00 00 00                 Open --> Cp[0]
  */
  {
    Share sp3 = sp[1] * vc[0];
    Share sp0 = sp[2] + sp3;

    Share sp2 = sp[0] * vc[1];
    Share sp1 = sp0 + sp2;

    gfp cp0 = vc[0] * vc[1];
    c.set_player(P.whoami());
    c.add(sp1, cp0, P.get_mac_keys());
    c.add(sp1, cp0, P.get_mac_keys());
  }
}
// a / b = c
void OnlineOp::div(Share &a, Share &b, Share &c)
{
  /*
  phase 0 triples
  */
  vector<Share> sp(3); // TRIPLE
  for (int i = 0; i < sp.size(); i++)
  {
    sp[i].set_player(P.whoami());
  }
  getTriples(Proc, sp, OCD, TRIPLE);

  vector<Share> sps(2); // SQUARE
  for (int i = 0; i < sps.size(); i++)
  {
    sps[i].set_player(P.whoami());
  }
  getTriples(Proc, sps, OCD, SQUARE);

  /*
  phase 1
  00 00 00 50 00 00 00 04 00 00 00 00 00 00 00 05     TRIPLE[a,b,c] --> Sp[4,0,5]
  00 00 00 52 00 00 00 03 00 00 00 06                 SQUARE[a,b] --> Sp[3,6]
  00 00 00 26 00 00 00 01 00 00 00 03 00 00 00 04     Sp[3] - Sp[4] --> Sp[1]
  00 00 00 02 00 00 00 06 00 00 00 A3                 A3 --> Sp[6]
  00 00 00 26 00 00 00 02 00 00 00 06 00 00 00 00     Sp[6] - Sp[0] --> Sp[2]
  00 00 00 A0 00 00 00 02 00 00 00 01 00 00 00 02     Sp[1,2] --> Open
  00 00 00 A1 00 00 00 02 00 00 00 01 00 00 00 02     --> Cp[1,2]
  */
  gfp cpa, cpb;
  Share spa, spb, spc;
  spa.set_player(P.whoami());
  spb.set_player(P.whoami());
  spc.set_player(P.whoami());
  Share sp1 = sps[0] - sp[0];
  Share sp2 = b - sp[1];

  vector<gfp> cp12;
  open({sp1, sp2}, cp12);
  if (verbose > 1)
  {
    cout << "div cp12[0]:" << cp12[0] << endl;
    cout << "div cp12[1]:" << cp12[1] << endl;
  }

  /*
  phase 2
  00 00 00 31 00 00 00 01 00 00 00 00 00 00 00 01     Sp[0] * Cp[1] --> Sp[1]
  00 00 00 21 00 00 00 00 00 00 00 05 00 00 00 01     Sp[5] + Sp[1] --> Sp[0]
  00 00 00 31 00 00 00 02 00 00 00 04 00 00 00 02     Sp[4] * Cp[2] --> Sp[2]
  00 00 00 21 00 00 00 01 00 00 00 00 00 00 00 02     Sp[0] + Sp[2] --> Sp[1]
  00 00 00 30 00 00 00 00 00 00 00 01 00 00 00 02     Cp[1] * Cp[2] --> Cp[0]
  00 00 00 22 00 00 00 00 00 00 00 01 00 00 00 00     Sp[1] + Cp[0] --> Sp[0]
  00 00 00 A0 00 00 00 01 00 00 00 00                 Sp[0] --> Open
  00 00 00 A1 00 00 00 01 00 00 00 01                 Open --> Cp[1]
  */
  sp1 = sp[1] * cp12[0];
  Share sp0 = sp[2] + sp1;
  sp2 = sp[0] * cp12[1];
  sp1 = sp0 + sp2;
  gfp cp0 = cp12[0] * cp12[1];
  sp0.add(sp1, cp0, P.get_mac_keys());

  vector<gfp> vcp0;
  open({sp0}, vcp0);
  if (verbose > 1)
  {
    cout << "div vcp0[0]:" << vcp0[0] << endl;
  }

  /*
  phase 3
  00 00 00 50 00 00 00 01 00 00 00 00 00 00 00 02     TRIPLE[a,b,c] --> Sp[1,0,2]
  00 00 00 01 00 00 00 02 00 00 00 01                 1 --> Cp[2]
  00 00 00 01 00 00 00 00 00 00 00 01                 1 --> Cp[0]
  00 00 00 31 00 00 00 04 00 00 00 03 00 00 00 00     Sp[3] * Cp[0] --> Sp[4]
  00 00 00 02 00 00 00 05 00 00 00 A6                 A6 --> Sp[5]
  00 00 00 26 00 00 00 03 00 00 00 05 00 00 00 01     Sp[5] - Sp[1] --> Sp[3]
  */
  getTriples(Proc, sp, OCD, TRIPLE);
  gfp cp2 = 1;
  cp0 = 1;
  Share sp4 = sps[0] * cp0;
  Share sp3 = a - sp[0];

  /*
  phase 4
  00 00 00 34 00 00 00 00 00 00 00 02 00 00 00 01     INV(Cp[1]) * Cp[2] --> Cp[0]
  */
  if (vcp0[0].is_zero())
    throw Processor_Error("Division by zero from register");
  gfp tmp;
  tmp.invert(vcp0[0]);
  tmp.mul(cp2);
  cp0 = tmp;

  /*
  phase 5
  00 00 00 31 00 00 00 05 00 00 00 04 00 00 00 00     Sp[4] * Cp[0] --> Sp[5]
  00 00 00 26 00 00 00 04 00 00 00 05 00 00 00 00     Sp[5] - Sp[0] --> Sp[4]
  00 00 00 A0 00 00 00 02 00 00 00 03 00 00 00 04     Sp[3,4] --> Open
  00 00 00 A1 00 00 00 02 00 00 00 01 00 00 00 02     --> Cp[1,2]
  00 00 00 31 00 00 00 03 00 00 00 00 00 00 00 01     Sp[0] * Cp[1] --> Sp[3]
  00 00 00 21 00 00 00 00 00 00 00 02 00 00 00 03     Sp[2] + Sp[3] --> Sp[0]
  00 00 00 31 00 00 00 02 00 00 00 01 00 00 00 02     Sp[1] * Cp[2] --> Sp[2]
  00 00 00 21 00 00 00 01 00 00 00 00 00 00 00 02     Sp[0] + Sp[2] --> Sp[1]
  00 00 00 30 00 00 00 00 00 00 00 01 00 00 00 02     Cp[1] * Cp[2] --> Cp[0]
  00 00 00 22 00 00 00 00 00 00 00 01 00 00 00 00     Sp[1] + Cp[0] --> Sp[0]
  00 00 00 A0 00 00 00 01 00 00 00 00                 Sp[0] --> Open
  00 00 00 A1 00 00 00 01 00 00 00 00                 --> Cp[0]
  */
  Share sp5 = sp4 * cp0;
  sp4 = sp5 - sp[1];
  vector<gfp> cp34;
  open({sp3, sp4}, cp34);
  if (verbose > 1)
  {
    cout << "div cp34[0]:" << cp34[0] << endl;
    cout << "div cp34[1]:" << cp34[1] << endl;
  }

  //
  sp3 = sp[1] * cp34[0];
  sp0 = sp[2] + sp3;
  sp2 = sp[0] * cp34[1];
  sp1 = sp0 + sp2;
  cp0 = cp34[0] * cp34[1];
  c.add(sp1, cp0, P.get_mac_keys());
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

void OnlineOp::get_inputs(vector<Share> &inputs) {}
void OnlineOp::get_inputs_dumy(vector<Share> &inputs)
{
  inputs.resize(P.nplayers());
  for (int i = 0; i < inputs.size(); i++)
  {
    inputs[i].set_player(P.whoami());
  }

  vector<int64_t> inputs_dumy = {176, 16, 5, 3, -9, 3, 7, 2, 3};
  inputs_dumy.resize(P.nplayers());
  for (int i = 0; i < inputs.size(); i++)
  {
    Proc.iop.private_input2(i, inputs[i], 0, Proc, P, machine, OCD, inputs_dumy);
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
