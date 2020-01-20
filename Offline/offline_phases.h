/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _offline_phases
#define _offline_phases

#include "System/Player.h"
#include "offline_data.h"

void mult_phase(int num_online, Player &P, int fake_sacrifice,
                offline_control_data &OCD, int verbose);

void square_phase(int num_online, Player &P, int fake_sacrifice,
                  offline_control_data &OCD, int verbose);

void bit_phase(int num_online, Player &P, int fake_sacrifice,
               offline_control_data &OCD, int verbose);

void inputs_phase(int num_online, Player &P, int fake_sacrifice,
                  offline_control_data &OCD, int verbose);

#endif
