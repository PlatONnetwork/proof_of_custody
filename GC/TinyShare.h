/*
 * TinyShare.h
 *
 */

#ifndef GC_TINYSHARE_H_
#define GC_TINYSHARE_H_

#include "ShareSecret.h"
#include "ShareParty.h"
#include "Secret.h"
#include "Protocols/Spdz2kShare.h"
#include "Processor/NoLivePrep.h"

namespace GC
{

template<int S> class TinySecret;
template<class T> class ShareThread;

template<int S>
class TinyShare : public Spdz2kShare<1, S>, public ShareSecret<TinySecret<S>>
{
    typedef TinyShare This;

public:
    typedef Spdz2kShare<1, S> super;

    typedef void DynamicMemory;

    typedef NoLivePrep<This> LivePrep;

    typedef SwitchableOutput out_type;

    typedef This small_type;

    static string name()
    {
        return "tiny share";
    }

    static ShareThread<TinySecret<S>>& get_party()
    {
        return ShareThread<TinySecret<S>>::s();
    }

    static This new_reg()
    {
        return {};
    }

    TinyShare()
    {
    }
    TinyShare(const typename super::super::super& other) :
            super(other)
    {
    }
    TinyShare(const super& other) :
            super(other)
    {
    }

    void XOR(const This& a, const This& b)
    {
        *this = a + b;
    }

    void public_input(bool input)
    {
        auto& party = get_party();
        *this = super::constant(input, party.P->my_num(),
                party.MC->get_alphai());
    }

    void random()
    {
        TinySecret<S> tmp;
        get_party().DataF.get_one(DATA_BIT, tmp);
        *this = tmp.get_reg(0);
    }
};

} /* namespace GC */

#endif /* GC_TINYSHARE_H_ */
