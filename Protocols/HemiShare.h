/*
 * HemiShare.h
 *
 */

#ifndef PROTOCOLS_HEMISHARE_H_
#define PROTOCOLS_HEMISHARE_H_

#include "SemiShare.h"

template<class T> class HemiPrep;

template<class T>
class HemiShare : public SemiShare<T>
{
    typedef HemiShare This;
    typedef SemiShare<T> super;

public:
    typedef SemiMC<This> MAC_Check;
    typedef DirectSemiMC<This> Direct_MC;
    typedef SemiInput<This> Input;
    typedef ::PrivateOutput<This> PrivateOutput;
    typedef SPDZ<This> Protocol;
    typedef HemiPrep<This> LivePrep;

    static const bool needs_ot = false;

    HemiShare()
    {
    }
    template<class U>
    HemiShare(const U& other) :
            super(other)
    {
    }

};

#endif /* PROTOCOLS_HEMISHARE_H_ */
