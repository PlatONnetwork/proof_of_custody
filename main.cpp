#include "RunTime/RunPoc.h"

int main(int argc, char *argv[])
{
    Config_Info CI;
    Init(argc, argv, CI);

    cout << "Begin of prog\n";
//    cout<<Share::SD.threshold<<endl;
//    cout<<Share::SD.M.nplayers()<<endl;
    BLS bls(Share::SD.M.nplayers(),Share::SD.threshold);
    Run_Poc(bls, CI);
    print_mclBnFr(bls.get_sk());
    print_mclBnG1(bls.vk);

    CI.machine.Dump_Memory(CI.my_number);
    Destroy_SSL_CTX(CI.ctx);

    cout << "End of prog\n";
    fflush(stdout);
}