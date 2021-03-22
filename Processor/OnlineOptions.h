/*
 * OnlineOptions.h
 *
 */

#ifndef PROCESSOR_ONLINEOPTIONS_H_
#define PROCESSOR_ONLINEOPTIONS_H_

#include "Tools/ezOptionParser.h"
#include "Math/bigint.h"

class OnlineOptions
{
public:
    static OnlineOptions singleton;

    bool interactive;
    int lgp;
    bigint prime;
    bool live_prep;
    int playerno;
    std::string progname;
    int batch_size;
    std::string memtype;
    bool bits_from_squares;
    bool direct;
    int bucket_size;
    std::string cmd_private_input_file;
    std::string cmd_private_output_file;

    OnlineOptions();
    OnlineOptions(ez::ezOptionParser& opt, int argc, const char** argv,
            int default_batch_size = 0, bool default_live_prep = true,
            bool variable_prime_length = false);
    void finalize(ez::ezOptionParser& opt, int argc, const char** argv);

    int prime_length();
    int prime_limbs();
};

#endif /* PROCESSOR_ONLINEOPTIONS_H_ */
