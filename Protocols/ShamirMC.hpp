/*
 * ShamirMC.cpp
 *
 */

#ifndef PROTOCOLS_SHAMIRMC_HPP_
#define PROTOCOLS_SHAMIRMC_HPP_

#include "ShamirMC.h"

template<class T>
ShamirMC<T>::~ShamirMC()
{
    if (os)
        delete os;
}

template<class T>
void ShamirMC<T>::POpen_Begin(vector<typename T::open_type>& values,
        const vector<T>& S, const Player& P)
{
    (void) values;
    prepare(S, P);
    P.send_all(os->mine);
}

template<class T>
void ShamirMC<T>::init_open(const Player& P, int n)
{
    int n_relevant_players = ShamirMachine::s().threshold + 1;
    if (reconstruction.empty())
    {
        reconstruction.resize(n_relevant_players);
        for (int i = 0; i < n_relevant_players; i++)
            reconstruction[i] = Shamir<T>::get_rec_factor(P.get_player(i),
                    P.num_players(), P.my_num(), n_relevant_players);
    }

    if (not os)
        os = new Bundle<octetStream>(P);

    for (auto& o : *os)
        o.reset_write_head();
    os->mine.reserve(n * T::size());
    this->player = &P;
}

template<class T>
void ShamirMC<T>::prepare(const vector<T>& S, const Player& P)
{
    init_open(P, S.size());
    for (auto& share : S)
        prepare_open(share);
}

template<class T>
void ShamirMC<T>::prepare_open(const T& share)
{
    share.pack(os->mine);
}

template<class T>
void ShamirMC<T>::POpen(vector<typename T::open_type>& values, const vector<T>& S,
        const Player& P)
{
    prepare(S, P);
    exchange(P);
    finalize(values, S);
}

template<class T>
void ShamirMC<T>::exchange(const Player& P)
{
    vector<bool> my_senders(P.num_players()), my_receivers(P.num_players());
    for (int i = 0; i < P.num_players(); i++)
    {
        my_senders[i] = P.get_offset(i) <= threshold;
        my_receivers[i] = P.get_offset(i) >= P.num_players() - threshold;
    }
    P.partial_broadcast(my_senders, my_receivers, *os);
}

template<class T>
void ShamirMC<T>::POpen_End(vector<typename T::open_type>& values,
        const vector<T>& S, const Player& P)
{
    P.receive_all(*os);
    finalize(values, S);
}

template<class T>
void ShamirMC<T>::finalize(vector<typename T::open_type>& values,
        const vector<T>& S)
{
    values.clear();
    for (size_t i = 0; i < S.size(); i++)
        values.push_back(finalize_open());
}

template<class T>
typename T::open_type ShamirMC<T>::finalize_open()
{
    assert(reconstruction.size());
    typename T::open_type res;
    for (size_t j = 0; j < reconstruction.size(); j++)
    {
        res +=
                (*os)[player->get_player(j)].template get<typename T::open_type>()
                        * reconstruction[j];
    }

    return res;
}

#endif
