#include "RunTime/RunPoc.h"

int main(int argc, char *argv[])
{
    Config_Info CI;
    Run_Init(argc, argv, CI);

    cout << "Begin of prog\n";
//    cout<<Share::SD.threshold<<endl;
//    cout<<Share::SD.M.nplayers()<<endl;
    BLS bls(Share::SD.M.nplayers(),Share::SD.threshold);
    Run_Poc(bls, CI);
 //   Run_PocSetup(bls, CI);


    string nonce = "123456";
    vector<Share> ek(4);
//    Run_PocEphemKey(ek,bls,nonce,CI);

    Run_Clear(CI);
//    CI.machine.Dump_Memory(CI.my_number);
//    Destroy_SSL_CTX(CI.ctx);

    cout << "End of prog\n";
    fflush(stdout);
}