#pragma once

#include "Offline/offline_data.h"
#include "Online/Machine.h"
#include "System/SystemData.h"
#include "poc.h"

#include <openssl/ssl.h>

/*
** Call chain:
** init -> setup -> offline -> ephemkey/genproof/online -> wait -> clear
*/

/*
* Initate the configure information
*/
void run_init(int argc, char* argv[], Config_Info& CI);

void run_poc_setup(BLS& bls, Config_Info& CI);

void run_offline(Config_Info& CI);

void run_poc_compute_enphem_key(vector<Share>& ek, BLS bls, const string msg, Config_Info& CI);
int run_poc_compute_custody_bit(const vector<Share>& keys, const vector<gfp>& msg, Config_Info& CI);
void run_online(Config_Info& CI);

void wait_for_exit(Config_Info& CI); // must be called before Run_Clear
void run_clear(Config_Info& CI);
