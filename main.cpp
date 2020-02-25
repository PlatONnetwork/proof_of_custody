#include "RunTime/RunPoc.h"

int main(int argc, char *argv[])
{
  Config_Info CI;
  Run_Init(argc, argv, CI);

  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  Run_PocSetup(bls, CI);

  Run_Offline(CI);

  string nonce = "123456";
  vector<Share> ek(4);

  vector<gfp> msg(CHUNK_NUM);
  for (int i = 0; i < msg.size(); i++)
  {
    msg[i].assign(i + 9);
  }

  Run_PocEphemKey(ek, bls, nonce, CI);

  int bit = Run_PocGenProof({ek[0], ek[1]}, msg, CI);
  //  Run_Online(CI);
  cout << "custody bit: " << bit << endl
       << endl;

  Wait_ForExit(CI);

  Run_Clear(CI);

  return 0;
}