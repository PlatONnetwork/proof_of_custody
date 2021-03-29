#define NO_MIXED_CIRCUITS

#include "Math/gfp.hpp"
#include "Machines/SPDZ.hpp"

#include "POC/RunPoc.h"
#include "poc_args.h"
#include "ipc_msg.h"
#include <unistd.h>

template <class T>
void run(const Paras &paras);
int run_stage(const Paras &paras);

int main(int argc, const char **argv)
{
    Paras paras;
    if (!parse_args(argc, argv, paras))
    {
        cerr << "parse args failed!" << endl;
        exit(1);
    }

    paras.batchsize = 100;
    paras.run_stage = true;
    if (!paras.run_stage)
    {
        cout << "stage all pid:" << getpid() << " Begin!" << endl;
        paras.stage = 0;
        run_stage(paras);
        return 0;
    }

    pid_t fpid = fork();
    if (fpid < 0)
    {
        cerr << "error in fork!" << endl;
        exit(1);
    }

    int loops = 5;
    int ret = -1;
    int status = -1;
    if (fpid == 0)
    {
        cout << "stage one pid:" << getpid() << " Begin!" << endl;
        paras.stage = 1;
        ret = run_stage(paras);
        cout << "stage one pid:" << getpid() << " End!" << endl;
    }
    else
    {
        cout << "stage two pid:" << getpid() << " Begin!" << endl;
        paras.stage = 2;
        paras.lgp = 128;
        paras.baseport = (paras.baseport > 50000) ? (paras.baseport - 10000) : (paras.baseport + 10000);
        ret = run_stage(paras);
        cout << "stage two pid:" << getpid() << " End!" << endl;

        waitpid(fpid, &status, 0);
        printf("status = %d\n", WEXITSTATUS(status));
    }
    return 0;
}

int run_stage(const Paras &paras)
{
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

        int stage = paras.stage;

        // stage 0-0
        runpoc.run_poc_setup(bls, CI);
        cout << "POC run_poc_setup elapsed(s):" << timer.elapsed_then_reset() << endl;
        PRINT_DEBUG_INFO();

        vector<bigint> local_bits, reveal_bits;
        if (stage == 0 || stage == 1)
        {
            string nonce = "123456";

            // stage 1-1
            runpoc.run_poc_compute_ephem_key_2primes_phase_one(local_bits, reveal_bits, bls, nonce, CI);
            cout << "POC run_poc_compute_ephem_key_2primes_phase_one elapsed(s):" << timer.elapsed_then_reset() << endl;
            PRINT_DEBUG_INFO();

            if (stage == 1)
            {
                // separation ..................
                stringstream ss;
                for (size_t i = 0; i < local_bits.size(); i++)
                {
                    ss << local_bits[i];
                }
                for (size_t i = 0; i < reveal_bits.size(); i++)
                {
                    ss << reveal_bits[i];
                }
                int size = local_bits.size();
                string s(ss.str());
                int len = s.size();

                cout << "send len:" << len << ",size:" << size << endl;

                ipcmsg msg;
                msg.pathname = string("/tmp/ipc.poc.p" + to_string(P.my_num()));
                ipc_msg_init(msg);

                msg.buf.mtype = 1; // vector size
                msg.buf.mlen = sizeof(int);
                memcpy(msg.buf.mtext, (char *)&size, sizeof(int));
                ipc_msg_send(msg);

                msg.buf.mtype = 2; // content size
                msg.buf.mlen = sizeof(int);
                memcpy(msg.buf.mtext, (char *)&len, sizeof(int));
                ipc_msg_send(msg);

                msg.buf.mtype = 3; // content
                msg.buf.mlen = len;
                memcpy(msg.buf.mtext, s.data(), len);
                ipc_msg_send(msg);

                msg.buf.mtype = 4; // end
                ipc_msg_recv(msg);
                ipc_msg_uninit(msg);
            }
        }
        if (stage == 0 || stage == 2)
        {
            if (stage == 2)
            {
                int size;
                int len;
                string s;

                ipcmsg msg;
                msg.pathname = string("/tmp/ipc.poc.p" + to_string(P.my_num()));
                ipc_msg_init(msg);

                msg.buf.mtype = 1; // vector size
                ipc_msg_recv(msg);
                memcpy((char *)&size, msg.buf.mtext, msg.buf.mlen);

                msg.buf.mtype = 2; // content size
                ipc_msg_recv(msg);
                memcpy((char *)&len, msg.buf.mtext, msg.buf.mlen);

                s.resize(len);
                msg.buf.mtype = 3; // content
                ipc_msg_recv(msg);
                memcpy((char *)s.data(), msg.buf.mtext, msg.buf.mlen);
                cout << "recv len:" << len << ",size:" << size << endl;

                local_bits.resize(size);
                reveal_bits.resize(size);
                stringstream is(s);
                for (size_t i = 0; i < local_bits.size(); i++)
                {
                    is >> local_bits[i];
                }
                for (size_t i = 10; i < reveal_bits.size(); i++)
                {
                    is >> reveal_bits[i];
                }
                // separation ..................

                msg.buf.mtype = 4; // end
                msg.buf.mlen = 1;
                ipc_msg_send(msg);
            }

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
            for (int iii = 0; iii < 10; iii++)
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
    player->comm_stats.print();
    delete player;
}
