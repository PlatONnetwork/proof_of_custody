/*
 * Bit.cpp
 *
 */

#include "Bit.h"
#include "gf2n.h"

Bit::Bit(const gf2n_short& other) :
        super(other.get_bit(0))
{
    assert(other.is_one() or other.is_zero());
}
