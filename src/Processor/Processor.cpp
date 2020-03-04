/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include "Processor.h"

Processor::Processor(int thread_num, unsigned int nplayers, Player& P)
    : online_thread_num(thread_num), iop(nplayers) {
  rounds = 0;
  sent = 0;
}

Processor::~Processor() {}

void Processor::POpen_Start(const vector<int>& reg, const vector<Share>& vs, int size, Player& P) {
  int sz = vs.size();
  Sh_PO.clear();
  Sh_PO.reserve(sz * size);
  if (size > 1) {
    throw invalid_size();
  } else {
    for (int i = 0; i < sz; i++) {
      Sh_PO.push_back(vs[i]);
    }
  }
  PO.resize(sz * size);
  OP.Open_To_All_Begin(PO, Sh_PO, P, 0);
}

void Processor::POpen_Stop(const vector<int>& reg, vector<gfp>& vs, int size, Player& P) {
  int sz = vs.size();
  PO.resize(sz * size);
  OP.Open_To_All_End(PO, Sh_PO, P, 0);
  if (size > 1) {
    throw invalid_size();
  } else {
    for (unsigned int i = 0; i < sz; i++) {
      vs[i] = PO[i];
    }
  }

  increment_counters(sz * size);
}
