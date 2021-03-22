/*
 * gf2nSquare.cpp
 *
 */

#include "Math/Square.h"

template<class U>
Square<U>& Square<U>::sub(const Square<U>& other)
{
    for (int i = 0; i < U::length(); i++)
        rows[i] -= other.rows[i];
    return *this;
}

template<class U>
Square<U>& Square<U>::rsub(const Square<U>& other)
{
    for (int i = 0; i < U::length(); i++)
        rows[i] = other.rows[i] - rows[i];
    return *this;
}

template<class U>
Square<U>& Square<U>::sub(const void* other)
{
    U value;
    value.assign(other);
    for (int i = 0; i < U::length(); i++)
        rows[i] -= value;
    return *this;
}

template<class U>
void Square<U>::conditional_add(BitVector& conditions,
        Square<U>& other, int offset)
{
    for (int i = 0; i < U::length(); i++)
        if (conditions.get_bit(N_ROWS * offset + i))
            rows[i] += other.rows[i];
}

template<class U>
void Square<U>::pack(octetStream& os) const
{
    for (int i = 0; i < U::length(); i++)
        rows[i].pack(os);
}

template<class U>
void Square<U>::unpack(octetStream& os)
{
    for (int i = 0; i < U::length(); i++)
        rows[i].unpack(os);
}

template<class U>
void Square<U>::to(U& result)
{
    to(result, U::prime_field);
}

template<class U>
template<int X, int L>
void Square<U>::to(gfp_<X, L>& result, true_type)
{
    mp_limb_t product[2 * L], sum[2 * L], tmp[L][2 * L];
    memset(tmp, 0, sizeof(tmp));
    memset(sum, 0, sizeof(sum));
    for (int i = 0; i < gfp_<X, L>::length(); i++)
    {
        memcpy(&(tmp[i/64][i/64]), &(rows[i]), sizeof(rows[i]));
        if (i % 64 == 0)
            memcpy(product, tmp[i/64], sizeof(product));
        else
            mpn_lshift(product, tmp[i/64], 2 * L, i % 64);
        mpn_add_fixed_n<2 * L>(sum, product, sum);
    }
    mp_limb_t q[2 * L], ans[2 * L];
    mpn_tdiv_qr(q, ans, 0, sum, 2 * L, gfp_<X, L>::get_ZpD().get_prA(), L);
    result.assign((void*) ans);
}
