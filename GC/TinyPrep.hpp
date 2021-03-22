/*
 * TinyPrep.cpp
 *
 */

#include "TinyPrep.h"

#include "Protocols/MascotPrep.hpp"

namespace GC
{

template<class T>
TinyPrep<T>::TinyPrep(DataPositions& usage, ShareThread<T>& thread,
        bool amplify) :
        BufferPrep<T>(usage), thread(thread), triple_generator(0),
        amplify(amplify)
{

}

template<class T>
TinyOnlyPrep<T>::TinyOnlyPrep(DataPositions& usage, ShareThread<T>& thread) :
        TinyPrep<T>(usage, thread), input_generator(0)
{
}

template<class T>
TinyPrep<T>::~TinyPrep()
{
    if (triple_generator)
        delete triple_generator;
}

template<class T>
TinyOnlyPrep<T>::~TinyOnlyPrep()
{
    if (input_generator)
        delete input_generator;
}

template<class T>
void TinyPrep<T>::set_protocol(Beaver<T>& protocol)
{
    init(protocol.P);
}

template<class T>
void TinyPrep<T>::init(Player& P)
{
    params.generateMACs = true;
    params.amplify = false;
    params.check = false;
    auto& thread = ShareThread<T>::s();
    triple_generator = new typename T::TripleGenerator(
            BaseMachine::s().fresh_ot_setup(), P.N, -1,
            OnlineOptions::singleton.batch_size, 1, params,
            thread.MC->get_alphai(), &P);
    triple_generator->multi_threaded = false;
}

template<class T>
void TinyOnlyPrep<T>::set_protocol(Beaver<T>& protocol)
{
    TinyPrep<T>::set_protocol(protocol);
    input_generator = new typename T::part_type::TripleGenerator(
            BaseMachine::s().fresh_ot_setup(), protocol.P.N, -1,
            OnlineOptions::singleton.batch_size, 1, this->params,
            this->thread.MC->get_alphai(), &protocol.P);
    input_generator->multi_threaded = false;
}

template<class T>
void TinyPrep<T>::buffer_triples()
{
    auto& triple_generator = this->triple_generator;
    assert(triple_generator != 0);
    params.generateBits = false;
    vector<array<typename T::check_type, 3>> triples;
    TripleShuffleSacrifice<typename T::check_type> sacrifice;
    size_t required;
    if (amplify)
        required = sacrifice.minimum_n_inputs_with_combining();
    else
        required = sacrifice.minimum_n_inputs();
    while (triples.size() < required)
    {
        triple_generator->generatePlainTriples();
        triple_generator->unlock();
        assert(triple_generator->plainTriples.size() != 0);
        for (size_t i = 0; i < triple_generator->plainTriples.size(); i++)
            triple_generator->valueBits[2].set_portion(i,
                    triple_generator->plainTriples[i][2]);
        triple_generator->run_multipliers({});
        for (size_t i = 0; i < triple_generator->plainTriples.size(); i++)
        {
            for (int j = 0; j < T::default_length; j++)
            {
                triples.push_back({});
                for (int k = 0; k < 3; k++)
                {
                    auto& share = triples.back()[k];
                    share.set_share(
                            triple_generator->plainTriples.at(i).at(k).get_bit(
                                    j));
                    typename T::part_type::mac_type mac;
                    mac = thread.MC->get_alphai() * share.get_share();
                    for (auto& multiplier : triple_generator->ot_multipliers)
                        mac += multiplier->macs.at(k).at(i * T::default_length + j);
                    share.set_mac(mac);
                }
            }
        }
    }
    sacrifice.triple_sacrifice(triples, triples,
            *thread.P, thread.MC->get_part_MC());
    if (amplify)
        sacrifice.triple_combine(triples, triples, *thread.P,
                thread.MC->get_part_MC());
    for (size_t i = 0; i < triples.size() / T::default_length; i++)
    {
        this->triples.push_back({});
        auto& triple = this->triples.back();
        for (auto& x : triple)
            x.resize_regs(T::default_length);
        for (int j = 0; j < T::default_length; j++)
        {
            auto& source_triple = triples[j + i * T::default_length];
            for (int k = 0; k < 3; k++)
                triple[k].get_reg(j) = source_triple[k];
        }
    }
}

template<class T>
void TinyPrep<T>::buffer_bits()
{
    auto tmp = BufferPrep<T>::get_random_from_inputs(thread.P->num_players());
    for (auto& bit : tmp.get_regs())
    {
        this->bits.push_back({});
        this->bits.back().resize_regs(1);
        this->bits.back().get_reg(0) = bit;
    }
}

template<class T>
void TinyPrep<T>::buffer_inputs_(int player, typename T::InputGenerator* input_generator)
{
    auto& inputs = this->inputs;
    inputs.resize(this->thread.P->num_players());
    assert(input_generator);
    input_generator->generateInputs(player);
    assert(input_generator->inputs.size() >= T::default_length);
    for (size_t i = 0; i < input_generator->inputs.size() / T::default_length; i++)
    {
        inputs[player].push_back({});
        inputs[player].back().share.resize_regs(T::default_length);
        for (int j = 0; j < T::default_length; j++)
        {
            auto& source_input = input_generator->inputs[j
                    + i * T::default_length];
            inputs[player].back().share.get_reg(j) = source_input.share;
            inputs[player].back().value ^= typename T::open_type(
                    source_input.value.get_bit(0)) << j;
        }
    }
}

template<class T>
array<T, 3> TinyPrep<T>::get_triple_no_count(int n_bits)
{
    assert(n_bits > 0);
    while ((unsigned)n_bits > triple_buffer.size())
    {
        array<T, 3> tmp;
        this->get(DATA_TRIPLE, tmp.data());
        for (size_t i = 0; i < tmp[0].get_regs().size(); i++)
        {
            triple_buffer.push_back(
            { {tmp[0].get_reg(i), tmp[1].get_reg(i), tmp[2].get_reg(i)} });
        }
    }

    array<T, 3> res;
    for (int j = 0; j < 3; j++)
        res[j].resize_regs(n_bits);

    for (int i = 0; i < n_bits; i++)
    {
        for (int j = 0; j < 3; j++)
            res[j].get_reg(i) = triple_buffer.back()[j];
        triple_buffer.pop_back();
    }

    return res;
}

template<class T>
size_t TinyPrep<T>::data_sent()
{
    size_t res = 0;
    if (triple_generator)
        res += triple_generator->data_sent();
    return res;
}

template<class T>
size_t TinyOnlyPrep<T>::data_sent()
{
    auto res = TinyPrep<T>::data_sent();
    if (input_generator)
        res += input_generator->data_sent();
    return res;
}

} /* namespace GC */
