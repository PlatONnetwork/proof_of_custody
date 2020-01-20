/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include "offline_IO_production.h"
#include "config.h"
#include "offline_FHE.h"
#include "offline_subroutines.h"

/* Make sz_IO_batch input triples */
void make_IO_data(Player &P, int fake_sacrifice, PRSS &prss,
                  unsigned int player_num, list<Share> &a, list<gfp> &opened,
                  offline_control_data &OCD, Open_Protocol &OP,
                  unsigned int online_thread)
{
  a.resize(0);
  opened.resize(0);

  if (Share::SD.type == Full && !fake_sacrifice)
    {
      //offline_FHE_IO(P, player_num, a, opened, pk, sk, PTD, OCD, online_thread, industry);
    }
  else if (fake_sacrifice)
    {
      make_IO_data_fake(P, player_num, a, opened);
    }
  else
    {
      make_IO_data_non_Full(P, prss, player_num, a, opened, OP);
    }
}
