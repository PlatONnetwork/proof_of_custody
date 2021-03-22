/*
 * TinierSharePrep.cpp
 *
 */

#ifndef GC_TINIERSHARE_PREP_HPP_
#define GC_TINIERSHARE_PREP_HPP_

#include "TinierSharePrep.h"

#include "PersonalPrep.hpp"

namespace GC
{

template<class T>
TinierSharePrep<T>::TinierSharePrep(DataPositions& usage, int input_player) :
        PersonalPrep<T>(usage, input_player), triple_generator(0),
        whole_prep(usage,
                ShareThread<TinierSecret<typename T::mac_key_type>>::s(),
                input_player == PersonalPrep<T>::SECURE)
{
}

template<class T>
TinierSharePrep<T>::TinierSharePrep(SubProcessor<T>*, DataPositions& usage) :
        TinierSharePrep(usage)
{
}

template<class T>
TinierSharePrep<T>::~TinierSharePrep()
{
    if (triple_generator)
        delete triple_generator;
}

template<class T>
void TinierSharePrep<T>::set_protocol(typename T::Protocol& protocol)
{
    if (triple_generator)
        return;

    params.generateMACs = true;
    params.amplify = false;
    params.check = false;
    auto& thread = ShareThread<TinierSecret<typename T::mac_key_type>>::s();
    triple_generator = new typename T::TripleGenerator(
            BaseMachine::s().fresh_ot_setup(), protocol.P.N, -1,
            OnlineOptions::singleton.batch_size
                    * TinierSecret<typename T::mac_key_type>::default_length, 1,
            params, thread.MC->get_alphai(), &protocol.P);
    triple_generator->multi_threaded = false;
    this->inputs.resize(thread.P->num_players());
    whole_prep.init(*thread.P);
}

template<class T>
void TinierSharePrep<T>::buffer_triples()
{
    if (this->input_player != this->SECURE)
    {
        this->buffer_personal_triples();
        return;
    }

    array<TinierSecret<typename T::mac_key_type>, 3> whole;
    whole_prep.get(DATA_TRIPLE, whole.data());
    for (size_t i = 0; i < whole[0].get_regs().size(); i++)
        this->triples.push_back(
        {{ whole[0].get_reg(i), whole[1].get_reg(i), whole[2].get_reg(i) }});
}

template<class T>
void TinierSharePrep<T>::buffer_inputs(int player)
{
    auto& inputs = this->inputs;
    assert(triple_generator);
    triple_generator->generateInputs(player);
    for (auto& x : triple_generator->inputs)
        inputs.at(player).push_back(x);
}

template<class T>
size_t TinierSharePrep<T>::data_sent()
{
    size_t res = whole_prep.data_sent();
    if (triple_generator)
        res += triple_generator->data_sent();
    return res;
}

}

#endif
