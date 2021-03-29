#define NO_MIXED_CIRCUITS

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include "POC/RunPoc.h"
#include "poc_args.h"

template <class T>
void run(const Paras &paras);
int main(int argc, const char **argv)
{
    Paras paras;
    if (!parse_args(argc, argv, paras))
    {
        cerr << "parse args failed!" << endl;
        exit(1);
    }

    int prime_length = paras.lgp;           // bit length of prime
    int n_limbs = (prime_length + 63) / 64; // compute number of 64-bit words needed
    cout << "prime_length:" << prime_length << " n_limbs:" << n_limbs << endl;

    // MASCOT
    Timer timer;
    timer.start();
    PRINT_DEBUG_INFO();
    switch (n_limbs)
    {
#define CASE_RUN(N)                    \
    case N:                            \
        run<Share<gfp_<0, N>>>(paras); \
        break
        CASE_RUN(1);
        CASE_RUN(2);
        CASE_RUN(3);
        CASE_RUN(4);
        CASE_RUN(5);
        CASE_RUN(6);
        CASE_RUN(7);
        CASE_RUN(8);
#undef CASE_RUN
    default:
        cerr << "not supported! n_limbs:" << n_limbs << endl;
        exit(1);
    }
    PRINT_DEBUG_INFO();
    cout << "POC run elapsed(s):" << timer.elapsed() << endl;

    return 0;
}

template <class T>
void run(const Paras &paras)
{
    Timer timer;
    timer.start();

    typedef typename T::clear clear;
    cout << "typeid(T).name():" << TYPENAME(typeid(T).name()) << endl;
    cout << "typeid(typename T::clear).name():" << TYPENAME(typeid(typename T::clear).name()) << endl;
    cout << "typeid(clear).name():" << TYPENAME(typeid(clear).name()) << endl;

    OnlineOptions &online_opts = OnlineOptions::singleton;
    online_opts.batch_size = paras.batchsize;
    Names N;
    Server::start_networking(N, paras.partyid, paras.parties, paras.hostname, paras.baseport);
    Player *player = nullptr;
    if (paras.use_encryption)
        player = new CryptoPlayer(N);
    else
        player = new PlainPlayer(N); // ThreadPlayer P(N);
    Player &P = *player;

    // initialize fields
    T::clear::init_default(paras.lgp);
    T::clear::next::init_default(paras.lgp, false);
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
    cout << "POC init elapsed(s):" << timer.elapsed_then_reset() << endl;

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
            cout << "POC run_poc_setup elapsed(s):" << timer.elapsed_then_reset() << endl;
            PRINT_DEBUG_INFO();

            string nonce = "123456";

            // stage 1-1
            vector<bigint> local_bits, reveal_bits;
            runpoc.run_poc_compute_ephem_key_2primes_phase_one(local_bits, reveal_bits, bls, nonce, CI);
            cout << "POC run_poc_compute_ephem_key_2primes_phase_one elapsed(s):" << timer.elapsed_then_reset() << endl;
            PRINT_DEBUG_INFO();

            // stage 2-1
            vector<T> ek(3);
            runpoc.run_poc_compute_ephem_key_2primes_phase_two(ek, local_bits, reveal_bits, CI);
            cout << "POC run_poc_compute_ephem_key_2primes_phase_two elapsed(s):" << timer.elapsed_then_reset() << endl;
            PRINT_DEBUG_INFO();

            // stage 2-2
            vector<T> pre_key;
            runpoc.run_poc_compute_custody_bit_offline_2primes(pre_key, ek, CI);
            cout << "POC run_poc_compute_custody_bit_offline_2primes elapsed(s):" << timer.elapsed_then_reset() << endl;
            PRINT_DEBUG_INFO();

            // stage 2-3
            {
                vector<clear> msg(CHUNK_NUM);
                for (int i = 0; i < msg.size(); i++)
                    msg[i] = i * 2;

                PRINT_DEBUG_INFO();
                int bit = runpoc.run_poc_compute_custody_bit_online_2primes(pre_key, ek[0], msg, CI);
                cout << "POC run_poc_compute_custody_bit_online_2primes elapsed(s):" << timer.elapsed_then_reset() << endl;
                cout << "custody bit: " << bit << endl;
                PRINT_DEBUG_INFO();
            }
        }
    }

    output.Check(P);
    T::LivePrep::teardown();

    delete player;
}
