/*
 * FakeProtocol.h
 *
 */

#ifndef PROTOCOLS_FAKEPROTOCOL_H_
#define PROTOCOLS_FAKEPROTOCOL_H_

#include "Replicated.h"
#include "Math/Z2k.h"

template<class T>
class FakeProtocol : public ProtocolBase<T>
{
    PointerVector<T> results;
    SeededPRNG G;

    T dot_prod;

    T trunc_max;

public:
    Player& P;

    FakeProtocol(Player& P) : P(P)
    {
    }

#ifdef VERBOSE
    ~FakeProtocol()
    {
        output_trunc_max<0>(T::invertible);
    }

    template<int>
    void output_trunc_max(false_type)
    {
        if (trunc_max != T())
            cerr << "Maximum bit length in truncation: "
                << (bigint(typename T::clear(trunc_max)).numBits() + 1)
                << " (" << trunc_max << ")" << endl;
    }

    template<int>
    void output_trunc_max(true_type)
    {
    }
#endif

    FakeProtocol branch()
    {
        return P;
    }

    void init_mul(SubProcessor<T>*)
    {
        results.clear();
    }

    typename T::clear prepare_mul(const T& x, const T& y, int = -1)
    {
        results.push_back(x * y);
        return {};
    }

    void exchange()
    {
    }

    T finalize_mul(int = -1)
    {
        return results.next();
    }

    void init_dotprod(SubProcessor<T>* proc)
    {
        init_mul(proc);
        dot_prod = {};
    }

    void prepare_dotprod(const T& x, const T& y)
    {
        dot_prod += x * y;
    }

    void next_dotprod()
    {
        results.push_back(dot_prod);
        dot_prod = 0;
    }

    T finalize_dotprod(int)
    {
        return finalize_mul();
    }

    void randoms(T& res, int n_bits)
    {
        res.randomize_part(G, n_bits);
    }

    int get_n_relevant_players()
    {
        return 1;
    }

    void trunc_pr(const vector<int>& regs, int size, SubProcessor<T>& proc)
    {
        trunc_pr<0>(regs, size, proc, T::characteristic_two);
    }

    template<int>
    void trunc_pr(const vector<int>&, int, SubProcessor<T>&, true_type)
    {
        throw not_implemented();
    }

    template<int>
    void trunc_pr(const vector<int>& regs, int size, SubProcessor<T>& proc, false_type)
    {
        for (size_t i = 0; i < regs.size(); i += 4)
            for (int l = 0; l < size; l++)
            {
                auto& res = proc.get_S_ref(regs[i] + l);
                auto& source = proc.get_S_ref(regs[i + 1] + l);
                T tmp = source - (T(1) << regs[i + 2] - 1);
                tmp = tmp < T() ? (T() - tmp) : tmp;
                trunc_max = max(trunc_max, tmp);
#ifdef CHECK_BOUNDS_IN_TRUNC_PR_EMULATION
                auto test = (source >> (regs[i + 2]));
                if (test != 0)
                {
                    cerr << typename T::clear(source) << " has more than "
                            << regs[i + 2]
                            << " bits in " << regs[i + 3]
                            << "-bit truncation (test value "
                            << typename T::clear(test) << ")" << endl;
                    throw runtime_error("trunc_pr overflow");
                }
#endif
                int n_shift = regs[i + 3];
#ifdef ROUND_NEAREST_IN_EMULATION
                res = source >> n_shift;
                if (n_shift > 0)
                {
                    bool overflow = T(source >> (n_shift - 1)).get_bit(0);
                    res += overflow;
                }
#else
#ifdef RISKY_TRUNCATION_IN_EMULATION
                T r;
                r.randomize(G);

                if (source.negative())
                    res = -T(((-source + r) >> n_shift) - (r >> n_shift));
                else
                    res = ((source + r) >> n_shift) - (r >> n_shift);
#else
                T r;
                r.randomize_part(G, n_shift - 1);
                res = (source + r) >> n_shift;
#endif
#endif
            }
    }
};

#endif /* PROTOCOLS_FAKEPROTOCOL_H_ */
