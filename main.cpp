#include "RunTime/RunPoc.h"

int main(int argc, char* argv[]) {
  Config_Info CI;
  Run_Init(argc, argv, CI);

  cout << "Begin of prog\n";
  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  Run_PocSetup(bls, CI);

  Run_Offline(CI);

  string nonce = "123456";
  vector<Share> ek(4);
  //Run_PocEphemKey(ek, bls, nonce, CI);

  Run_Online(CI);

  Wait_ForExit(CI);

  Run_Clear(CI);

  cout << "End of prog\n";
  return 0;
}