/*
 * SpdzWiseShare.hpp
 *
 */

#ifndef PROTOCOLS_SPDZWISESHARE_HPP_
#define PROTOCOLS_SPDZWISESHARE_HPP_

#include "SpdzWiseShare.h"

#include "fake-stuff.hpp"

template<class T>
void SpdzWiseShare<T>::read_or_generate_mac_key(string directory, Player& P, T& mac_key)
{
    try
    {
        read_mac_key(directory, P.N, mac_key);
    }
    catch (mac_key_error&)
    {
        SeededPRNG G;
        mac_key.randomize(G);
    }

    try
    {
        // validate MAC key
        typename open_part_type::MAC_Check MC;
        auto masked = typename T::Honest::Protocol(P).get_random() + mac_key;
        MC.open(masked, P);
        MC.Check(P);
    }
    catch (mac_fail&)
    {
#ifdef VERBOSE
        cerr << "Generating fresh MAC key for " << type_string() << endl;
#endif
        mac_key = typename T::Honest::Protocol(P).get_random();
    }
}

#endif /* PROTOCOLS_SPDZWISESHARE_HPP_ */
