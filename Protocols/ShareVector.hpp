/*
 * ShareVector.cpp
 *
 */

#include "ShareVector.h"
#include "FHE/FFT.h"

template<class U>
void ShareVector<U>::fft(const FFT_Data& fftd)
{
    array<vector<modp>, 2> data;
    for (auto& share : *this)
    {
        data[0].push_back(share.get_share());
        data[1].push_back(share.get_mac());
    }

    for (auto& x : data)
    {
        if (fftd.get_twop() == 0)
            FFT_Iter2(x, fftd.phi_m(), fftd.get_root(0), fftd.get_prD());
        else
            FFT_non_power_of_two(x, x, fftd);
    }

    for (int i = 0; i < fftd.phi_m(); i++)
    {
        (*this)[i] = {data[0][i], data[1][i]};
    }
}
