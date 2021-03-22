/*
 * HemiPrep.h
 *
 */

#ifndef PROTOCOLS_HEMIPREP_H_
#define PROTOCOLS_HEMIPREP_H_

#include "ReplicatedPrep.h"
#include "FHEOffline/Multiplier.h"

template<class T>
class HemiPrep : public SemiHonestRingPrep<T>
{
    typedef typename T::clear::FD FD;

    static PairwiseMachine* pairwise_machine;
    static Lock lock;

    vector<Multiplier<FD>*> multipliers;

    SeededPRNG G;

    map<string, Timer> timers;

public:
    static void basic_setup(Player& P);
    static void teardown();

    HemiPrep(SubProcessor<T>* proc, DataPositions& usage) :
            BufferPrep<T>(usage),
            BitPrep<T>(proc, usage), RingPrep<T>(proc, usage),
            SemiHonestRingPrep<T>(proc, usage)
    {
    }

    void buffer_triples();
};

#endif /* PROTOCOLS_HEMIPREP_H_ */
