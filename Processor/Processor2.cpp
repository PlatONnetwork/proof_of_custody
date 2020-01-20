/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include "Processor.h"
#include "GC/Garbled.h"
#include "Local/Local_Functions.h"
#include "Offline/DABitMachine.h"

extern MaliciousDABitMachine daBitMachine;
extern Base_Circuits Global_Circuit_Store;
extern Local_Functions Global_LF;

void Processor::POpen_Start2(const vector<int> &reg, const vector<Share> &vs, int size, Player &P)
{
  // printf("POpen_Start : size = %d     reg.size = %lu\n ",size,reg.size());
  int sz = vs.size();
  Sh_PO.clear();
  Sh_PO.reserve(sz * size);
  if (size > 1)
  {
#if 0
    for (typename vector<int>::const_iterator reg_it = reg.begin();
         reg_it != reg.end(); reg_it++)
    {
      typename vector<Share>::iterator begin = Sp.begin() + *reg_it;
      Sh_PO.insert(Sh_PO.end(), begin, begin + size);
    }
#endif
  }
  else
  {
    //stringstream os;
    //vs[0].output(os, true);
    //printf("Share :  %s \n", os.str().c_str());
    for (int i = 0; i < sz; i++)
    {
      Sh_PO.push_back(vs[i]);
    }
  }
  PO.resize(sz * size);
  OP.Open_To_All_Begin(PO, Sh_PO, P, 0);
}

void Processor::POpen_Stop2(const vector<int> &reg, vector<gfp> &vs, int size, Player &P)
{
  int sz = vs.size();
  PO.resize(sz * size);
  OP.Open_To_All_End(PO, Sh_PO, P, 0);
  if (size > 1)
  {
#if 0
    typename vector<gfp>::iterator PO_it = PO.begin();
    for (typename vector<int>::const_iterator reg_it = reg.begin();
         reg_it != reg.end(); reg_it++)
    {
      for (typename vector<gfp>::iterator C_it = Cp.begin() + *reg_it;
           C_it != Cp.begin() + *reg_it + size; C_it++)
      {
        *C_it = *PO_it;
        PO_it++;
      }
    }
#endif
  }
  else
  {
    for (unsigned int i = 0; i < sz; i++)
    {
      vs[i] = PO[i];
    }
  }

  increment_counters(sz * size);
}
