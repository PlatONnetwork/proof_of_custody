#define NO_MIXED_CIRCUITS

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include "POC/OnlineOp.h"
#include "POC/Group.h"

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
    int port_base = 15555;
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

    {
        OnlineOp<T> online_op(P, protocol, preprocessing, processor, output);
        online_op.test_add();
        online_op.test_sub();
        online_op.test_mul();
        online_op.test_div();
        online_op.test_sqr();
        online_op.test_inv();

        online_op.test_uhf();
        online_op.test_legendre();
        online_op.test_get_inputs();
        online_op.test_bit_ops();
    }
    {
        G1Op<T> online_op(P, protocol, preprocessing, processor, output);
        online_op.G1_test_add_aff();
        online_op.G1_test_add_jac();
        online_op.G1_test_add_proj();
    }
    {
        G2Op<T> online_op(P, protocol, preprocessing, processor, output);
        online_op.G2_test_add_aff();
        online_op.G2_test_add_jac();
        online_op.G2_test_add_proj();
    }

    output.Check(P);
    T::LivePrep::teardown();
}
