#ifndef _DiscreteGauss
#define _DiscreteGauss

/* Class to sample from a Discrete Gauss distribution of 
   standard deviation R
*/

#include <FHE/Generator.h>
#include "Tools/random.h"
#include <vector>
#include <math.h>

class DiscreteGauss
{
  /* This is the bound we use on for the NewHope approximation
   * to a discrete Gaussian with sigma=sqrt(B/2)
   */
  int NewHopeB;

  public:

  void set(double R);

  void pack(octetStream& o) const { o.serialize(NewHopeB); }
  void unpack(octetStream& o) { o.unserialize(NewHopeB); }

  DiscreteGauss(double R) { set(R); }
  ~DiscreteGauss()        { ; }

  // Rely on default copy constructor/assignment
  
  int sample(PRNG& G, int stretch = 1) const;
  double get_R() const { return sqrt(0.5 * NewHopeB); }
  int get_NewHopeB() const { return NewHopeB; }

  bool operator!=(const DiscreteGauss& other) const;
};


/* Sample from integer lattice of dimension n 
 * with standard deviation R
 */
class RandomVectors
{
  int n,h;
  DiscreteGauss DG;  // This generates the main distribution

  public:

  void set(int nn,int hh,double R);  // R is input STANDARD DEVIATION
  void set_n(int nn);

  void pack(octetStream& o) const { o.store(n); o.store(h); DG.pack(o); }
  void unpack(octetStream& o)
  { o.get(n); o.get(h); DG.unpack(o); }

  RandomVectors(int h, double R) : RandomVectors(0, h, R) {}
  RandomVectors(int nn,int hh,double R) : DG(R) { set(nn,hh,R);  }
  ~RandomVectors()        { ; }

  // Rely on default copy constructor/assignment

  double get_R() const { return DG.get_R(); }
  DiscreteGauss get_DG() const { return DG; }
  int get_h() const { return h; }
  
  // Sample from Discrete Gauss distribution
  vector<bigint> sample_Gauss(PRNG& G, int stretch = 1) const;

  // Next samples from Hwt distribution unless hwt>n/2 in which 
  // case it uses Gauss
  vector<bigint> sample_Hwt(PRNG& G) const;

  // Sample from {-1,0,1} with Pr(-1)=Pr(1)=1/4 and Pr(0)=1/2
  vector<bigint> sample_Half(PRNG& G) const;

  // Sample from (-B,0,B) with uniform prob
  vector<bigint> sample_Uniform(PRNG& G,const bigint& B) const;

  bool operator!=(const RandomVectors& other) const;
};

template<class T>
class RandomGenerator : public Generator<T>
{
protected:
  mutable PRNG G;

public:
  RandomGenerator(PRNG& G) { this->G.SetSeed(G); }
};

template<class T>
class UniformGenerator : public RandomGenerator<T>
{
  int n_bits;
  bool positive;

public:
  UniformGenerator(PRNG& G, int n_bits, bool positive = true) :
    RandomGenerator<T>(G), n_bits(n_bits), positive(positive) {}
  Generator<T>* clone() const { return new UniformGenerator<T>(*this); }
  void get(T& x) const  { this->G.get(x, n_bits, positive); }
};

template<class T>
class GaussianGenerator : public RandomGenerator<T>
{
  DiscreteGauss DG;
  int stretch;

public:
  GaussianGenerator(const DiscreteGauss& DG, PRNG& G, int stretch = 1) :
    RandomGenerator<T>(G), DG(DG), stretch(stretch) {}
  Generator<T>* clone() const { return new GaussianGenerator<T>(*this); }
  void get(T& x) const { x = DG.sample(this->G, stretch); }
};

int sample_half(PRNG& G);

template<class T>
class HalfGenerator : public RandomGenerator<T>
{
public:
  HalfGenerator(PRNG& G) :
    RandomGenerator<T>(G) {}
  Generator<T>* clone() const { return new HalfGenerator<T>(*this); }
  void get(T& x) const { x = sample_half(this->G); }
};

#endif
