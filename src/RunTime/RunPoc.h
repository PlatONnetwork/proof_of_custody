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
void Run_Init(int argc, char* argv[], Config_Info& CI);

void Run_PocSetup(BLS& bls, Config_Info& CI);

void Run_Offline(Config_Info& CI);

void Run_PocEphemKey(vector<Share>& ek, BLS bls, const string msg, Config_Info& CI);
int Run_PocGenProof(const vector<Share>& keys, const vector<gfp>& msg, Config_Info& CI);
void Run_Online(Config_Info& CI);

void Wait_ForExit(Config_Info& CI); // must be called before Run_Clear
void Run_Clear(Config_Info& CI);
