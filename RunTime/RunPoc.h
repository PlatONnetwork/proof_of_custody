#pragma once

#include "Offline/offline_data.h"
#include "Online/Machine.h"
#include "System/SystemData.h"
#include "poc.h"

#include <openssl/ssl.h>

//vector<sacrificed_data> SacrificeD;
/*
* Initate the configure information
*/
void Run_Init(int argc, char *argv[], Config_Info &CI);
void Run_Clear(Config_Info &CI);

void Run_PocSetup(BLS &bls, Config_Info &CI);

void Run_PocEphemKey(vector<Share> &ek, BLS bls, const string msg, Config_Info &CI);

void Run_PocGenProof(Config_Info &CI);

//-----------------the following will be removed------------------//

void Init(int argc, char *argv[], Config_Info &CI);

/*
* run proof of custody with mpc
*/
void Run_Poc(BLS &bls, Config_Info &CI);