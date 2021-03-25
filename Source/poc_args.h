#pragma once

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include <string>

class Paras
{
public:
  int parties = -1;
  int partyid = -1;
  int lgp = 128; // Bit length of GF(p) field
  std::string hostname = "127.0.0.1";
  int baseport = 5000;
  int batchsize = 100;
  bool use_encryption = false;
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
      "5000",                              // Default.
      0,                                   // Required?
      1,                                   // Number of args expected.
      0,                                   // Delimiter if expecting multiple args.
      "Base port number (default: 5000).", // Help description.
      "-pn",                               // Flag token.
      "--portnum"                          // Flag token.
  );
  opt.add("500", 0, 1, 0, "Batch size", "-bs", "--batchsize");
  opt.add(
      "",                        // Default.
      0,                         // Required?
      0,                         // Number of args expected.
      0,                         // Delimiter if expecting multiple args.
      "Use encrypted channels.", // Help description.
      "-e",                      // Flag token.
      "--encrypted"              // Flag token.
  );

  opt.parse(argc, argv);
  opt.get("-N")->getInt(paras.parties);
  opt.get("-P")->getInt(paras.partyid);
  opt.get("-lgp")->getInt(paras.lgp);
  opt.get("-h")->getString(paras.hostname);
  opt.get("-pn")->getInt(paras.baseport);
  opt.get("-bs")->getInt(paras.batchsize);
  paras.use_encryption = opt.isSet("-e");

#ifdef VERBOSE
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
