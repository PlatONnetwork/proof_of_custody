/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _offline_phases
#define _offline_phases

#include "FHE_Factory.h"
#include "System/Player.h"
#include "offline_data.h"

enum ODtype {
  Triples,
  Squares,
  Bits
};
int check_exit(int num_online, const Player &P, offline_control_data &OCD, ODtype T);
bool propose_what_to_do(int num_online, Player &P, int &finish,
                        vector<int> &make_inputs, bool &minput_global,
                        offline_control_data &OCD,
                        int verbose);

void mult_phase(int num_online, Player &P, int fake_sacrifice,
                offline_control_data &OCD,
                const FHE_PK &pk, const FHE_SK &sk, const FFT_Data &PTD,
                FHE_Industry &industry,
                int verbose);

void square_phase(int num_online, Player &P, int fake_sacrifice,
                  offline_control_data &OCD,
                  const FHE_PK &pk, const FHE_SK &sk, const FFT_Data &PTD,
                  FHE_Industry &industry,
                  int verbose);

void bit_phase(int num_online, Player &P, int fake_sacrifice,
               offline_control_data &OCD,
               const FHE_PK &pk, const FHE_SK &sk, const FFT_Data &PTD,
               FHE_Industry &industry,
               int verbose);

/* As well as doing the input generation, this also acts as a bit
 * of a control to ensure things close down cleanly
 */
void inputs_phase(int num_online, Player &P, int fake_sacrifice,
                  offline_control_data &OCD, const FHE_PK &pk,
                  const FHE_SK &sk, const FFT_Data &PTD,
                  FHE_Industry &industry,
                  int verbose);




void mult_phase(int num_online, Player &P, int fake_sacrifice,
                offline_control_data &OCD, int verbose);

void square_phase(int num_online, Player &P, int fake_sacrifice,
                  offline_control_data &OCD,int verbose);

void bit_phase(int num_online, Player &P, int fake_sacrifice,
               offline_control_data &OCD, int verbose);

/* As well as doing the input generation, this also acts as a bit
 * of a control to ensure things close down cleanly
 */
void inputs_phase(int num_online, Player &P, int fake_sacrifice,
                  offline_control_data &OCD, int verbose);

#endif
