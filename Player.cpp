/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2019, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <pthread.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
using namespace std;

#include "System/Networking.h"
#include "System/RunTime.h"
#include "System/Player.h"
#include "config.h"
#include "Online/bls.h"
#include "Online/vss.h"

/*
  unsigned int my_number;
  unsigned int no_online_threads;
  vector<gfp> MacK;
  SSL_CTX *ctx;
  vector<unsigned int> portnum;
  SystemData SD;
  Machine machine;
  offline_control_data OCD;
  int verbose;
*/
void init(int argc, const char *argv[],
          unsigned int &my_number,
          unsigned int &no_online_threads,
          SSL_CTX *&ctx,
          vector<unsigned int> &portnum,
          SystemData &SD,
          Machine &machine,
          offline_control_data &OCD,
          int &verbose)
{
  if (argc != 2)
  {
    cerr << "ERROR: incorrect number of arguments to Player.x\n";
  }
  else
  {
    my_number = (unsigned int)atoi(argv[1]);
  }

  string memtype = "empty";
  unsigned int portnumbase = 5000;
  verbose = -1;

  /*************************************
   *  Setup offline_control_data OCD   *
   *************************************/
  //  offline_control_data OCD;
  OCD.minm = 0;
  OCD.mins = 0;
  OCD.minb = 0;
  OCD.maxm = 0;
  OCD.maxs = 0;
  OCD.maxb = 0;
  OCD.maxI = 0;

  cout << "(Min,Max) number of ...\n";
  cout << "\t(" << OCD.minm << ",";
  if (OCD.maxm == 0)
  {
    cout << "infty";
  }
  else
  {
    cout << OCD.maxm;
  }
  cout << ") multiplication triples" << endl;

  cout << "\t(" << OCD.mins << ",";
  if (OCD.maxs == 0)
  {
    cout << "infty";
  }
  else
  {
    cout << OCD.maxs;
  }
  cout << ") square pairs" << endl;

  cout << "\t(" << OCD.minb << ",";
  if (OCD.maxb == 0)
  {
    cout << "infty";
  }
  else
  {
    cout << OCD.maxb;
  }
  cout << ") bits" << endl;

  /*************************************
   *     Initialise the system data    *
   *************************************/
  //  SystemData SD("Data/NetworkData.txt");
  //  SD = SystemData("Data/NetworkData.txt");

  if (my_number >= SD.n)
  {
    throw data_mismatch();
  }

  /*************************************
   *    Initialize the portnums        *
   *************************************/
  //  vector<unsigned int> portnum(SD.n);
  portnum.resize(SD.n);

  for (unsigned int i = 0; i < SD.n; i++)
  {
    portnum[i] = portnumbase + i;
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
  if (ShD.M.nplayers() != SD.n)
  {
    throw data_mismatch();
  }
  if (SD.fake_offline == 1)
  {
    ShD.Otype = Fake;
  }
  Share::init_share_data(ShD);

  /* Initialize SSL */
  Init_SSL_CTX(ctx, my_number, SD);

  /* Initialize the machine */
  //  Machine machine;
  if (verbose < 0)
  {
    machine.set_verbose();
    verbose = 0;
  }
  machine.SetUp_Memory(my_number, memtype);

  // Here you configure the IO in the machine
  //  - This depends on what IO machinary you are using
  //  - Here we are just using the simple IO class
  unique_ptr<Input_Output_Simple> io(new Input_Output_Simple);
  io->init(cin, cout, true);
  machine.Setup_IO(std::move(io));

  // Load the initial tapes for the first program into the schedule
  //  unsigned int no_online_threads = 1;
  no_online_threads = 1;
}

/*
void testTool(Player &P)
{
  mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);

  string str;
  mclBnFr tmpfr;
  mclBnG1 basePoint, tmpG1;
  mclBnFr_setByCSPRNG(&tmpfr);
  print_mclBnFr(tmpfr);
  mclBnFr_to_str(str, tmpfr);
  str_to_mclBnFr(tmpfr, str);
  print_mclBnFr(tmpfr);
  cout << str.size() << endl;

  mclBnG1_setStr(&basePoint, (char *)G1_P.c_str(), G1_P.size(), 10);
  mclBnG1_mul(&tmpG1, &basePoint, &tmpfr);
  print_mclBnG1(tmpG1);
  mclBnG1_to_str(str, tmpG1);
  str_to_mclBnG1(tmpG1, str);
  print_mclBnG1(tmpG1);
  cout << str.size() << endl;

  cout << "test Player:\n";
  string ss;
  if (P.whoami() == 0)
  {
    ss = str;
    P.send_to_player(1, ss, 1);
  }

  if (P.whoami() == 1)
  {
    P.receive_from_player(0, ss, 1, false);
  }

  mclBnFr tmp;
  str_to_mclBnFr(tmp, ss);
  print_mclBnFr(tmp);
}

void testVSS()
{
  VSS v(3, 1);
  vector<mclBnFr> shs;
  vector<mclBnG1> aux;

  v.rnd_secret();
  v.gen_share(shs, aux);
  mclBnG1 tmp, tmp1;
  for (int i = 0; i < aux.size(); i++)
  {
    cout << "aux " << i << ": " << endl;
    print_mclBnG1(aux[i]);
  }
  mclBnG1_add(&tmp, &aux[0], &aux[1]);
  mclBnG1_add(&tmp1, &tmp, &aux[2]);
  print_mclBnG1(tmp1);

  cout << "secret:\n";
  print_mclBnFr(v.get_secret());

  cout << "recovered secret:\n";
  mclBnFr out;
  recover_share(out, shs);
  print_mclBnFr(out);

  int count = 0;

  for (int i = 0; i < v.nparty; i++)
  {
    if (!v.verify_share(shs[i], aux, i + 1))
    {
      count++;
    }
  }

  if (!count)
  {
    cout << "VSS Correct!" << endl;
  }
  else
  {
    cout << count << " VSS Errors!" << endl;
  }
}

void testBLS(Player &P)
{

  BLS bls1;
  cout << "Fr Size: " << mclBn_getFrByteSize() << endl;
  cout << "G1 Size: " << mclBn_getG1ByteSize() << endl;
  const string msg = "1234567890";
  bls1.keygen();
  bls1.sign(msg);

  BLS bls2;
  bls2.set_vk(bls1.vk);
  if (bls2.verify(bls1.sigma, msg))
  {
    cout << "Normal BLS Correct!" << endl;
  }
  else
  {
    cout << "Normal BLS Wrong!" << endl;
  }

  //DKG TEST
  BLS dbls(3, 1);
  dbls.dstb_keygen(P);
  cout << "vk is: " << endl;
  print_mclBnG1(dbls.vk);
  cout << "secret share is: " << endl;
  print_mclBnFr(dbls.get_sk());

  vector<bls_sk> shares(P.nplayers());
  shares[P.whoami()] = dbls.get_sk();
  string ss;
  mclBnFr out;
  mclBnG1 outG1;

  if (P.whoami() != 0)
  {
    mclBnFr_to_str(ss, dbls.get_sk());
    P.send_to_player(0, ss, 1);
  }
  else
  {
    for (int i = 1; i < P.nplayers(); i++)
    {
      P.receive_from_player(i, ss, 1, false);
      str_to_mclBnFr(shares[i], ss);
    }

    recover_share(out, shares);
    cout << "recovered secret key is: " << endl;
    print_mclBnFr(out);
    mclFr_to_G1(outG1, out);

    if (mclBnG1_isEqual(&outG1, &dbls.vk))
    {
      cout << "DKG test correct!\n";
    }
    else
    {
      cout << "DKG test wrong!\n";
    }
  }
}
*/
int main(int argc, const char *argv[])
{
  unsigned int my_number;
  unsigned int no_online_threads;
  SSL_CTX *ctx;
  vector<unsigned int> portnum;
  SystemData SD("Data/NetworkData.txt");
  Machine machine;
  offline_control_data OCD;
//  Player P;
  int verbose = 0;

  init(argc, argv,
       my_number,
       no_online_threads,
       ctx,
       portnum,
       SD,
       machine,
       OCD,
       verbose);
/*
  vector<unsigned int> portnum1(SD.n);
  for (int i = 0; i < SD.n; i++)
  {
    portnum1[i] = 50000 + SD.n + i;
  }
  int ssocket;
  vector<vector<vector<int>>> csockets(1, vector<vector<int>>(SD.n, vector<int>(3)));
  Get_Connections(ssocket, csockets, portnum1, my_number, SD, verbose - 2);
  printf("Connected!\n");
  P.Init(my_number, SD, 0, ctx, csockets[0], verbose);

  cout << "Number of Players: " << P.nplayers() << endl;
  
  testTool(P);
  testVSS();
  testBLS(P);

  Close_Connections(ssocket, csockets, my_number);
  */
  vector<gfp> MacK;
  MacK.resize(0);
  printf("begin runscale\n");
  Run_Scale(my_number, no_online_threads, MacK,
            ctx, portnum,
            SD, machine, OCD,
            verbose);

  machine.Dump_Memory(my_number);

  Destroy_SSL_CTX(ctx);

  printf("End of prog\n");
  fflush(stdout);
}
