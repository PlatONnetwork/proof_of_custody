#define NO_MIXED_CIRCUITS

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"
#include "Tools/Helper.h"

const int PRIME_LENGTH = 128;     // bit length of prime
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

    return 0;
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
    int port_base = 19999;
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

    int n = 3;
    vector<T> a(n), b(n);
    typename T::clear result;

    input.reset_all(P);
    cout << "time04:" << time(0) << endl;
    for (int i = 0; i < n; i++)
        input.add_from_all(i + 3);
    cout << "time05:" << time(0) << endl;

    input.exchange();
    for (int i = 0; i < n; i++)
    {
        a[i] = input.finalize(0);
        b[i] = input.finalize(1);
    }

#if 1
    vector<T> c(n);
    protocol.init_mul(&processor);
    cout << "time08:" << time(0) << endl;
    for (int i = 0; i < n; i++)
        protocol.prepare_mul(a[i], b[i]);
    cout << "time09:" << time(0) << endl;

    cout << "time18:" << time(0) << endl;
    for (int i = 0; i < n; i++)
        protocol.prepare_mul(a[i], b[i]);
    cout << "time19:" << time(0) << endl;

    protocol.exchange();
    for (int i = 0; i < n; i++)
        c[i] = protocol.finalize_mul();

    output.init_open(P, n);
    for (auto &s : c)
        output.prepare_open(s);
    output.exchange(P);

    vector<typename T::clear> results(n);
    for (int i = 0; i < n; i++)
    {
        results[i] = output.finalize_open();
        cout << "result" << i << ": " << results[i] << endl;
    }

#else
    T c;
    protocol.init_dotprod(&processor);
    cout << "time08:" << time(0) << endl;
    for (int i = 0; i < n; i++)
        protocol.prepare_dotprod(a[i], b[i]);
    cout << "time09:" << time(0) << endl;

    protocol.next_dotprod();
    protocol.exchange();
    c = protocol.finalize_dotprod(n);

    output.init_open(P);
    output.prepare_open(c);
    output.exchange(P);
    result = output.finalize_open();
    cout << "result: " << result << endl;
#endif
    output.Check(P);

    // auto xx = preprocessing.triple_generator;
    // for (auto &p : xx->players)
    // {
    //     cout << "------------------q" << endl;
    //     p->comm_stats.print();
    //     cout << "------------------x" << endl;
    // }

    T::LivePrep::teardown();
    P.comm_stats.print();
}
