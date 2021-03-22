/*
 * Integer.cpp
 *
 */

#include "Integer.h"

template<class T>
const int IntBase<T>::N_BITS;

template<class T>
void IntBase<T>::output(ostream& s,bool human) const
{
  if (human)
    s << a;
  else
    s.write((char*)&a, sizeof(a));
}

template<class T>
void IntBase<T>::input(istream& s,bool human)
{
  if (human)
    s >> a;
  else
    s.read((char*)&a, sizeof(a));
}

inline
void Integer::reqbl(int n)
{
  if ((int)n < 0 && size() * 8 != -(int)n)
    {
      throw Processor_Error(
          "Program compiled for rings of length " + to_string(-(int)n)
          + " but VM supports only "
          + to_string(size() * 8));
    }
  else if ((int)n > 0)
    {
      throw Processor_Error("Program compiled for fields not rings");
    }
}

inline
Integer::Integer(const Integer& x, int n_bits)
{
  if (n_bits == 1)
    *this = x & 1;
  else if (n_bits == 64)
    *this = x;
  else
    {
      a = abs(x.get());
      a &= ~(uint64_t(-1) << (n_bits - 1) << 1);
      if (x < 0)
        a = -a;
    }
}
