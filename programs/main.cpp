#include "RunTime/RunPoc.h"
#include "Math/Lagrange.h"

#include <thread>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <condition_variable>
#include <mutex>
#include <atomic>
using namespace std;

int run_test(int argc, char* argv[]) {
  Config_Info CI;
  run_init(argc, argv, CI);
  run_offline(CI);
  run_test_bit_ops(CI);

  wait_for_exit(CI);
//  output_statistics(CI);

  run_clear(CI);
}

int run_once(int argc, char* argv[]) {
  Config_Info CI;
  run_init(argc, argv, CI);

  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  run_poc_setup(bls, CI);
  run_offline(CI);

  string nonce = "123456";
  vector<Share> ek(4);
  run_poc_compute_ephem_key(ek, bls, nonce, CI);

  vector<gfp> msg(CHUNK_NUM);
  for (int i = 0; i < msg.size(); i++) {
    msg[i].assign(i + 9);
  }

  vector<Share> pre_key;

#if 1 // set 0/1 to switch
  run_poc_compute_custody_bit_offline(pre_key, {ek[0], ek[1]}, CI);
  int bit = run_poc_compute_custody_bit_online(pre_key, msg, CI);
#else
  int bit = run_poc_compute_custody_bit({ek[0], ek[1]}, msg, CI);
#endif

  cout << "custody bit: " << bit << endl << endl;

  wait_for_exit(CI);
  output_statistics(CI);

  run_clear(CI);

  return 0;
}

int run_simulator(int argc, char* argv[], int how_long) {
  Config_Info CI;
  run_init(argc, argv, CI);

  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  run_poc_setup(bls, CI);
  run_offline(CI);

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
      run_poc_compute_ephem_key(ek0, bls, nonce, CI);
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
        int bit = run_poc_compute_custody_bit({ek[0], ek[1]}, msg, CI);
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

  wait_for_exit(CI);

  run_clear(CI);

  return 0;
}

int main(int argc, char* argv[]) {
  int ret = 1;
  //ret = run_test(argc, argv);
    ret = run_once(argc, argv); // run once
  //  ret = run_simulator(argc, argv, 60 * 10); // run 60*10 s
  return ret;
}