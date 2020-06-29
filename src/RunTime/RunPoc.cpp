#include "RunPoc.h"

#include "Offline/offline_phases.h"
#include "System/Networking.h"
#include "Tools/Timer.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <pthread.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdio.h>

#ifdef BENCH_MEMORY
#include <sys/resource.h>
#endif
using namespace std;

class thread_info {
 public:
  int thread_num;
  const SystemData* SD;
  offline_control_data* OCD;
  SSL_CTX* ctx;
  Player* player = nullptr;
  int me; // my number
  unsigned int no_online_threads;
  vector<vector<int>> csockets;
  vector<gfp> MacK;

  int verbose;

  Machine* machine; // Pointer to the machine

  ~thread_info() {
    delete player;
    player = nullptr;
  }
};

vector<pthread_t> offline_threads;
vector<thread_info> tinfo;

Timer global_time;

// Forward declarations to make code easier to read
void* Main_Offline_Func(void* ptr);

vector<sacrificed_data> SacrificeD;

/*
[0,3] for offline
4,5,6,7 for PocSetup PocEphemKey PocGenProofPre PocGenProof
*/
enum ThreadPlayer {
  TP_Offline = 3,
  // the following for online phase
  TP_PocSetup,
  TP_PocEphemKey,
  TP_PocGenProofPre,
  TP_PocGenProof,
  // extras here
  TP_NUMS
};

struct Stat2 {
 public:
  string name;
  long data_sent;
  long data_received;
  long pp_messages_sent;
  long pp_messages_recv;
  double elapsed;
  Stat2& set(const Player& P, double e) {
    data_sent = P.data_sent;
    data_received = P.data_received;
    pp_messages_sent = P.pp_messages_sent;
    pp_messages_recv = P.pp_messages_recv;
    elapsed = e;
    return *this;
  }

  string fmt_csv(bool withheader = false) {
    stringstream ss;
    if (withheader) {
      ss << "phase name,elapsed,sent data,recv data,sent msgs,recv msgs" << endl;
      return ss.str();
    }
    ss << name << "," << elapsed << "," << data_sent << "," << data_received << ","
       << pp_messages_sent << "," << pp_messages_recv << endl;
    return ss.str();
  }

  string fmt_console(bool withheader = true, bool withtail = true) {
    stringstream ss;
    if (withheader) {
      // clang-format off
      ss << "+---------------+------------+------------+------------+------------+------------+" << endl;
      ss << "|" << setw(15) << "phase name " << "|" << setw(12) << "elapsed "
         << "|" << setw(12) << "sent data "  << "|" << setw(12) << "recv data "
         << "|" << setw(12) << "sent msgs "  << "|" << setw(12) << "recv msgs "
         << "|" << endl;
      ss << "+---------------+------------+------------+------------+------------+------------+" << endl;
      // clang-format on
    }
    // clang-format off
      ss << "|" << setw(14) << name      << " |" << setw(11) << elapsed       << " "
         << "|" << setw(11) << data_sent << " |" << setw(11) << data_received << " "
         << "|" << setw(11) << pp_messages_sent << " |" << setw(11) << pp_messages_recv << " "
         << "|" << endl;
    // clang-format on
    if (withtail) {
      // clang-format off
      ss << "+---------------+------------+------------+------------+------------+------------+" << endl;
      // clang-format on
    }
    return ss.str();
  }

  string to_string() {
    stringstream ss;
    // clang-format off
    ss << setprecision(15)
       << endl << "[phase_stat] ==================================="
       << endl << "[phase_stat]     phase: " << name 
       << endl << "[phase_stat]   elapsed: " << elapsed
       << endl << "[phase_stat] sent data: " << data_sent 
       << endl << "[phase_stat] recv data: " << data_received
       << endl << "[phase_stat] sent msgs: " << pp_messages_sent
       << endl << "[phase_stat] recv msgs: " << pp_messages_recv
       << endl << "[phase_stat] ==================================="
       << endl;
    // clang-format on
    return ss.str();
  }

  void print(int verbose = 1) {
    if (verbose > 0)
      cout << to_string();
  }
};
vector<Stat2> stats; // stats.size() == TP_NUMS
void init_stats_info() {
  stats.resize(ThreadPlayer::TP_NUMS); // == ThreadPlayer::TP_NUMS
  {
    for (int i = 0; i < ThreadPlayer::TP_NUMS; i++) {
      string name("Unknow");
      switch (i) {
        case 0: name = "mult_phase"; break;
        case 1: name = "square_phase"; break;
        case 2: name = "bit_phase"; break;
        case 3: name = "inputs_phase"; break;
        case TP_PocSetup: name = "setup"; break;
        case TP_PocEphemKey: name = "ephem_key"; break;
        case TP_PocGenProofPre: name = "gen_proof_pre"; break;
        case TP_PocGenProof: name = "gen_proof"; break;
        default: break;
      }
      stats[i].name = name;
    }
  }
}

void init_thread_info(Config_Info& CI, vector<thread_info>& tinfo) {
  unsigned int tnthreads = CI.tnthreads;
  tinfo.resize(tnthreads);
  vector<gfp> MacK(0);
  for (unsigned int i = 0; i < tnthreads; i++) {
    tinfo[i].thread_num = i;
    tinfo[i].SD = &CI.SD;
    tinfo[i].OCD = &CI.OCD;
    tinfo[i].ctx = CI.ctx;
    tinfo[i].MacK = MacK;
    tinfo[i].me = CI.my_number;
    tinfo[i].no_online_threads = CI.no_online_threads;
    tinfo[i].csockets = CI.csockets[i];
    tinfo[i].machine = &CI.machine;
    tinfo[i].verbose = CI.verbose;
    tinfo[i].player = new Player(CI.my_number, CI.SD, i, CI.ctx, CI.csockets[i], MacK, CI.verbose);
  }
}

void run_init(int argc, char* argv[], Config_Info& CI) {
  if (argc != 2) {
    cerr << "ERROR: incorrect number of arguments to Player.x\n";
    cerr << argv[0] << " <party-id>" << endl;
  } else {
    CI.my_number = (unsigned int)atoi(argv[1]);
  }

  string memtype = "empty";
  unsigned int portnumbase = 20000;

  if (CI.version == 0) {
    portnumbase = 20000;
  } else if (CI.version == 1) {
    portnumbase = 30000;
  }
  CI.verbose = -1;

  /*************************************
   *  Setup offline_control_data OCD   *
   *************************************/
  //  offline_control_data OCD;
  CI.OCD.minm = 0;
  CI.OCD.mins = 0;
  CI.OCD.minb = 0;
  CI.OCD.maxm = 0; //3000; //set for optimized performance
  CI.OCD.maxs = 0; //1000;
  CI.OCD.maxb = 0;
  CI.OCD.maxI = 0;

  cout << "(min, max) number of triples: (" << CI.OCD.minm << ", " << CI.OCD.maxm << ")" << endl;
  cout << "(min, max) number of squares: (" << CI.OCD.mins << ", " << CI.OCD.maxs << ")" << endl;
  cout << "(min, max) number of    bits: (" << CI.OCD.minb << ", " << CI.OCD.maxb << ")" << endl;

  /*************************************
   *     Initialise the system data    *
   *************************************/
  CI.SD = SystemData("Data/NetworkData.txt");

  if (CI.my_number >= CI.SD.n) {
    throw data_mismatch();
  }

  /*************************************
   *    Initialize the portnums        *
   *************************************/
  CI.portnum.resize(CI.SD.n);

  for (unsigned int i = 0; i < CI.SD.n; i++) {
    CI.portnum[i] = portnumbase + i;
  }

  /*************************************
   * Initialise the secret sharing     *
   * data and the gfp field data       *
   *************************************/
  string dsd("Data/SharingData.txt");
  if (CI.version == 0) {
  } else if (CI.version == 1) {
    dsd = "Data/SharingData1.txt";
  }
  ifstream inp(dsd);
  if (inp.fail()) {
    throw file_error("Data/SharingData.txt");
  }
  bigint p;
  inp >> p;
  //cout << "\n\np=" << p << endl;
  gfp::init_field(p);
  ShareData ShD;
  inp >> ShD;
  inp.close();
  if (ShD.M.nplayers() != CI.SD.n) {
    throw data_mismatch();
  }
  if (CI.SD.fake_offline == 1) {
    ShD.Otype = Fake;
  }
  Share::init_share_data(ShD);

  /* Initialize SSL */
  Init_SSL_CTX(CI.ctx, CI.SD.PlayerCRT[CI.my_number], CI.SD.RootCRT);

  /* Initialize the machine */
  //  Machine machine;
  if (CI.verbose < 0) {
    CI.machine.set_verbose();
    CI.verbose = 0;
  }
  CI.machine.SetUp_Memory(CI.my_number, memtype);

  // Here you configure the IO in the machine
  //  - This depends on what IO machinary you are using
  //  - Here we are just using the simple IO class
  unique_ptr<Input_Output_Simple> io(new Input_Output_Simple);
  io->init(cin, cout, true);
  CI.machine.Setup_IO(std::move(io));

  // Load the initial tapes for the first program into the schedule
  //  unsigned int no_online_threads = 1;
  CI.no_online_threads = 1;

  CI.machine.SetUp_Threads(CI.no_online_threads);
  CI.OCD.resize(CI.no_online_threads, CI.SD.n, CI.my_number);

  SacrificeD.resize(CI.no_online_threads);
  for (unsigned int i = 0; i < CI.no_online_threads; i++) {
    SacrificeD[i].initialize(CI.SD.n);
  }

  /* Initialize the networking TCP sockets */
  CI.tnthreads = ThreadPlayer::TP_NUMS;
  CI.csockets =
    vector<vector<vector<int>>>(CI.tnthreads, vector<vector<int>>(CI.SD.n, vector<int>(NSSL)));
  Get_Connections(CI.ssocket, CI.csockets, CI.portnum, CI.my_number, CI.SD, 1);
  printf("All connections now done\n");

  init_thread_info(CI, tinfo);
  init_stats_info();
  global_time.start();
  global_time.reset();
}

void run_clear0(Config_Info& CI) { tinfo.clear(); }

void run_clear(Config_Info& CI) {
  //  cout << "----------Begin of Clear----------------------------" << endl;
  run_clear0(CI);
  CI.machine.Dump_Memory(CI.my_number);
  Close_Connections(CI.ssocket, CI.csockets, CI.my_number);
  Destroy_SSL_CTX(CI.ctx);
  global_time.stop();
  usleep(500000);
  cout << endl
       << "Total Time (with thread locking) = " << global_time.elapsed() << " seconds" << endl
       << endl;
  //  cout << "----------End of Clear-------------------------------" << endl;
}

void run_poc_setup(BLS& bls, Config_Info& CI) {
  cout << "----------Begin of setup-----------------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocSetup].player);

  Timer timer;
  timer.start();
  poc_setup(bls, P);
  timer.stop();
  stats[ThreadPlayer::TP_PocSetup].set(P, timer.elapsed()).print(CI.verbose);

  if (CI.verbose > 0) {
    cout << "secre key share: " << endl;
    print_mclBnFr(bls.get_sk());

    cout << "public key: " << endl;
    print_mclBnG1(bls.vk);
  }
  cout << "----------End of setup-------------------------------" << endl;
}

void run_poc_compute_ephem_key(vector<Share>& ek, BLS& bls, const string& msg, Config_Info& CI) {
  cout << "----------Begin of compute_enphem_key----------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);

  Timer timer;
  timer.start();

  G2_Affine_Coordinates ac;
  poc_compute_ephem_key(ac, bls, msg, 0, P, CI);
  ek.resize(4);
  ek[0] = ac.x.real;
  ek[1] = ac.x.imag;
  ek[2] = ac.y.real;
  ek[3] = ac.y.imag;

  timer.stop();
  stats[ThreadPlayer::TP_PocEphemKey].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_enphem_key------------" << endl;
}

void run_poc_compute_custody_bit_offline(
  vector<Share>& pre_key, const vector<Share>& keys, Config_Info& CI) {
  cout << "----------Begin of compute_custody_bit_offline-------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocGenProofPre].player);

  Timer timer;
  timer.start();
  poc_compute_custody_bit_offline(pre_key, keys, 0, P, CI);
  timer.stop();
  stats[ThreadPlayer::TP_PocGenProofPre].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_offline-------------------" << endl;
}

int run_poc_compute_custody_bit_online(
  const vector<Share>& pre_key, const vector<gfp>& msg, Config_Info& CI) {
  cout << "----------Begin of compute_custody_bit_online-------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  Timer timer;
  timer.start();
  int res = poc_compute_custody_bit_online(pre_key, msg, 0, P, CI);
  timer.stop();
  stats[ThreadPlayer::TP_PocGenProof].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_online-------------------" << endl;

  return res;
}

int run_poc_compute_custody_bit(
  const vector<Share>& keys, const vector<gfp>& msg, Config_Info& CI) {
  cout << "----------Begin of compute_custody_bit-------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  Timer timer;
  timer.start();
  int res = poc_compute_custody_bit(keys, msg, 0, P, CI);
  timer.stop();
  stats[ThreadPlayer::TP_PocGenProof].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit---------------------" << endl;
  return res;
}

//======================
//======================
// the 2 primes version

void run_poc_compute_ephem_key_2primes_phase_one(
  vector<bigint>& local_bits, vector<bigint>& reveal_bits, BLS& bls, const string& msg,
  Config_Info& CI) {
  cout << "----------Begin of compute_enphem_key in 2 primes version phase_one----------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);

  Timer timer;
  timer.start();

  G2_Affine_Coordinates ac;
  poc_compute_ephem_key(ac, bls, msg, 0, P, CI);
  vector<Share> ek_tmp(2);
  ek_tmp[0] = ac.x.real;
  ek_tmp[1] = ac.x.imag;

  vector<Share> shared_bits;
  shared_rand_bits_phase_one(shared_bits, local_bits, 0, P, CI);
  decompose_and_reveal(reveal_bits, ek_tmp, shared_bits, 0, P, CI);

  timer.stop();
  stats[ThreadPlayer::TP_PocEphemKey].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_enphem_key in 2 primes version phase_one------------" << endl;
}

void run_poc_compute_ephem_key_2primes_phase_two(
  vector<Share>& ek, const vector<bigint>& local_bits, const vector<bigint>& reveal_bits,
  Config_Info& CI) {
  cout << "----------Begin of compute_enphem_key in 2 primes version phase_two----------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);
  Timer timer;
  timer.start();

  vector<Share> shared_bits;
  shared_rand_bits_phase_two(shared_bits, local_bits, 0, P, CI);
  xor_and_combine(ek, shared_bits, reveal_bits, 0, P, CI);

  timer.stop();
  stats[ThreadPlayer::TP_PocEphemKey].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_enphem_key in 2 primes version phase_two------------" << endl;
}

void run_poc_compute_custody_bit_offline_2primes(
  vector<Share>& pre_key, const vector<Share>& keys, Config_Info& CI) {
  //  run_poc_compute_custody_bit_offline(pre_key, keys, CI);
  cout << "----------Begin of compute_custody_bit_offline_2primes-------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocGenProofPre].player);

  Timer timer;
  timer.start();
  poc_compute_custody_bit_offline_2primes(pre_key, keys, 0, P, CI);
  timer.stop();
  stats[ThreadPlayer::TP_PocGenProofPre].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_offline_2primes-------------------" << endl;
}

int run_poc_compute_custody_bit_online_2primes(
  const vector<Share>& pre_key, const Share& key, const vector<gfp>& msg, Config_Info& CI) {
  cout << "----------Begin of compute_custody_bit_online_2primes-------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  Timer timer;
  timer.start();
  int res = poc_compute_custody_bit_online_2primes(pre_key, key, msg, 0, P, CI);
  timer.stop();
  stats[ThreadPlayer::TP_PocGenProof].set(P, timer.elapsed()).print(CI.verbose);

  cout << "----------End of compute_custody_bit_online_primes-------------------" << endl;

  return res;
}

void* Main_Offline_Func(void* ptr) {
  thread_info* tinfo = (thread_info*)ptr;
  unsigned int num = tinfo->thread_num;
  int me = tinfo->me;
  int verbose = tinfo->verbose;
  fflush(stdout);

  Timer timer;
  timer.start();
  offline_phase(num, 0, *(tinfo->player), (tinfo->SD)->fake_sacrifice, *(tinfo->OCD), verbose);
  timer.stop();
  stats[num].set(*(tinfo->player), timer.elapsed()).print(verbose);

  return NULL;
}

void run_offline(Config_Info& CI) {
  printf("Setting up offline phase threads\n");

  // offline phase, 4 threads. id in [0,3], use CI.csockets[0~3]
  unsigned int offline_thread_nums = ThreadPlayer::TP_Offline + 1;
  offline_threads.resize(offline_thread_nums);
  for (unsigned int i = 0; i < offline_thread_nums; i++) {
    if (pthread_create(&offline_threads[i], NULL, Main_Offline_Func, &tinfo[i])) {
      throw C_problem("Problem spawning thread");
    }
  }
}

void wait_for_exit(Config_Info& CI) {
  // set offline & online finished
  CI.OCD.OCD_mutex[0].lock();
  CI.OCD.finish_offline[0] = 1;
  CI.OCD.finished_online[0] = 1;
  CI.OCD.OCD_mutex[0].unlock();

  printf("Waiting for all offline clients to finish\n");
  fflush(stdout);
  for (unsigned int i = 0; i < offline_threads.size(); i++) {
    pthread_join(offline_threads[i], NULL);
  }

  long long total_triples = 0, total_squares = 0, total_bits = 0, total_inputs = 0;
  for (size_t i = 0; i < CI.no_online_threads; i++) {
    total_triples += CI.OCD.totm[i];
    total_squares += CI.OCD.tots[i];
    total_bits += CI.OCD.totb[i];
    total_inputs += CI.OCD.totI[i];
  }

  cout << "Produced a total of " << total_triples << " triples" << endl;
  cout << "Produced a total of " << total_squares << " squares" << endl;
  cout << "Produced a total of " << total_bits << " bits" << endl;
  cout << "Produced a total of " << total_inputs << " inputs" << endl;
}

void output_statistics(Config_Info& CI) {
  {
    cout << "format 1:" << endl;
    for (int i = 0; i < CI.tnthreads; i++) {
      if (i == 0) {
        cout << stats[i].fmt_console(true, false);
      } else if (i < CI.tnthreads - 1) {
        cout << stats[i].fmt_console(false, false);
      } else {
        cout << stats[i].fmt_console(false, true);
      }
    }
  }
  if (false) {
    cout << "format 2:" << endl;
    for (int i = 0; i < CI.tnthreads; i++) {
      if (i == 0) {
        cout << stats[i].fmt_console(true, true);
      } else {
        cout << stats[i].fmt_console(false, true);
      }
    }
  }
  {
    cout << "format 3: See " << CI.my_number << ".csv" << endl;
    string filename = "out/" + to_string(CI.my_number) + ".csv";
    ofstream ofile(filename, ios::out | ios::app);
    if (!ofile.is_open()) {
      return;
    }
    ofile.seekp(0, ios::end);
    if (ofile.tellp() == 0) {
      // write header
      ofile << "now,n,t," << stats[0].fmt_csv(true);
    }
    time_t now = time(0);
    for (int i = 0; i < CI.tnthreads; i++) {
      ofile << now << "," << CI.SD.n << "," << CI.SD.t << "," << stats[i].fmt_csv(false);
    }

    ofile.close();
  }
}

void run_test_bit_ops(Config_Info& CI) {
  cout << "----------Begin of test-----------------------------" << endl;
  Player& P = *(tinfo[ThreadPlayer::TP_PocSetup].player);

  Processor Proc(0, P.nplayers(), P);

  OnlineOp online_op(Proc, 0, P, CI.OCD, CI.machine);

  online_op.test_bit_ops();
  cout << "----------End of test-------------------------------" << endl;
}
