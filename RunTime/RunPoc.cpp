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

void Init(int argc, char *argv[], Config_Info &CI)
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
  CI.OCD.maxm = 0;
  CI.OCD.maxs = 0;
  CI.OCD.maxb = 0;
  CI.OCD.maxI = 0;

  cout << "(Min,Max) number of ...\n";
  cout << "\t(" << CI.OCD.minm << ",";
  if (CI.OCD.maxm == 0)
  {
    cout << "infty";
  }
  else
  {
    cout << CI.OCD.maxm;
  }
  cout << ") multiplication triples" << endl;

  cout << "\t(" << CI.OCD.mins << ",";
  if (CI.OCD.maxs == 0)
  {
    cout << "infty";
  }
  else
  {
    cout << CI.OCD.maxs;
  }
  cout << ") square pairs" << endl;

  cout << "\t(" << CI.OCD.minb << ",";
  if (CI.OCD.maxb == 0)
  {
    cout << "infty";
  }
  else
  {
    cout << CI.OCD.maxb;
  }
  cout << ") bits" << endl;

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
  //  vector<unsigned int> portnum(SD.n);
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
  cout << "\n\np=" << p << endl;
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
  Init_SSL_CTX(CI.ctx, CI.my_number, CI.SD);

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
}
//---------------------------//
//---------------------------//

class thread_info
{
public:
  int thread_num;
  const SystemData *SD;
  offline_control_data *OCD;
  SSL_CTX *ctx;
  int me;
  unsigned int no_online_threads;
  vector<vector<int>> csockets;
  vector<gfp> MacK;

  int verbose;

  Machine *machine; // Pointer to the machine
};

// We have 5 threads per online phase
//   - Online
//   - Sacrifice (and input tuple production)
//   - Mult Triple Production
//   - Square Pair Production
//   - Bit Production
vector<pthread_t> threads;

Timer global_time;

// Forward declarations to make code easier to read
void *Main_Func(void *ptr);

vector<sacrificed_data> SacrificeD;

/* Global data structure to hold the OT stuff */
//OT_Thread_Data OTD;

/* Before calling this we assume various things have
 * been set up. In particular the following functions have
 * been called
 *
 *    Init_SSL_CTX
 *    gfp::init_field
 *    Share::init_share_data
 *    machine.SetUp_Memory
 *    machine.IO.init
 *    FHE data has been initialized if needed
 *
 * We also assume the machine.schedule has been initialised
 * with some stringstream tapes and a stringstream to a schedule file
 *
 * This function assumes that afterwards we sort out 
 * closing down SSL and Dump'ing memory if need be
 * for a future application
 *
 */

void Run_Poc(BLS &bls, Config_Info &CI)
{

  CI.machine.SetUp_Threads(CI.no_online_threads);
  CI.OCD.resize(CI.no_online_threads, CI.SD.n, CI.my_number);

  SacrificeD.resize(CI.no_online_threads);
  for (unsigned int i = 0; i < CI.no_online_threads; i++)
  {
    SacrificeD[i].initialize(CI.SD.n);
  }

  unsigned int nthreads = 5 * CI.no_online_threads;
  unsigned int tnthreads = nthreads;

  /* Initialize the networking TCP sockets */
  int ssocket;
  vector<vector<vector<int>>> csockets(tnthreads + 1, vector<vector<int>>(CI.SD.n, vector<int>(3)));
  Get_Connections(ssocket, csockets, CI.portnum, CI.my_number, CI.SD, CI.verbose);
  printf("All connections now done\n");

  global_time.start();

  vector<gfp> MacK(0);

  Player P(CI.my_number, CI.SD, tnthreads, CI.ctx, csockets[tnthreads], MacK, CI.verbose);
  poc_Setup(bls, P);

  //--------------------------//
  printf("Setting up threads\n");
  fflush(stdout);
  threads.resize(tnthreads);
  vector<thread_info> tinfo(tnthreads);
  for (unsigned int i = 0; i < tnthreads; i++)
  {
    if (i < nthreads)
    {
      tinfo[i].thread_num = i;
    }
    tinfo[i].SD = &CI.SD;
    tinfo[i].OCD = &CI.OCD;
    tinfo[i].ctx = CI.ctx;
    tinfo[i].MacK = MacK;
    tinfo[i].me = CI.my_number;
    tinfo[i].no_online_threads = CI.no_online_threads;
    tinfo[i].csockets = csockets[i];
    tinfo[i].machine = &CI.machine;
    tinfo[i].verbose = CI.verbose;
    if (pthread_create(&threads[i], NULL, Main_Func, &tinfo[i]))
    {
      throw C_problem("Problem spawning thread");
    }
  }

  // Get all online threads in sync
  CI.machine.Synchronize();

  // Now run the programs
  CI.machine.run();

  printf("Waiting for all clients to finish\n");
  fflush(stdout);
  for (unsigned int i = 0; i < tnthreads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  Close_Connections(ssocket, csockets, CI.my_number);

  global_time.stop();
  cout << "Total Time (with thread locking) = " << global_time.elapsed() << " seconds" << endl;

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

void *Main_Func(void *ptr)
{
  thread_info *tinfo = (thread_info *)ptr;
  unsigned int num = tinfo->thread_num;
  int me = tinfo->me;
  int verbose = tinfo->verbose;
  printf("I am player %d in thread %d\n", me, num);
  fflush(stdout);

  Player P(me, *(tinfo->SD), num, (tinfo->ctx), (tinfo->csockets), (tinfo->MacK), verbose - 1);

  printf("Set up player %d in thread %d \n", me, num);
  fflush(stdout);

  if (num < 10000)
  {
    int num5 = num % 5;
    int num_online = (num - num5) / 5;
    switch (num5)
    {
    case 0:
      mult_phase(num_online, P, (tinfo->SD)->fake_sacrifice, *(tinfo->OCD), verbose);
      break;
    case 1:
      square_phase(num_online, P, (tinfo->SD)->fake_sacrifice, *(tinfo->OCD), verbose);
      break;
    case 2:
      bit_phase(num_online, P, (tinfo->SD)->fake_sacrifice, *(tinfo->OCD), verbose);
      break;
    case 3:
      inputs_phase(num_online, P, (tinfo->SD)->fake_sacrifice, *(tinfo->OCD), verbose);
      break;
    case 4:
      online_phase(num_online, P, *(tinfo->OCD), *(tinfo)->machine);
      break;
    default:
      throw bad_value();
      break;
    }
  }
  else
  {
    throw bad_value();
  }

#ifdef BENCH_NETDATA
  P.print_network_data(num);
#endif

#ifdef BENCH_MEMORY
  Print_Memory_Info(me, num);
#endif

  return 0;
}

#ifdef BENCH_MEMORY
void Print_Memory_Info(int player_num, int thread_num)
{
  int who = RUSAGE_THREAD;
  //int who = RUSAGE_SELF; // for the calling process
  struct rusage r_usage;

  int ret = getrusage(who, &r_usage);
  if (ret != 0)
  {
    printf(BENCH_TEXT_BOLD BENCH_COLOR_RED BENCH_MAGIC_START
           "MEMORY:\n"
           "  PLAYER#%d->THREAD#%u (PROCESS#%d)\n"
           "  ERROR: return value -> %d\n" BENCH_MAGIC_END BENCH_ATTR_RESET,
           player_num, thread_num, who, ret);
  }
  else
  {
    printf(BENCH_TEXT_BOLD BENCH_COLOR_RED BENCH_MAGIC_START
           "{\"player\":%u,\n"
           "  \"thread\":%d,\n"
           "  \"process\":%d,\n"
           "  \"memory\":{\n"
           "    \"max_rss\":{\"KB\":%ld,\"MB\":%.2f}\n"
           "  }\n"
           "}\n" BENCH_MAGIC_END BENCH_ATTR_RESET,
           player_num, thread_num, who, r_usage.ru_maxrss, ((double)r_usage.ru_maxrss / 1000));
  }
}
#endif