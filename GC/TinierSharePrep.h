/*
 * TinierSharePrep.h
 *
 */

#ifndef GC_TINIERSHAREPREP_H_
#define GC_TINIERSHAREPREP_H_

#include "Protocols/ReplicatedPrep.h"
#include "OT/NPartyTripleGenerator.h"
#include "ShareThread.h"
#include "PersonalPrep.h"

namespace GC
{

template<class T> class TinierPrep;
template<class T> class TinierSecret;

template<class T>
class TinierSharePrep : public PersonalPrep<T>
{
    typename T::TripleGenerator* triple_generator;
    MascotParams params;

    TinierPrep<TinierSecret<typename T::mac_key_type>> whole_prep;

    void buffer_triples();
    void buffer_squares() { throw not_implemented(); }
    void buffer_bits() { throw not_implemented(); }
    void buffer_inverses() { throw not_implemented(); }

    void buffer_inputs(int player);

public:
    TinierSharePrep(DataPositions& usage, int input_player =
            PersonalPrep<T>::SECURE);
    TinierSharePrep(SubProcessor<T>*, DataPositions& usage);
    ~TinierSharePrep();

    void set_protocol(typename T::Protocol& protocol);

    size_t data_sent();
};

}

#endif /* GC_TINIERSHAREPREP_H_ */
