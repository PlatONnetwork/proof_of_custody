/*
 * paper-example.cpp
 *
 * Working example similar to Figure 2 in https://eprint.iacr.org/2020/521
 *
 */

#define NO_MIXED_CIRCUITS

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include "POC/RunPoc.h"

const int PRIME_LENGTH = 381;     // bit length of prime
const string PROTOCOL = "MASCOT"; // protocol name

template <class T>
void run(char **argv, int prime_length);

int main(int argc, char **argv)
{
    // bit length of prime
    const int prime_length = PRIME_LENGTH;

    // compute number of 64-bit words needed
    const int n_limbs = (prime_length + 63) / 64;
    cout << "prime_length:" << prime_length << " n_limbs:" << n_limbs << endl;

    // need player number and number of players
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << "<my number: 0/1/...> <total number of players> [protocol [threshold]]" << endl;
        exit(1);
    }

    string protocol = PROTOCOL;
    if (argc > 3)
        protocol = argv[3];

    auto xtime = time(0);
    cout << "time0:" << time(0) << endl;
    if (protocol == "MASCOT")
        run<Share<gfp_<0, n_limbs>>>(argv, prime_length);
    else
    {
        cerr << "Unknown protocol: " << protocol << endl;
        exit(1);
    }
    cout << "time1:" << time(0) << endl;
    cout << "time:" << time(0) - xtime << endl;
}

template <class T>
void run(char **argv, int prime_length)
{
    typedef typename T::clear clear;
    cout << "typeid(T).name():" << TYPENAME(typeid(T).name()) << endl;
    cout << "typeid(typename T::clear).name():" << TYPENAME(typeid(typename T::clear).name()) << endl;
    cout << "typeid(clear).name():" << TYPENAME(typeid(clear).name()) << endl;

    /*
    ez::ezOptionParser opt;
    OnlineOptions &online_opts = OnlineOptions::singleton;
    online_opts = {opt, argc, argv, 1000, live_prep_default, true};
    OnlineOptions(ez::ezOptionParser & opt, int argc, const char **argv,
                  int default_batch_size = 0, bool default_live_prep = true,
                  bool variable_prime_length = false);
    */
    OnlineOptions &online_opts = OnlineOptions::singleton;
    cout << "online_opts.batch_size:" << online_opts.batch_size << endl;
    online_opts.batch_size = 100;
    cout << "online_opts.batch_size:" << online_opts.batch_size << endl;

    // set up networking on localhost
    Names N;
    int my_number = atoi(argv[1]);
    int n_parties = atoi(argv[2]);
    int port_base = 29999;
    Server::start_networking(N, my_number, n_parties, "localhost", port_base);
    //CryptoPlayer P(N);
    PlainPlayer P(N);
    cout << "time1:" << time(0) << endl;

    // initialize fields
    T::clear::init_default(prime_length);
    T::clear::next::init_default(prime_length, false);

    // must initialize MAC key for security of some protocols
    typename T::mac_key_type mac_key;
    T::read_or_generate_mac_key("", P, mac_key);

    // global OT setup
    BaseMachine machine;
    if (T::needs_ot)
        machine.ot_setups.push_back({P});

    // keeps tracks of preprocessing usage (triples etc)
    DataPositions usage;
    usage.set_num_players(P.num_players());

    // output protocol
    typename T::MAC_Check output(mac_key);

    // various preprocessing
    typename T::LivePrep preprocessing(0, usage);
    SubProcessor<T> processor(output, preprocessing, P);

    // input protocol
    typename T::Input input(processor, output);

    // multiplication protocol
    typename T::Protocol protocol(P);

    output.Check(P);
    {
        POC<T> poc(P, protocol, preprocessing, processor, output);
        OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);
        BLS<T> bls(P.num_players(), P.num_players() - 1);
        RunPOC<T> runpoc(poc, P, protocol, preprocessing, processor, output);
        Config_Info CI;

        // case 1
        {
            // stage 0-0
            runpoc.run_poc_setup(bls, CI);
            PRINT_DEBUG_INFO();

            string nonce = "123456";

            output.Check(P);
            // stage 1-1
            vector<bigint> local_bits, reveal_bits;
            runpoc.run_poc_compute_ephem_key_2primes_phase_one(local_bits, reveal_bits, bls, nonce, CI);
            PRINT_DEBUG_INFO();

            output.Check(P);
            // stage 2-1
            vector<T> ek(3);
            runpoc.run_poc_compute_ephem_key_2primes_phase_two(ek, local_bits, reveal_bits, CI);
            PRINT_DEBUG_INFO();

            output.Check(P);
            // stage 2-2
            vector<T> pre_key;
            runpoc.run_poc_compute_custody_bit_offline_2primes(pre_key, ek, CI);
            PRINT_DEBUG_INFO();

            output.Check(P);
            // stage 2-3
            int loops = 1;
            for (int l = 0; l < loops; l++)
            {
                cout << "run ...loops:" << l << "/" << loops << endl;
                Timer timer;
                timer.start();

                PRINT_DEBUG_INFO();

                {
                    vector<clear> msg(CHUNK_NUM);
                    for (int i = 0; i < msg.size(); i++)
                    {
                        msg[i] = i * 2;
                    }

                    PRINT_DEBUG_INFO();
                    int bit = runpoc.run_poc_compute_custody_bit_online_2primes(pre_key, ek[0], msg, CI);
                    PRINT_DEBUG_INFO();

                    cout << "custody bit: " << bit << endl;
                    timer.stop();
                    cout << "stage2 elapsed:" << timer.elapsed() << endl;
                }
            }
        }
    }

    output.Check(P);

    T::LivePrep::teardown();
}
