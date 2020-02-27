/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _Processor
#define _Processor

#include "LSSS/Open_Protocol.h"
#include "Online/Machine.h"

#include "Processor_IO.h"

#include <stack>
using namespace std;

struct TempVars {
  gfp ansp;
  Share Sansp;
  bigint aa, aa2;
  // INPUT and LDSI
  gfp rrp, tp, tmpp;
  gfp xip;
};

class Processor {
  // Integer stack (used for loops to store the running variable)
  stack<long> stacki;

  // Optional argument to the tape
  int arg;

  // Registers of various types
  vector<gfp> Cp;
  vector<Share> Sp;
  vector<long> Ri;


  // This is the vector of partially opened values and shares we need to store
  // as the Open commands are split in two
  vector<gfp> PO;
  vector<Share> Sh_PO;
  Open_Protocol OP;

  int online_thread_num;

  // Keep track of how much data sent, and in how many rounds
  unsigned int sent, rounds;

  PRNG prng;

  // To make sure we do not need to keep allocating/deallocating memory
  // we maintain some scratch variables for use in routines
  TempVars temp;

 public:
  // Data structures for input and output of private data
  Processor_IO iop;

 public:
  friend class Instruction;
  Processor() {}
  Processor(int online_thread_num, unsigned int nplayers, Player& P);
  ~Processor();

  int get_thread_num() const {
    return online_thread_num;
  }

  Share& get_Sp_ref(int i) {
    return Sp[i];
  }
  /* Direct access to PO class*/
  void Open_To_All_Begin(vector<gfp>& values, const vector<Share>& S, Player& P, int connection) {
    OP.Open_To_All_Begin(values, S, P, connection);
  }
  void Open_To_All_End(vector<gfp>& values, const vector<Share>& S, Player& P, int connection) {
    OP.Open_To_All_End(values, S, P, connection);
  }

  /* Open/Close Registers*/
  void POpen_Start(const vector<int>& reg, const vector<Share>& vs, int size, Player& P);
  void POpen_Stop(const vector<int>& reg, vector<gfp>& vs, int size, Player& P);

  void RunOpenCheck(Player& P, const string& aux, int connection) {
    OP.RunOpenCheck(P, aux, connection, false);
  }

  unsigned int get_random_uint() {
    return prng.get_uint();
  }

  // Add rounds up and add data sent in
  void increment_counters(unsigned int size) {
    sent += size;
    rounds++;
  }
};

#endif
