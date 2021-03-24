#pragma once

// T == Share<gfp_<0, n_limbs>>
template <class T>
class Complex
{
public:
  T real;
  T imag;
  Complex() {}
  Complex(T &_real, T &_imag) : real(_real), imag(_imag) {}
  void setValue(T &_real, T &_imag)
  {
    real = _real;
    imag = _imag;
  }
};

template <class T>
class Complex_Plain
{
  typedef typename T::clear clear;

public:
  clear real;
  clear imag;
  Complex_Plain() {}
  Complex_Plain(clear &_real, clear &_imag) : real(_real), imag(_imag) {}
  void setValue(clear &_real, clear &_imag)
  {
    real = _real;
    imag = _imag;
  }
};
