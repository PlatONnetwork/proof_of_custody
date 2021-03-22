/*
 * fixint.h
 *
 */

#ifndef MATH_FIXINT_H_
#define MATH_FIXINT_H_

#include "Z2k.h"

template<int L>
class fixint : public SignedZ2<64 * (L + 1)>
{
public:
    typedef SignedZ2<64 * (L + 1)> super;

    fixint()
    {
    }

    template<class T>
    fixint(const T& other) :
            super(other)
    {
        char check = this->a[this->N_WORDS - 1] >> 56;
        assert(check == 0 or check == -1);
    }

    fixint operator-(const fixint& other) const
    {
        return super::operator-(other);
    }

    fixint operator-() const
    {
        return super::operator-();
    }

    fixint operator^(const fixint& other) const
    {
        assert(L == 0);
        return fixint(this->a[0] ^ other.a[0]);
    }

    void generateUniform(PRNG& G, int n_bits, bool positive = true)
    {
        G.get(bigint::tmp, n_bits, positive);
        *this = bigint::tmp;
    }

    void randomBnd(PRNG& G, const bigint& bound, bool positive)
    {
        G.randomBnd(bigint::tmp, bound, positive);
        *this = bigint::tmp;
    }

    int get_min_alloc() const
    {
        return this->N_BYTES;
    }

    size_t report_size(int) const
    {
        return this->N_BYTES;
    }

    template<class T>
    void allocate_slots(const T& limit)
    {
        int n_bits = this->size_in_bits();
        if (numBits(limit) - 56 > n_bits)
        {
        cerr << "cannot hold " << numBits(limit) << " bits, " << n_bits
                << " available" << endl;
            throw runtime_error("fixed-length integer too small");
        }
    }
};

template<int L>
bigint operator-(const bigint& x, const fixint<L>& y)
{
    return x - (bigint::tmp = y);
}

template<int L>
bigint& operator+=(bigint& x, const fixint<L>& y)
{
    x += (bigint::tmp = y);
    return x;
}

template<int L>
bigint operator%(const fixint<L>& x, const bigint& y)
{
    return (bigint::tmp = x) % y;
}

template<int L>
fixint<L>& operator%=(fixint<L>& x, const bigint& y)
{
    return x = (bigint::tmp = x) % y;
}

#endif /* MATH_FIXINT_H_ */
