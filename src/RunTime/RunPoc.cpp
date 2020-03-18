#include "RunPoc.h"

#include "Offline/offline_phases.h"
#include "Online/Online.h"
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

class thread_info
{
public:
  int thread_num;
  const SystemData *SD;
  offline_control_data *OCD;
  SSL_CTX *ctx;
  Player *player = nullptr;
  int me; // my number
  unsigned int no_online_threads;
  vector<vector<int>> csockets;
  vector<gfp> MacK;

  int verbose;

  Machine *machine; // Pointer to the machine

  ~thread_info()
  {
    delete player;
    player = nullptr;
  }
};

vector<pthread_t> offline_threads;
vector<thread_info> tinfo;

Timer global_time;
Timer offline_time;

// Forward declarations to make code easier to read
void *Main_Offline_Func(void *ptr);

vector<sacrificed_data> SacrificeD;

/*
[0,3] for offline
4,5,6,7 for PocSetup PocEphemKey PocGenProofPre PocGenProof
*/
enum ThreadPlayer
{
  TP_Offline = 3,
  // the following for online phase
  TP_PocSetup,
  TP_PocEphemKey,
  TP_PocGenProofPre,
  TP_PocGenProof,
  // extras here
  TP_PocExtraOnline,
  TP_NUMS
};

void init_thread_info(Config_Info &CI, vector<thread_info> &tinfo)
{
  unsigned int tnthreads = CI.tnthreads;
  tinfo.resize(tnthreads);
  vector<gfp> MacK(0);
  for (unsigned int i = 0; i < tnthreads; i++)
  {
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

void run_init(int argc, char *argv[], Config_Info &CI)
{
  if (argc != 2)
  {
    cerr << "ERROR: incorrect number of arguments to Player.x\n";
  }
  else
  {
    CI.my_number = (unsigned int)atoi(argv[1]);
  }

  string memtype = "empty";
  unsigned int portnumbase = 5000;
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

  if (CI.my_number >= CI.SD.n)
  {
    throw data_mismatch();
  }

  /*************************************
   *    Initialize the portnums        *
   *************************************/
  CI.portnum.resize(CI.SD.n);

  for (unsigned int i = 0; i < CI.SD.n; i++)
  {
    CI.portnum[i] = portnumbase + i;
  }

  /*************************************
   * Initialise the secret sharing     *
   * data and the gfp field data       *
   *************************************/
  ifstream inp("Data/SharingData.txt");
  if (inp.fail())
  {
    throw file_error("Data/SharingData.txt");
  }
  bigint p;
  inp >> p;
  //cout << "\n\np=" << p << endl;
  gfp::init_field(p);
  ShareData ShD;
  inp >> ShD;
  inp.close();
  if (ShD.M.nplayers() != CI.SD.n)
  {
    throw data_mismatch();
  }
  if (CI.SD.fake_offline == 1)
  {
    ShD.Otype = Fake;
  }
  Share::init_share_data(ShD);

  /* Initialize SSL */
  Init_SSL_CTX(CI.ctx, CI.SD.PlayerCRT[CI.my_number], CI.SD.RootCRT);

  /* Initialize the machine */
  //  Machine machine;
  if (CI.verbose < 0)
  {
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
  for (unsigned int i = 0; i < CI.no_online_threads; i++)
  {
    SacrificeD[i].initialize(CI.SD.n);
  }

  /* Initialize the networking TCP sockets */
  CI.tnthreads = ThreadPlayer::TP_NUMS;
  CI.csockets =
      vector<vector<vector<int>>>(CI.tnthreads, vector<vector<int>>(CI.SD.n, vector<int>(3)));
  Get_Connections(CI.ssocket, CI.csockets, CI.portnum, CI.my_number, CI.SD, 1);
  printf("All connections now done\n");

  init_thread_info(CI, tinfo);
  global_time.start();
}

void run_clear(Config_Info &CI)
{
  //  cout << "----------Begin of Clear----------------------------" << endl;
  tinfo.clear();
  CI.machine.Dump_Memory(CI.my_number);
  Close_Connections(CI.ssocket, CI.csockets, CI.my_number);
  Destroy_SSL_CTX(CI.ctx);
  global_time.stop();
  cout << endl
       << "Total Time (with thread locking) = " << global_time.elapsed() << " seconds" << endl
       << endl;
  //  cout << "----------End of Clear-------------------------------" << endl;
}

void run_poc_setup(BLS &bls, Config_Info &CI)
{
  cout << "----------Begin of setup-----------------------------" << endl;
  Timer setup_time;
  setup_time.start();
  Player &P = *(tinfo[ThreadPlayer::TP_PocSetup].player);

  poc_setup(bls, P);

  setup_time.stop();

  cout << "secre key share: " << endl;
  print_mclBnFr(bls.get_sk());

  cout << "public key: " << endl;
  print_mclBnG1(bls.vk);

  cout << "sent data: " << P.data_sent << " Bytes" << endl;
  cout << "recv data: " << P.data_received << " Bytes" << endl;
  cout << "sent msgs: " << P.pp_messages_sent << endl;
  cout << "recv msgs: " << P.pp_messages_recv << endl
       << endl;

  cout << "setup time: " << setup_time.elapsed() << " seconds" << endl
       << endl;

  cout << "----------End of setup-------------------------------" << endl;
}

void run_poc_compute_enphem_key(vector<Share> &ek, BLS bls, const string msg, Config_Info &CI)
{
  cout << "----------Begin of compute_enphem_key----------" << endl;
  Timer ek_time;
  ek_time.start();
  Player &P = *(tinfo[ThreadPlayer::TP_PocEphemKey].player);

  G2_Affine_Coordinates ac;
  poc_compute_enphem_key(ac, bls, msg, 0, P, CI);
  ek.resize(4);
  ek[0] = ac.x.real;
  ek[1] = ac.x.imag;
  ek[2] = ac.y.real;
  ek[3] = ac.y.imag;

  ek_time.stop();

  cout << "sent data: " << P.data_sent << " Bytes" << endl;
  cout << "recv data: " << P.data_received << " Bytes" << endl;
  cout << "sent msgs: " << P.pp_messages_sent << endl;
  cout << "recv msgs: " << P.pp_messages_recv << endl;

  cout << endl
       << "compute_enphem_key time: " << ek_time.elapsed() << " seconds" << endl
       << endl;
  cout << "----------End of compute_enphem_key------------" << endl;
}

void run_poc_compute_custody_bit_offline(vector<Share> &pre_key,
                                         const vector<Share> &keys,
                                         Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit_offline-------------------" << endl;
  Timer custody_offline_time;
  custody_offline_time.start();

  Player &P = *(tinfo[ThreadPlayer::TP_PocGenProofPre].player);

  poc_compute_custody_bit_offline(pre_key, keys, 0, P, CI);
  
  custody_offline_time.stop();

  cout << "sent data: " << P.data_sent << " Bytes" << endl;
  cout << "recv data: " << P.data_received << " Bytes" << endl;
  cout << "sent msgs: " << P.pp_messages_sent << endl;
  cout << "recv msgs: " << P.pp_messages_recv << endl;

  cout << endl
       << "compute_custody_bit_offline time " << custody_offline_time.elapsed() << " seconds" << endl
       << endl;

  cout << "----------End of compute_custody_bit_offline-------------------" << endl;
}

int run_poc_compute_custody_bit_online(const vector<Share> &pre_key,
                                       const vector<gfp> &msg,
                                       Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit_online-------------------" << endl;
  Timer custody_online_time;
  custody_online_time.start();

  Player &P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  int res = poc_compute_custody_bit_online(pre_key,msg,0,P,CI);

  custody_online_time.stop();

  cout << "sent data: " << P.data_sent << " Bytes" << endl;
  cout << "recv data: " << P.data_received << " Bytes" << endl;
  cout << "sent msgs: " << P.pp_messages_sent << endl;
  cout << "recv msgs: " << P.pp_messages_recv << endl;

  cout << endl
       << "compute_custody_bit_online time " << custody_online_time.elapsed() << " seconds" << endl
       << endl;

  cout << "----------End of compute_custody_bit_online-------------------" << endl;

  return res;
}

int run_poc_compute_custody_bit(const vector<Share> &keys, const vector<gfp> &msg, Config_Info &CI)
{
  cout << "----------Begin of compute_custody_bit-------------------" << endl;
  Timer genproof_time;
  genproof_time.start();

  Player &P = *(tinfo[ThreadPlayer::TP_PocGenProof].player);

  int res = poc_compute_custody_bit(keys, msg, 0, P, CI);

  genproof_time.stop();

  cout << "sent data: " << P.data_sent << " Bytes" << endl;
  cout << "recv data: " << P.data_received << " Bytes" << endl;
  cout << "sent msgs: " << P.pp_messages_sent << endl;
  cout << "recv msgs: " << P.pp_messages_recv << endl;

  cout << endl
       << "compute_custody_bit time " << genproof_time.elapsed() << " seconds" << endl
       << endl;

  cout << "----------End of compute_custody_bit---------------------" << endl;
  return res;
}

void run_online(Config_Info &CI)
{
  Player &P = *(tinfo[ThreadPlayer::TP_PocExtraOnline].player);

  printf("Setting up online phase threads\n");
  online_phase(0, P, CI.OCD, CI.machine);
}

void *Main_Offline_Func(void *ptr)
{
  thread_info *tinfo = (thread_info *)ptr;
  unsigned int num = tinfo->thread_num;
  int me = tinfo->me;
  int verbose = tinfo->verbose;
  printf("I am player %d in thread %d\n", me, num);
  fflush(stdout);

  offline_phase(num, 0, *(tinfo->player), (tinfo->SD)->fake_sacrifice, *(tinfo->OCD), verbose);

  return NULL;
}

void run_offline(Config_Info &CI)
{
  printf("Setting up offline phase threads\n");

  offline_time.start();

  // offline phase, 4 threads. id in [0,3], use CI.csockets[0~3]
  unsigned int offline_thread_nums = ThreadPlayer::TP_Offline + 1;
  offline_threads.resize(offline_thread_nums);
  for (unsigned int i = 0; i < offline_thread_nums; i++)
  {
    if (pthread_create(&offline_threads[i], NULL, Main_Offline_Func, &tinfo[i]))
    {
      throw C_problem("Problem spawning thread");
    }
  }
}

void wait_for_exit(Config_Info &CI)
{
  // set offline & online finished
  CI.OCD.OCD_mutex[0].lock();
  CI.OCD.finish_offline[0] = 1;
  CI.OCD.finished_online[0] = 1;
  CI.OCD.OCD_mutex[0].unlock();

  printf("Waiting for all offline clients to finish\n");
  fflush(stdout);
  for (unsigned int i = 0; i < offline_threads.size(); i++)
  {
    pthread_join(offline_threads[i], NULL);
  }

  offline_time.stop();
  cout << endl
       << "offline_time: " << offline_time.elapsed() << " seconds" << endl
       << endl;
  long long total_triples = 0, total_squares = 0, total_bits = 0, total_inputs = 0;
  for (size_t i = 0; i < CI.no_online_threads; i++)
  {
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