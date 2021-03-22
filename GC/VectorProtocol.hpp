/*
 * VectorProtocol.cpp
 *
 */

#include "VectorProtocol.h"

namespace GC
{

template<class T>
VectorProtocol<T>::VectorProtocol(Player& P) :
        part_protocol(P)
{
}

template<class T>
void VectorProtocol<T>::init_mul(SubProcessor<T>* proc)
{
    assert(proc);
    init_mul(proc->DataF, proc->MC);
}

template<class T>
void VectorProtocol<T>::init_mul(Preprocessing<T>& prep,
        typename T::MAC_Check& MC)
{
    part_protocol.init_mul(prep.get_part(), MC.get_part_MC());
}

template<class T>
typename T::clear VectorProtocol<T>::prepare_mul(const T& x,
        const T& y, int n)
{
    for (int i = 0; i < n; i++)
        part_protocol.prepare_mul(x.get_reg(i), y.get_reg(i), 1);
    return {};
}

template<class T>
void VectorProtocol<T>::exchange()
{
    part_protocol.exchange();
}

template<class T>
T VectorProtocol<T>::finalize_mul(int n)
{
    T res;
    res.resize_regs(n);
    for (int i = 0; i < n; i++)
        res.get_reg(i) = part_protocol.finalize_mul(1);
    return res;
}

} /* namespace GC */
