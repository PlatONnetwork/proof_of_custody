/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include "Processor.h"

Processor::Processor(int thread_num, unsigned int nplayers,
                     Player &P) : online_thread_num(thread_num), iop(nplayers)
{
  rounds = 0;
  sent = 0;
}

Processor::~Processor()
{
//  fprintf(stderr, "Sent %d elements in %d rounds\n", sent, rounds);
#ifdef VERBOSE
  cout << "dabitgen statistics:" << endl;
  cout << "Produced " << daBitGen->total << " dabits" << endl;
  for (auto &timer : daBitGen->timers)
    cout << timer.first << " took time " << timer.second.elapsed() / 1e6 << endl;
#endif
}

void Processor::POpen_Start(const vector<int> &reg, const vector<Share> &vs, int size, Player &P)
{
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
    for (int i = 0; i < sz; i++)
    {
      Sh_PO.push_back(vs[i]);
    }
  }
  PO.resize(sz * size);
  OP.Open_To_All_Begin(PO, Sh_PO, P, 0);
}

void Processor::POpen_Stop(const vector<int> &reg, vector<gfp> &vs, int size, Player &P)
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

void Processor::POpen_Start(const vector<int> &reg, int size, Player &P)
{
  // printf("POpen_Start : size = %d     reg.size = %lu\n ",size,reg.size());
  int sz = reg.size();
  Sh_PO.clear();
  Sh_PO.reserve(sz * size);
  if (size > 1)
  {
    for (typename vector<int>::const_iterator reg_it = reg.begin();
         reg_it != reg.end(); reg_it++)
    {
      typename vector<Share>::iterator begin = Sp.begin() + *reg_it;
      Sh_PO.insert(Sh_PO.end(), begin, begin + size);
    }
  }
  else
  {
    for (int i = 0; i < sz; i++)
    {
      Sh_PO.push_back(get_Sp_ref(reg[i]));
    }
  }
  PO.resize(sz * size);
  OP.Open_To_All_Begin(PO, Sh_PO, P);
}

void Processor::POpen_Stop(const vector<int> &reg, int size, Player &P)
{
  int sz = reg.size();
  PO.resize(sz * size);
  OP.Open_To_All_End(PO, Sh_PO, P);
  if (size > 1)
  {
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
  }
  else
  {
    for (unsigned int i = 0; i < reg.size(); i++)
    {
      get_Cp_ref(reg[i]) = PO[i];
    }
  }

  increment_counters(reg.size() * size);
}

void Processor::clear_registers()
{
  for (unsigned int i = 0; i < Cp.size(); i++)
  {
    Cp[i].assign_zero();
  }
  for (unsigned int i = 0; i < Sp.size(); i++)
  {
    Sp[i].assign_zero();
  }
  for (unsigned int i = 0; i < Ri.size(); i++)
  {
    Ri[i] = 0;
  }

#ifdef DEBUG
  for (unsigned int i = 0; i < rwp.size(); i++)
  {
    rwp[i] = 0;
  }
  for (unsigned int i = 0; i < rwi.size(); i++)
  {
    rwi[i] = 0;
  }
  for (unsigned int i = 0; i < rwsr.size(); i++)
  {
    rwsr[i] = 0;
  }
  for (unsigned int i = 0; i < rwsb.size(); i++)
  {
    rwsb[i] = 0;
  }
#endif
}
