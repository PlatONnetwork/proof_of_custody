/*
 * MaliciousRepSecret.h
 *
 */

#ifndef GC_MALICIOUSREPSECRET_H_
#define GC_MALICIOUSREPSECRET_H_

#include "ShareSecret.h"
#include "Machine.h"
#include "ThreadMaster.h"
#include "Protocols/Beaver.h"
#include "Protocols/MaliciousRepMC.h"
#include "Processor/DummyProtocol.h"

template<class T> class MaliciousRepMC;

namespace GC
{

template<class T> class ShareThread;
template<class T> class RepPrep;

class SmallMalRepSecret : public FixedVec<BitVec_<unsigned char>, 2>
{
    typedef FixedVec<BitVec_<unsigned char>, 2> super;
    typedef SmallMalRepSecret This;

public:
    typedef MaliciousRepMC<This> MC;
    typedef BitVec_<unsigned char> open_type;
    typedef open_type clear;
    typedef BitVec mac_key_type;

    static MC* new_mc(mac_key_type)
    {
        return new HashMaliciousRepMC<This>;
    }

    SmallMalRepSecret()
    {
    }
    template<class T>
    SmallMalRepSecret(const T& other) :
            super(other)
    {
    }

    This lsb() const
    {
        return *this & 1;
    }
};

class MaliciousRepSecret : public ReplicatedSecret<MaliciousRepSecret>
{
    typedef ReplicatedSecret<MaliciousRepSecret> super;

public:
    typedef Memory<MaliciousRepSecret> DynamicMemory;

    typedef MaliciousRepMC<MaliciousRepSecret> MC;
    typedef MC MAC_Check;

    typedef Beaver<MaliciousRepSecret> Protocol;
    typedef ReplicatedInput<MaliciousRepSecret> Input;
    typedef RepPrep<MaliciousRepSecret> LivePrep;

    typedef MaliciousRepSecret part_type;
    typedef MaliciousRepSecret whole_type;

    typedef SmallMalRepSecret small_type;

    static const bool expensive_triples = true;

    static MC* new_mc(mac_key_type)
    {
        try
        {
            if (ThreadMaster<MaliciousRepSecret>::s().machine.more_comm_less_comp)
                return new CommMaliciousRepMC<MaliciousRepSecret>;
        }
        catch(no_singleton& e)
        {
        }
        return new HashMaliciousRepMC<MaliciousRepSecret>;
    }

    static MaliciousRepSecret constant(const BitVec& other, int my_num, const BitVec& alphai)
    {
        (void) my_num, (void) alphai;
        return other;
    }

    MaliciousRepSecret() {}
    template<class T>
    MaliciousRepSecret(const T& other) : super(other) {}
};

}

#endif /* GC_MALICIOUSREPSECRET_H_ */
