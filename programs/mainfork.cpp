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

static double diffx(timespec start, timespec end) {
  timespec temp;

  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp.tv_sec + (double)temp.tv_nsec / 1e9;
}

static inline std::string getVmSize() {
  string vm("");
  string proc_pid_status("/proc/" + to_string(getpid()) + "/status");
  std::string line, tag;
  std::ifstream ifile(proc_pid_status);
  if (ifile.good() && ifile.is_open()) {
    while (std::getline(ifile, line)) {
      std::istringstream iss(line);
      iss >> tag;
      //if ((tag == "VmPeak:") || (tag == "VmSize:") || (tag == "VmHWM:") || (tag == "VmRSS:")) {
      if ((tag == "VmHWM:") || (tag == "VmRSS:")) {
        ssize_t pos = line.find("\t", 0);
        if (pos != std::string::npos) {
          //vm = line.substr(pos + 1, line.length() - pos - 1);
        }
        vm += line;
        vm += " ";
      }
      //cout << "line: " << line << endl;
    }
    ifile.close();
  }
  return vm;
}

#define POC_DEBUG_PRINT 0

clock_t clock_start = 0;
struct timespec cpu_start, cpu_end;
double clock_seconds = 0;
double cpu_seconds = 0;

#define CPU_ELAPSED_BEG() \
  clock_start = clock();  \
  clock_gettime(CLOCK_REALTIME, &cpu_start)

#define CPU_ELAPSED_END(tag)                                      \
  clock_gettime(CLOCK_REALTIME, &cpu_end);                        \
  clock_seconds = diffx(cpu_start, cpu_end);                      \
  cpu_seconds = (double)(clock() - clock_start) / CLOCKS_PER_SEC; \
  cout << tag << " CPU clock_seconds:" << clock_seconds << " cpu_seconds:" << cpu_seconds << endl

int run_stage_one(int argc, char* argv[], int partyid, int loops = 1) {
  UnixClient client(partyid);
  client.init();

  Config_Info CI;
  CI.version = 0; // set 0 in stage1
  //CI.verbose = 2;

  CPU_ELAPSED_BEG();
  run_init(argc, argv, CI);
  CPU_ELAPSED_END("stage1 init");

  BLS bls(Share::SD.M.nplayers(), Share::SD.threshold);

  CPU_ELAPSED_BEG();
  run_poc_setup(bls, CI);
  CPU_ELAPSED_END("stage1 setup");

  run_offline(CI);

  //for (int l = 0; l < loops; l++) {
  for (int l = 0; l < 1; l++) {
    Timer timer;
    timer.start();

    string nonce = "123456";
    vector<bigint> local_bits, reveal_bits;

    CPU_ELAPSED_BEG();
    run_poc_compute_ephem_key_2primes_phase_one(local_bits, reveal_bits, bls, nonce, CI);
    CPU_ELAPSED_END("stage1 phase_one");

    {
      stringstream ss;
      for (int i = 0; i < local_bits.size(); i++) {
        ss << local_bits[i];
      }
      for (int i = 0; i < reveal_bits.size(); i++) {
        ss << reveal_bits[i];
      }

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

  CPU_ELAPSED_BEG();
  run_init(argc, argv, CI);
  CPU_ELAPSED_END("stage2 init");

  run_offline(CI);

  vector<bigint> local_bits, reveal_bits;
  vector<Share> ek(3);

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

    // assign
    stringstream is(s);
    for (int i = 0; i < local_bits.size(); i++) {
      is >> local_bits[i];
    }
    for (int i = 0; i < reveal_bits.size(); i++) {
      is >> reveal_bits[i];
    }

    CPU_ELAPSED_BEG();
    run_poc_compute_ephem_key_2primes_phase_two(ek, local_bits, reveal_bits, CI);
    CPU_ELAPSED_END("stage2 phase_two");
  }

  vector<Share> pre_key;
  CPU_ELAPSED_BEG();
  run_poc_compute_custody_bit_offline_2primes(pre_key, ek, CI);
  CPU_ELAPSED_END("stage2 offline_2primes");

  for (int l = 0; l < loops; l++) {
    cout << "run ...loops:" << l << "/" << loops << endl;
    Timer timer;
    timer.start();

    {
      random_device rd;
      vector<gfp> msg(CHUNK_NUM);
      int tmp;
      for (int i = 0; i < msg.size(); i++) {
        tmp = rd();
        msg[i].assign(tmp);
      }

      CPU_ELAPSED_BEG();
      int bit = run_poc_compute_custody_bit_online_2primes(pre_key, ek[0], msg, CI);
      CPU_ELAPSED_END("stage2 online_2primes");

      cout << "custody bit: " << bit << endl << endl;

      timer.stop();
      cout << "stage2 elapsed:" << timer.elapsed() << endl;
    }
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

  int loops = 5; // how many loops will run
  int ret = -1;
  int status = -1;
  if (fpid == 0) {
    cout << "stage1 pid:" << getpid() << " Begin!" << endl;
    ret = run_stage_one(argc, argv, partyid, loops);
    cout << "stage1 pid:" << getpid() << " End! VmPeak:" << getVmSize() << endl;

  } else {
    cout << "stage2 pid:" << getpid() << " Begin!" << endl;
    ret = run_stage_two(argc, argv, partyid, loops);
    cout << "stage2 pid:" << getpid() << " End! VmPeak:" << getVmSize() << endl;

    // waiting for child exit
    waitpid(fpid, &status, 0);
    printf("status = %d\n", WEXITSTATUS(status));
  }

  return ret;
}
