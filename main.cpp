#include "RunTime/RunPoc.h"

#include <thread>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <condition_variable>
#include <mutex>
#include <atomic>
using namespace std;

int run_once(int argc, char* argv[]) {
  Config_Info CI;
  Run_Init(argc, argv, CI);

  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  Run_PocSetup(bls, CI);
  Run_Offline(CI);

  string nonce = "123456";
  vector<Share> ek(4);
  Run_PocEphemKey(ek, bls, nonce, CI);

  vector<gfp> msg(CHUNK_NUM);
  for (int i = 0; i < msg.size(); i++) {
    msg[i].assign(i + 9);
  }
  int bit = Run_PocGenProof({ek[0], ek[1]}, msg, CI);
  cout << "custody bit: " << bit << endl << endl;

  Wait_ForExit(CI);

  Run_Clear(CI);

  return 0;
}

int run_simulator(int argc, char* argv[], int how_long) {
  Config_Info CI;
  Run_Init(argc, argv, CI);

  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  Run_PocSetup(bls, CI);
  Run_Offline(CI);

  mutex mtx_ek;
  bool is_runing = true;
  atomic<bool> is_runing_ephemkey{false};
  atomic<bool> is_runing_proof{false};

  vector<Share> ek(0);
  auto run_ephemkey = [&]() {
    int l = 0;
    while (is_runing) {
      l++;
      string nonce = "123456-" + std::to_string(l);
      cout << "Run_PocEphemKey counter:" << l << ", nonce:" << nonce << endl;
      vector<Share> ek0(4);
      is_runing_ephemkey = true;
      Run_PocEphemKey(ek0, bls, nonce, CI);
      is_runing_ephemkey = false;
      {
        unique_lock<mutex> lck(mtx_ek);
        ek.assign(ek0.begin(), ek0.end());
      }
      int t = 60 * 3;
      while (is_runing && (t-- > 0))
        sleep(1);
    }
  };
  thread t_run_ephemkey = thread(run_ephemkey);

  auto run_proof = [&]() {
    while (true) {
      if (ek.size() == 4)
        break;
      usleep(200000);
    }
    int l = 0;
    while (is_runing) {
      l++;
      cout << "Run_PocGenProof counter: " << l << endl;
      vector<gfp> msg(CHUNK_NUM);
      for (int i = 0; i < msg.size(); i++) {
        msg[i].assign(i + 9);
      }
      is_runing_proof = true;
      {
        unique_lock<mutex> lck(mtx_ek);
        int bit = Run_PocGenProof({ek[0], ek[1]}, msg, CI);
        cout << "custody bit: " << bit << endl << endl;
      }
      is_runing_proof = false;
      sleep(1);
    }
  };
  thread t_run_proof = thread(run_proof);

  sleep(how_long);
  is_runing = false;

  t_run_ephemkey.join();
  t_run_proof.join();

  Wait_ForExit(CI);

  Run_Clear(CI);

  return 0;
}

int main(int argc, char* argv[]) {
  int ret = 1;
  ret = run_once(argc, argv); // run once
  // sleep(1); // for closing connections completed
  // ret = run_simulator(argc, argv, 60 * 10); // run 60*10 s
  return ret;
}