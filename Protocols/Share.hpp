#ifndef PROTOCOLS_SHARE_H_
#define PROTOCOLS_SHARE_H_

#include "Share.h"


template<class T, class V>
template<class U>
void Share_<T, V>::read_or_generate_mac_key(string directory, const Player& P,
        U& key)
{
    try
    {
        read_mac_key(directory, P.N, key);
    }
    catch (mac_key_error&)
    {
#ifdef VERBOSE
        cerr << "Generating fresh MAC key" << endl;
#endif
        SeededPRNG G;
        key.randomize(G);
    }
}

template<class T, class V>
inline
void Share_<T, V>::mul_by_bit(const Share_<T, V>& S,const clear& aa)
{
  a.mul(S.a,aa);
  mac.mul(S.mac,aa);
}

template<>
inline
void Share_<SemiShare<gf2n>, SemiShare<gf2n>>::mul_by_bit(
    const Share_<SemiShare<gf2n>, SemiShare<gf2n>>& S, const gf2n& aa)
{
  a.mul_by_bit(S.a,aa);
  mac.mul_by_bit(S.mac,aa);
}

template<class T, class V>
inline void Share_<T, V>::pack(octetStream& os, bool full) const
{
  a.pack(os);
  if (full)
    mac.pack(os);
}

template<class T, class V>
inline void Share_<T, V>::unpack(octetStream& os, bool full)
{
  a.unpack(os);
  if (full)
    mac.unpack(os);
}

#endif
