/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include <unistd.h>

#include "Online.h"
#include "Processor/Processor.h"
#include "OnlineOp.h"
#include "Group.h"

#include "bls.h"

extern vector<sacrificed_data> SacrificeD;

void online_phase(int online_num, Player& P, offline_control_data& OCD, Machine& machine) {
  printf("Doing online for player %d in online thread %d\n", P.whoami(), online_num);
  fflush(stdout);

  Processor Proc(online_num, P.nplayers(), P);

  ///////////////////////////////////////////////
  OnlineOp online_op(Proc, online_num, P, OCD, machine);
  if (false) { // set true for test
    cout << "used triples: " << online_op.UT.UsedTriples << endl;
    cout << "used squares: " << online_op.UT.UsedSquares << endl;
    cout << "used bits: " << online_op.UT.UsedBit << endl;

    G1Op g1op(Proc, online_num, P, OCD, machine);
    g1op.G1_test_add_aff();
    g1op.G1_test_add_jac();
    g1op.G1_test_add_proj();

    G2Op g2op(Proc, online_num, P, OCD, machine);
    g2op.G2_test_add_aff();
    g2op.G2_test_add_jac();
    g2op.G2_test_add_proj();
  }

  online_op.test_uhf();
  online_op.test_legendre();
  online_op.test_get_inputs();
  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);
  //  bls.dstb_keygen(P);
  bls.keygen();
  cout << "secre key share: " << endl;
  print_mclBnFr(bls.get_sk());

  cout << "public key: " << endl;
  print_mclBnG1(bls.vk);

  G2_Affine_Coordinates ac;
  //bls.dstb_sign(ac, "123456", Proc, online_num, P, OCD, machine);
  ///////////////////////////////////////////////

  // MAC/Hash Check
  if (online_num == 0) {
    Proc.RunOpenCheck(P, machine.get_IO().Get_Check(), 0);
    Proc.RunOpenCheck(P, machine.get_IO().Get_Check(), 2);
  } else {
    Proc.RunOpenCheck(P, "", 0);
    Proc.RunOpenCheck(P, "", 2);
  }

  printf("Exiting online phase : %d\n", online_num);
}
