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
using namespace std;

#define POC_DEBUG_PRINT 0

int run_stage1(int argc, char* argv[], int partyid, int loops = 1) {
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
    vector<Share> ek(4);
    run_poc_compute_enphem_key(ek, bls, nonce, CI);

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
      for (int i = 0; i < ek.size(); i++) {
        ek[i].output_bigint(ss);
      }
#if POC_DEBUG_PRINT
      {
        cout << "client:" << ss.str().length() << endl;
        cout << "client:" << ss.str() << endl;
      }
#endif

      string s(ss.str());
      int len = s.length();
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

int run_stage2(int argc, char* argv[], int partyid, int loops = 1) {
  UnixServer server(partyid);
  server.init();

  Timer timer;
  timer.start();

  Config_Info CI;
  CI.version = 1; // set 0 in stage2

  run_init(argc, argv, CI);
  run_offline(CI);

  for (int l = 0; l < loops; l++) {
    Timer timer;
    timer.start();
    vector<Share> ek(4);
    {
      // recv
      int len = 0;
      server.recv((char*)&len, 4);
      char* buf = new char[len + 1];
      server.recv(buf, len);
      string s(buf, len);

#if POC_DEBUG_PRINT
      cout << s << endl;
#endif
      // assign
      stringstream is(s);
      for (int i = 0; i < ek.size(); i++) {
        //ek[i].input(is, true);
        ek[i].input_bigint(is);
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
    cerr << "ERROR: incorrect number of arguments to Player.x\n";
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
    ret = run_stage1(argc, argv, partyid, loops);
  } else {
    ret = run_stage2(argc, argv, partyid, loops);

    // waiting for child exit
    waitpid(fpid, &status, 0);
    printf("status = %d\n", WEXITSTATUS(status));
  }
  cout << "pid:" << getpid() << " Done!" << endl;

  return ret;
}
