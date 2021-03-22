/*
 * SemiPrep.h
 *
 */

#ifndef PROTOCOLS_SEMIPREP_H_
#define PROTOCOLS_SEMIPREP_H_

#include "MascotPrep.h"

template<class T>
class SemiPrep : public virtual OTPrep<T>, public virtual SemiHonestRingPrep<T>
{
public:
    SemiPrep(SubProcessor<T>* proc, DataPositions& usage);

    void buffer_triples();
};

#endif /* PROTOCOLS_SEMIPREP_H_ */
