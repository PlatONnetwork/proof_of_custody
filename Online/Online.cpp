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

extern vector<sacrificed_data> SacrificeD;

void online_phase(int online_num, Player &P, offline_control_data &OCD,
                  Machine &machine)
{
  printf("Doing online for player %d in online thread %d\n", P.whoami(),
         online_num);
  fflush(stdout);

  // Wait until enough triples have been produced
  bool wait = true;
  while (wait)
  {
    wait = false;
    OCD.mul_mutex[online_num].lock();
    if (OCD.totm[online_num] < OCD.minm)
    {
      wait = true;
    }
    OCD.mul_mutex[online_num].unlock();

    OCD.sqr_mutex[online_num].lock();
    if (OCD.tots[online_num] < OCD.mins)
    {
      wait = true;
    }
    OCD.sqr_mutex[online_num].unlock();

    if (wait)
    {
      sleep(1);
    }
    sleep(3);
  }
  printf("Starting online phase\n");

  // Initialise the program
  Processor Proc(online_num, P.nplayers(), P);

  bool flag = true;

  // synchronize
  fprintf(stderr, "Signal online thread ready %d\n", online_num);
  machine.Signal_Ready(online_num, true);

  while (flag)
  { // Pause online thread until it has a program to run
    int program = machine.Pause_While_Nothing_To_Do(online_num);

    if (program == -1)
    {
      flag = false;
      fprintf(stderr, "\tThread %d terminating\n", online_num);
    }
    else
    { // Execute the program
      //Proc.execute(machine.progs[program], machine.get_OTI_arg(online_num), P, machine, OCD);

      // MAC/Hash Check
      if (online_num == 0)
      {
        Proc.RunOpenCheck(P, machine.get_IO().Get_Check(), 0);
        Proc.RunOpenCheck(P, machine.get_IO().Get_Check(), 2);
      }
      else
      {
        Proc.RunOpenCheck(P, "", 0);
        Proc.RunOpenCheck(P, "", 2);
      }

      machine.Signal_Finished_Tape(online_num);
    }
  }

  ///////////////////////

  for (int i = 0; i < 1; i++)
  {
    OnlineOp online_op(Proc, online_num, P, OCD, machine);
    vector<Share> inputs_dumy;
    online_op.test_add();
    online_op.test_add_plain();
    online_op.test_mul_plain();
    online_op.test_mul();
    online_op.test_sqr();
    online_op.test_div();

    online_op.test_complex_add();
    online_op.test_complex_sub();
    online_op.test_complex_mul_plain();
    online_op.test_complex_mul();
    online_op.test_complex_sqr();
    online_op.test_complex_inv();
    online_op.test_complex_div();

    cout << "used triples: " << online_op.UT.UsedTriples << endl;
    cout << "used squares: " << online_op.UT.UsedSquares << endl;
    cout << "used bits: " << online_op.UT.UsedBit << endl;
  }

  G1Op g1op(Proc, online_num, P, OCD, machine);
  g1op.G1_test_add_aff();
  g1op.G1_test_add_jac();
  g1op.G1_test_add_proj();

  G2Op g2op(Proc, online_num, P, OCD, machine);
  g2op.G2_test_add_aff();
  g2op.G2_test_add_jac();
  g2op.G2_test_add_proj();

  ///////////////////////

  // Run checks again
  if (online_num == 0)
  {
    Proc.RunOpenCheck(P, machine.get_IO().Get_Check(), 0);
    Proc.RunOpenCheck(P, machine.get_IO().Get_Check(), 2);
  }
  else
  {
    Proc.RunOpenCheck(P, "", 0);
    Proc.RunOpenCheck(P, "", 2);
  }

  machine.Lock_Until_Ready(online_num);

  // Signal offline threads I am dying now
  OCD.OCD_mutex[online_num].lock();
  OCD.finish_offline[online_num] = 1;
  OCD.finished_online[online_num] = 1;
  OCD.OCD_mutex[online_num].unlock();
  printf("Exiting online phase : %d\n", online_num);
}
