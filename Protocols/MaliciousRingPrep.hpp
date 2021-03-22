/*
 * MaliciousRingPrep.hpp
 *
 */

#ifndef PROTOCOLS_MALICIOUSRINGPREP_HPP_
#define PROTOCOLS_MALICIOUSRINGPREP_HPP_

#include "ReplicatedPrep.h"

#include "DabitSacrifice.hpp"
#include "Spdz2kPrep.hpp"

template<class T>
void MaliciousDabitOnlyPrep<T>::buffer_dabits(ThreadQueues* queues)
{
    buffer_dabits<0>(queues, T::clear::characteristic_two);
}

template<class T>
template<int>
void MaliciousDabitOnlyPrep<T>::buffer_dabits(ThreadQueues*, true_type)
{
    throw runtime_error("only implemented for integer-like domains");
}

template<class T>
template<int>
void MaliciousDabitOnlyPrep<T>::buffer_dabits(ThreadQueues* queues, false_type)
{
    assert(this->proc != 0);
    vector<dabit<T>> check_dabits;
    DabitSacrifice<T> dabit_sacrifice;
    this->buffer_dabits_without_check(check_dabits,
            dabit_sacrifice.minimum_n_inputs(), queues);
    dabit_sacrifice.sacrifice_and_check_bits(this->dabits, check_dabits,
            *this->proc, queues);
}

#endif /* PROTOCOLS_MALICIOUSRINGPREP_HPP_ */
