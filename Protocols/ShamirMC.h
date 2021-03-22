/*
 * ShamirMC.h
 *
 */

#ifndef PROTOCOLS_SHAMIRMC_H_
#define PROTOCOLS_SHAMIRMC_H_

#include "MAC_Check_Base.h"
#include "Protocols/ShamirShare.h"
#include "Machines/ShamirMachine.h"
#include "Tools/Bundle.h"

template<class T>
class ShamirMC : public MAC_Check_Base<T>
{
    vector<typename T::open_type::Scalar> reconstruction;

    void finalize(vector<typename T::open_type>& values, const vector<T>& S);

protected:
    Bundle<octetStream>* os;
    const Player* player;
    int threshold;

    void prepare(const vector<T>& S, const Player& P);

public:
    ShamirMC() : os(0), player(0), threshold(ShamirMachine::s().threshold) {}

    // emulate MAC_Check
    ShamirMC(const typename T::mac_key_type& _, int __ = 0, int ___ = 0) : ShamirMC()
    { (void)_; (void)__; (void)___; }

    // emulate Direct_MAC_Check
    ShamirMC(const typename T::mac_key_type& _, Names& ____, int __ = 0, int ___ = 0) :
        ShamirMC()
    { (void)_; (void)__; (void)___; (void)____; }

    virtual ~ShamirMC();

    void POpen(vector<typename T::open_type>& values,const vector<T>& S,const Player& P);
    void POpen_Begin(vector<typename T::open_type>& values,const vector<T>& S,const Player& P);
    void POpen_End(vector<typename T::open_type>& values,const vector<T>& S,const Player& P);

    virtual void init_open(const Player& P, int n = 0);
    virtual void prepare_open(const T& secret);
    virtual void exchange(const Player& P);
    virtual typename T::open_type finalize_open();

    void Check(const Player& P) { (void)P; }
};

#endif /* PROTOCOLS_SHAMIRMC_H_ */
