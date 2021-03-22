/*
 * ShamirMachine.h
 *
 */

#ifndef MACHINES_SHAMIRMACHINE_H_
#define MACHINES_SHAMIRMACHINE_H_

#include "Tools/ezOptionParser.h"

class ShamirOptions
{
public:
    static ShamirOptions singleton;
    static ShamirOptions& s();

    int nparties;
    int threshold;

    ShamirOptions();
    ShamirOptions(ez::ezOptionParser& opt, int argc, const char** argv);
};

class ShamirMachine : public ShamirOptions
{
};

template<template<class U> class T>
class ShamirMachineSpec
{
public:
    ShamirMachineSpec(int argc, const char** argv);
};

#endif /* MACHINES_SHAMIRMACHINE_H_ */
