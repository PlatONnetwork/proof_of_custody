#pragma once

#include "Offline/offline_data.h"
#include "Online/Machine.h"
#include "System/SystemData.h"
#include "poc.h"

#include <openssl/ssl.h>

class Config_Info
{
public:
    unsigned int my_number;
    unsigned int no_online_threads;
    SSL_CTX *ctx;
    vector<unsigned int> portnum;
    SystemData SD;
    Machine machine;
    offline_control_data OCD;
    int verbose;
    Config_Info() {}
};

/*
* Initate the configure information
*/
void Init(int argc, char *argv[], Config_Info &CI);

/*
* run proof of custody with mpc
*/
void Run_Poc(BLS &bls, Config_Info &CI);