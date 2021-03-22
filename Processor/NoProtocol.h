/*
 * NoProtocol.h
 *
 */

#ifndef PROCESSOR_NOPROTOCOL_H_
#define PROCESSOR_NOPROTOCOL_H_

#include "Protocols/Replicated.h"

template<class T>
class NoProtocol : public ProtocolBase<T>
{
public:
    NoProtocol(Player&)
    {

    }

    void init_mul(SubProcessor<T>*)
    {
        throw not_implemented();
    }
    typename T::clear prepare_mul(const T&, const T&, int n = -1)
    {
        (void) n;
        throw not_implemented();
    }
    void exchange()
    {
        throw not_implemented();
    }
    T finalize_mul(int n = -1)
    {
        (void) n;
        throw not_implemented();
    }
};



#endif /* PROCESSOR_NOPROTOCOL_H_ */
