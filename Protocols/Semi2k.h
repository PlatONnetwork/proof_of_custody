/*
 * Semi2k.h
 *
 */

#ifndef PROTOCOLS_SEMI2K_H_
#define PROTOCOLS_SEMI2K_H_

#include "SPDZ.h"

template<class T>
class Semi2k : public SPDZ<T>
{
    SeededPRNG G;

public:
    Semi2k(Player& P) :
            SPDZ<T>(P)
    {
    }

    void randoms(T& res, int n_bits)
    {
        res.randomize_part(G, n_bits);
    }
};

#endif /* PROTOCOLS_SEMI2K_H_ */
