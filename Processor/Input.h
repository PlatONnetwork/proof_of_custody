/*
 * Input.h
 *
 */

#ifndef PROCESSOR_INPUT_H_
#define PROCESSOR_INPUT_H_

#include <vector>
using namespace std;

#include "Tools/Buffer.h"
#include "Tools/time-func.h"
#include "Tools/PointerVector.h"

class ArithmeticProcessor;

template<class T>
class InputBase
{
    typedef typename T::clear clear;

protected:
    Player* P;

    Buffer<typename T::clear, typename T::clear> buffer;
    Timer timer;

public:
    vector<octetStream> os;
    int values_input;

    template<class U>
    static void input(SubProcessor<T>& Proc, const vector<int>& args, int size);

    static int get_player(SubProcessor<T>& Proc, int arg, bool player_from_arg);
    static void input_mixed(SubProcessor<T>& Proc, const vector<int>& args,
            int size, bool player_from_reg);
    template<class U>
    static void prepare(SubProcessor<T>& Proc, int player, const int* params, int size);
    template<class U>
    static void finalize(SubProcessor<T>& Proc, int player, const int* params, int size);

    InputBase(ArithmeticProcessor* proc = 0);
    InputBase(SubProcessor<T>* proc);
    virtual ~InputBase();

    virtual void reset(int player) = 0;
    void reset_all(Player& P);

    virtual void add_mine(const typename T::open_type& input, int n_bits = -1) = 0;
    virtual void add_other(int player) = 0;
    void add_from_all(const clear& input);

    virtual void send_mine() = 0;
    virtual void exchange();

    virtual T finalize_mine() = 0;
    virtual void finalize_other(int player, T& target, octetStream& o, int n_bits = -1) = 0;
    virtual T finalize(int player, int n_bits = -1);

    void raw_input(SubProcessor<T>& proc, const vector<int>& args, int size);
};

template<class T>
class Input : public InputBase<T>
{
    typedef typename T::open_type open_type;
    typedef typename T::clear clear;
    typedef typename T::MAC_Check MAC_Check;

    SubProcessor<T>* proc;
    MAC_Check& MC;
    Preprocessing<T>& prep;
    Player& P;
    vector< PointerVector<T> > shares;
    open_type rr, t, xi;

public:
    Input(SubProcessor<T>& proc);
    Input(SubProcessor<T>& proc, MAC_Check& mc);
    Input(SubProcessor<T>* proc, Player& P);
    Input(MAC_Check& MC, Preprocessing<T>& prep, Player& P);

    void reset(int player);

    void add_mine(const open_type& input, int n_bits = -1);
    void add_other(int player);

    void send_mine();

    T finalize_mine();
    void finalize_other(int player, T& target, octetStream& o, int n_bits = -1);
};

#endif /* PROCESSOR_INPUT_H_ */
