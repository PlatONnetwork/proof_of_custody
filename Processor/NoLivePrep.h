/*
 * NoLivePrep.h
 *
 */

#ifndef PROCESSOR_NOLIVEPREP_H_
#define PROCESSOR_NOLIVEPREP_H_

#include "Tools/Exceptions.h"
#include "Data_Files.h"

template<class T> class SubProcessor;
class DataPositions;

template<class T>
class NoLivePrep : public Sub_Data_Files<T>
{
public:
    static void basic_setup(Player&)
    {
    }
    static void teardown()
    {
    }

    NoLivePrep(SubProcessor<T>* proc, DataPositions& usage) : Sub_Data_Files<T>(0, 0, "", usage, 0)
    {
        (void) proc;
        throw not_implemented();
    }
    template<class U>
    NoLivePrep(DataPositions& usage, U& _) : NoLivePrep(0, usage)
    {
        (void) _;
    }
    NoLivePrep(DataPositions& usage) : NoLivePrep(0, usage)
    {
    }
};

#endif /* PROCESSOR_NOLIVEPREP_H_ */
