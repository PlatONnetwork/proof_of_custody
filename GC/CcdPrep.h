/*
 * CcdPrep.h
 *
 */

#ifndef GC_CCDPREP_H_
#define GC_CCDPREP_H_

#include "Protocols/ReplicatedPrep.h"

class DataPositions;

namespace GC
{

template<class T> class ShareThread;

template<class T>
class CcdPrep : public BufferPrep<T>
{
    typename T::part_type::LivePrep part_prep;
    typename T::part_type::MAC_Check part_MC;
    SubProcessor<typename T::part_type>* part_proc;
    ShareThread<T>& thread;

public:
    CcdPrep(DataPositions& usage, ShareThread<T>& thread) :
            BufferPrep<T>(usage), part_prep(usage), part_proc(0), thread(thread)
    {
    }

    CcdPrep(SubProcessor<T>*, DataPositions& usage) :
            CcdPrep(usage, ShareThread<T>::s())
    {
    }

    ~CcdPrep()
    {
        if (part_proc)
            delete part_proc;
    }

    void set_protocol(typename T::Protocol& protocol)
    {
        part_proc = new SubProcessor<typename T::part_type>(part_MC,
                part_prep, protocol.get_part().P);
    }

    Preprocessing<typename T::part_type>& get_part()
    {
        return part_prep;
    }

    void buffer_triples()
    {
        throw not_implemented();
    }

    void buffer_bits()
    {
        assert(part_proc);
        for (int i = 0; i < OnlineOptions::singleton.batch_size; i++)
            this->bits.push_back(part_prep.get_bit());
    }

    void buffer_squares()
    {
        throw not_implemented();
    }

    void buffer_inverses()
    {
        throw not_implemented();
    }
};

} /* namespace GC */

#endif /* GC_CCDPREP_H_ */
