/*
 * Replicated.h
 *
 */

#ifndef PROTOCOLS_REPLICATED_H_
#define PROTOCOLS_REPLICATED_H_

#include <assert.h>
#include <vector>
#include <array>
using namespace std;

#include "Tools/octetStream.h"
#include "Tools/random.h"
#include "Tools/PointerVector.h"
#include "Networking/Player.h"

template<class T> class SubProcessor;
template<class T> class ReplicatedMC;
template<class T> class ReplicatedInput;
template<class T> class ReplicatedPrivateOutput;
template<class T> class Share;
template<class T> class Rep3Share;
template<class T> class MAC_Check_Base;
template<class T> class Preprocessing;
class Instruction;

class ReplicatedBase
{
public:
    array<PRNG, 2> shared_prngs;

    Player& P;

    ReplicatedBase(Player& P);
    ReplicatedBase(Player& P, array<PRNG, 2>& prngs);

    ReplicatedBase branch();

    int get_n_relevant_players() { return P.num_players() - 1; }
};

template <class T>
class ProtocolBase
{
    virtual void buffer_random() { throw not_implemented(); }

protected:
    vector<T> random;

    int trunc_pr_counter;

public:
    typedef T share_type;

    int counter;

    ProtocolBase();
    virtual ~ProtocolBase();

    void muls(const vector<int>& reg, SubProcessor<T>& proc, typename T::MAC_Check& MC,
            int size);
    void mulrs(const vector<int>& reg, SubProcessor<T>& proc);

    void multiply(vector<T>& products, vector<pair<T, T>>& multiplicands,
            int begin, int end, SubProcessor<T>& proc);

    T mul(const T& x, const T& y);

    virtual void init_mul(SubProcessor<T>* proc) = 0;
    virtual typename T::clear prepare_mul(const T& x, const T& y, int n = -1) = 0;
    virtual void exchange() = 0;
    virtual T finalize_mul(int n = -1) = 0;

    void init_dotprod(SubProcessor<T>* proc) { init_mul(proc); }
    void prepare_dotprod(const T& x, const T& y) { prepare_mul(x, y); }
    void next_dotprod() {}
    T finalize_dotprod(int length);

    virtual T get_random();

    virtual void trunc_pr(const vector<int>& regs, int size, SubProcessor<T>& proc)
    { (void) regs, (void) size; (void) proc; throw runtime_error("trunc_pr not implemented"); }

    virtual void randoms(T&, int) { throw runtime_error("randoms not implemented"); }
    virtual void randoms_inst(vector<T>&, const Instruction&);

    virtual void start_exchange() { exchange(); }
    virtual void stop_exchange() {}

    virtual void check() {}
};

template <class T>
class Replicated : public ReplicatedBase, public ProtocolBase<T>
{
    array<octetStream, 2> os;
    PointerVector<typename T::clear> add_shares;
    typename T::clear dotprod_share;

public:
    typedef ReplicatedMC<T> MAC_Check;
    typedef ReplicatedInput<T> Input;

    static const bool uses_triples = false;

    Replicated(Player& P);
    Replicated(const ReplicatedBase& other);

    static void assign(T& share, const typename T::clear& value, int my_num)
    {
        assert(T::length == 2);
        share.assign_zero();
        if (my_num < 2)
            share[my_num] = value;
    }

    void init_mul(SubProcessor<T>* proc);
    void init_mul(Preprocessing<T>& prep, typename T::MAC_Check& MC);

    void init_mul();
    typename T::clear prepare_mul(const T& x, const T& y, int n = -1);
    void exchange();
    T finalize_mul(int n = -1);

    void prepare_reshare(const typename T::clear& share, int n = -1);

    void init_dotprod(SubProcessor<T>*) { init_mul(); }
    void init_dotprod();
    void prepare_dotprod(const T& x, const T& y);
    void next_dotprod();
    T finalize_dotprod(int length);

    template<class U>
    void trunc_pr(const vector<int>& regs, int size, U& proc);

    T get_random();
    void randoms(T& res, int n_bits);

    void start_exchange();
    void stop_exchange();
};

#endif /* PROTOCOLS_REPLICATED_H_ */
