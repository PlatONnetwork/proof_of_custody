/*
 * TinyPrep.h
 *
 */

#ifndef GC_TINYPREP_H_
#define GC_TINYPREP_H_

#include "Thread.h"
#include "OT/MascotParams.h"
#include "Protocols/Beaver.h"
#include "Protocols/ReplicatedPrep.h"

namespace GC
{

template<class T>
class TinyPrep : public BufferPrep<T>
{
protected:
    ShareThread<T>& thread;

    typename T::TripleGenerator* triple_generator;
    MascotParams params;

    vector<array<typename T::part_type, 3>> triple_buffer;

    const bool amplify;

public:
    TinyPrep(DataPositions& usage, ShareThread<T>& thread, bool amplify = true);
    ~TinyPrep();

    void set_protocol(Beaver<T>& protocol);
    void init(Player& P);

    void buffer_triples();
    void buffer_bits();

    void buffer_squares() { throw not_implemented(); }
    void buffer_inverses() { throw not_implemented(); }

    void buffer_inputs_(int player, typename T::InputGenerator* input_generator);

    array<T, 3> get_triple_no_count(int n_bits);

    size_t data_sent();
};

template<class T>
class TinyOnlyPrep : public TinyPrep<T>
{
    typename T::part_type::TripleGenerator* input_generator;

public:
    TinyOnlyPrep(DataPositions& usage, ShareThread<T>& thread);
    ~TinyOnlyPrep();

    void set_protocol(Beaver<T>& protocol);

    void buffer_inputs(int player)
    {
        this->buffer_inputs_(player, input_generator);
    }

    size_t data_sent();
};

} /* namespace GC */

#endif /* GC_TINYPREP_H_ */
