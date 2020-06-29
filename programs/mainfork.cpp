#include "RunTime/RunPoc.h"
#include "Math/Lagrange.h"

#include "ud_socket.h"

#include <thread>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <random>
using namespace std;

#define POC_DEBUG_PRINT 0

int run_stage_one(int argc, char* argv[], int partyid, int loops = 1) {
  UnixClient client(partyid);
  client.init();

  Config_Info CI;
  CI.version = 0; // set 0 in stage1

  run_init(argc, argv, CI);
  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  run_poc_setup(bls, CI);
  run_offline(CI);

  for (int l = 0; l < loops; l++) {
    Timer timer;
    timer.start();

    string nonce = "123456";
    vector<bigint> local_bits, reveal_bits;
    run_poc_compute_ephem_key_2primes_phase_one(local_bits, reveal_bits, bls, nonce, CI);

    {
#if POC_DEBUG_PRINT
      {
        stringstream ss;
        for (int i = 0; i < ek.size(); i++) {
          ek[i].output(ss, true);
        }
        if (true) {
          // debug
          cout << "9client:" << ss.str().length() << endl;
          cout << "9client:" << ss.str() << endl;
        }
      }
#endif
      stringstream ss;
      for (int i = 0; i < local_bits.size(); i++) {
        ss << local_bits[i];
      }
      for (int i = 0; i < reveal_bits.size(); i++) {
        ss << reveal_bits[i];
      }
#if POC_DEBUG_PRINT
      {
        cout << "client:" << ss.str().length() << endl;
        cout << "client:" << ss.str() << endl;
      }
#endif

      int size = local_bits.size();
      string s(ss.str());
      int len = s.length();
      client.send((const char*)&size, 4);
      client.send((const char*)&len, 4);
      client.send(s.data(), len);
    }

    timer.stop();
    cout << "stage1 elapsed:" << timer.elapsed() << endl;
  }

  wait_for_exit(CI);
  output_statistics(CI);

  run_clear(CI);

  client.uninit();
  return 0;
}
int run_stage_two(int argc, char* argv[], int partyid, int loops = 1) {
  UnixServer server(partyid);
  server.init();

  Timer timer;
  timer.start();

  Config_Info CI;
  CI.version = 1; // set 0 in stage2
  CI.verbose = 2;

  run_init(argc, argv, CI);
  run_offline(CI);

  vector<bigint> local_bits, reveal_bits;

  for (int l = 0; l < loops; l++) {
    Timer timer;
    timer.start();
    {
      // recv
      int size = 0;
      server.recv((char*)&size, 4);
      int len = 0;
      server.recv((char*)&len, 4);
      char* buf = new char[len + 1];
      server.recv(buf, len);
      string s(buf, len);

      local_bits.resize(size);
      reveal_bits.resize(size);

#if POC_DEBUG_PRINT
      cout << s << endl;
#endif
      // assign
      stringstream is(s);
      for (int i = 0; i < local_bits.size(); i++) {
        is >> local_bits[i];
      }
      for (int i = 0; i < reveal_bits.size(); i++) {
        is >> reveal_bits[i];
      }
#if POC_DEBUG_PRINT
      {
        // debug
        stringstream ss;
        for (int i = 0; i < ek.size(); i++) {
          ek[i].output(ss, true);
        }
        cout << "server:" << ss.str().length() << endl;
        cout << "server:" << ss.str() << endl;
      }
#endif
    }

    vector<Share> ek(3);
    run_poc_compute_ephem_key_2primes_phase_two(ek, local_bits, reveal_bits, CI);

    random_device rd;
    vector<gfp> msg(CHUNK_NUM);
    int tmp;
    for (int i = 0; i < msg.size(); i++) {
      tmp = rd();
      msg[i].assign(tmp);
    }

    vector<Share> pre_key;

#if 1 // set 0/1 to switch
    run_poc_compute_custody_bit_offline_2primes(pre_key, ek, CI);
    int bit = run_poc_compute_custody_bit_online_2primes(pre_key, ek[0], msg, CI);
#else
    int bit = run_poc_compute_custody_bit({ek[0], ek[1]}, msg, CI);
#endif

    cout << "custody bit: " << bit << endl << endl;

    timer.stop();
    cout << "stage2 elapsed:" << timer.elapsed() << endl;
  }

  wait_for_exit(CI);
  output_statistics(CI);

  run_clear(CI);

  server.uninit();

  return 0;
}
int main(int argc, char* argv[]) {
  // parse party id
  if (argc < 2) {
    cerr << "error: incorrect number of arguments to Player.x\n";
    cerr << argv[0] << " <party-id>" << endl;
    exit(0);
  }
  int partyid = (unsigned int)atoi(argv[1]);

  pid_t fpid = fork();
  if (fpid < 0) {
    cerr << "error in fork!" << endl;
    exit(1);
  }

  int loops = 1; // how many loops will run
  int ret = -1;
  int status = -1;
  if (fpid == 0) {
    cout << "stage1 pid:" << getpid() << " Begin!" << endl;
    ret = run_stage_one(argc, argv, partyid, loops);
  } else {
    cout << "stage2 pid:" << getpid() << " Begin!" << endl;
    ret = run_stage_two(argc, argv, partyid, loops);

    // waiting for child exit
    waitpid(fpid, &status, 0);
    printf("status = %d\n", WEXITSTATUS(status));
  }
  cout << "pid:" << getpid() << " Done!" << endl;

  return ret;
}
