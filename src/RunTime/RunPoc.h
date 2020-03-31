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
void run_init(int argc, char *argv[], Config_Info &CI);

void run_poc_setup(BLS &bls, Config_Info &CI);

void run_offline(Config_Info &CI);

void run_poc_compute_enphem_key(vector<Share> &ek,
                                BLS bls,
                                const string msg,
                                Config_Info &CI);

void run_poc_compute_custody_bit_offline(vector<Share> &pre_key,
                                         const vector<Share> &keys,
                                         Config_Info &CI);

int run_poc_compute_custody_bit_online(const vector<Share> &pre_key,
                                       const vector<gfp> &msg,
                                       Config_Info &CI);

//run offline and online in one step
int run_poc_compute_custody_bit(const vector<Share> &keys,
                                const vector<gfp> &msg,
                                Config_Info &CI);

void wait_for_exit(Config_Info &CI); // must be called before run_clear
void output_statistics(Config_Info &CI); // must be called before run_clear, after wait_for_exit
void run_clear(Config_Info &CI);

