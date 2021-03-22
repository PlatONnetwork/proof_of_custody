/*
 * LowGearKeyGen.h
 *
 */

#ifndef PROTOCOLS_LOWGEARKEYGEN_H_
#define PROTOCOLS_LOWGEARKEYGEN_H_

#include "ShareVector.h"
#include "FHEOffline/PairwiseMachine.h"
#include "Protocols/MascotPrep.h"
#include "Processor/Processor.h"
#include "GC/TinierSecret.h"
#include "Math/gfp.h"

template<int X, int L>
class KeyGenProtocol
{
public:
    typedef Share<gfp_<X, L>> share_type;
    typedef typename share_type::open_type open_type;
    typedef ShareVector<share_type> vector_type;

protected:
    Player& P;
    const FHE_Params& params;
    const FFT_Data& fftd;

    SeededPRNG G;
    DataPositions usage;

    share_type get_bit()
    {
        return prep->get_bit();
    }

public:
    Preprocessing<share_type>* prep;
    MAC_Check_<share_type>* MC;
    SubProcessor<share_type>* proc;

    KeyGenProtocol(Player& P, const FHE_Params& params, int level = 0);
    ~KeyGenProtocol();

    void input(vector<vector_type>& shares, const Rq_Element& secret);
    template<class T>
    void binomial(vector_type& shares, T& prep);
    template<class T>
    void hamming(vector_type& shares, T& prep);
    template<class T>
    void secret_key(vector_type& shares, T& prep);
    vector_type schur_product(const vector_type& x, const vector_type& y);
    void output_to(int player, vector<open_type>& opened,
            vector<share_type>& shares);
};

template<int L>
class LowGearKeyGen : public KeyGenProtocol<5, L>
{
    typedef KeyGenProtocol<5, L> super;

    typedef typename super::share_type share_type;
    typedef typename super::open_type open_type;
    typedef typename super::vector_type vector_type;

    Player& P;
    PairwiseMachine& machine;

    void generate_keys(FHE_Params& params);

public:
    LowGearKeyGen(Player& P, PairwiseMachine& machine, FHE_Params& params);

    template<class FD>
    void run(PairwiseSetup<FD>& setup);
};

#endif /* PROTOCOLS_LOWGEARKEYGEN_H_ */
