#pragma once

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include <string>

class Paras
{
public:
  int parties = -1;
  int partyid = -1;
  int lgp = 128;  // Bit length of GF(p) field
  int lgp1 = 381;
  int lgp2 = 256;
  std::string hostname = "127.0.0.1";
  int baseport = 8201;
  int batchsize = 100;
  bool use_encryption = false;
  bool run_stage = false; // if true, separate two processes
  int stage = 0;          // 0,all;1,stage one;2,stage two
};

static inline bool parse_args(int argc, const char **argv, Paras &paras)
{
  ez::ezOptionParser opt;
  opt.add(
      "2",                 // Default.
      0,                   // Required?
      1,                   // Number of args expected.
      0,                   // Delimiter if expecting multiple args.
      "Number of players", // Help description.
      "-N",                // Flag token.
      "--parties"          // Flag token.
  );
  opt.add("0", 0, 1, 0, "Player number", "-P", "--partyid");
  opt.add(
      "128",                                      // Default.
      0,                                          // Required?
      1,                                          // Number of args expected.
      0,                                          // Delimiter if expecting multiple args.
      "Bit length of GF(p) field (default: 128)", // Help description.
      "-lgp",                                     // Flag token.
      "--lgp"                                     // Flag token.
  );
  opt.add("381", 0, 1, 0, "", "-lgp1", "--lgp1");
  opt.add("256", 0, 1, 0, "", "-lgp2", "--lgp2");
  opt.add(
      "localhost",                                          // Default.
      0,                                                    // Required?
      1,                                                    // Number of args expected.
      0,                                                    // Delimiter if expecting multiple args.
      "Host where party 0 is running (default: localhost)", // Help description.
      "-h",                                                 // Flag token.
      "--hostname"                                          // Flag token.
  );
  opt.add(
      "8201",                              // Default.
      0,                                   // Required?
      1,                                   // Number of args expected.
      0,                                   // Delimiter if expecting multiple args.
      "Base port number (default: 8201).", // Help description.
      "-pn",                               // Flag token.
      "--portnum"                          // Flag token.
  );
  opt.add("100", 0, 1, 0, "Batch size", "-bs", "--batchsize");
  opt.add(
      "",                        // Default.
      0,                         // Required?
      0,                         // Number of args expected.
      0,                         // Delimiter if expecting multiple args.
      "Use encrypted channels.", // Help description.
      "-e",                      // Flag token.
      "--encrypted"              // Flag token.
  );
  opt.add("", 0, 0, 0, "Run stages in two processes", "-r", "--run_stage");

  opt.parse(argc, argv);
  opt.get("-N")->getInt(paras.parties);
  opt.get("-P")->getInt(paras.partyid);
  opt.get("-lgp")->getInt(paras.lgp);
  opt.get("-lgp1")->getInt(paras.lgp1);
  opt.get("-lgp2")->getInt(paras.lgp2);
  opt.get("-h")->getString(paras.hostname);
  opt.get("-pn")->getInt(paras.baseport);
  opt.get("-bs")->getInt(paras.batchsize);
  paras.use_encryption = opt.isSet("-e");
  paras.run_stage = opt.isSet("-s");

#ifdef DEBUG
  string usage;
  opt.getUsage(usage);
  cout << "usage:\n"
       << usage << endl;
  string out;
  opt.prettyPrint(out);
  cout << "options:\n"
       << out << endl;
#endif

  return true;
}
