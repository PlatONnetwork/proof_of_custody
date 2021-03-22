#ifndef _gf2n
#define _gf2n

#include <stdlib.h>
#include <string.h>

#include <iostream>
using namespace std;

#include "Tools/random.h"

#include "Math/gf2nlong.h"
#include "Math/field_types.h"

class gf2n_short;
class P2Data;
class Bit;
template<class T> class Square;
typedef Square<gf2n_short> gf2n_short_square;

void expand_byte(gf2n_short& a,int b);
void collapse_byte(int& b,const gf2n_short& a);

/* This interface compatible with the gfp interface
 * which then allows us to template the Share
 * data type.
 */


/* 
  Arithmetic in Gf_{2^n} with n<64
*/

class gf2n_short : public ValueInterface
{
  friend class gf2n_long;

  word a;

  static int n,t1,t2,t3,nterms;
  static int l0,l1,l2,l3;
  static word mask;
  static bool useC;

  /* Assign x[0..2*nwords] to a and reduce it...  */
  void reduce_trinomial(word xh,word xl);
  void reduce_pentanomial(word xh,word xl);

  void reduce(word xh,word xl)
   { if (nterms==3)
        { reduce_pentanomial(xh,xl); }
     else
        { reduce_trinomial(xh,xl);   }
   }

  static void init_tables();

  public:

  typedef gf2n_short value_type;
  typedef word internal_type;
  typedef gf2n_short next;
  typedef ::Square<gf2n_short> Square;
  typedef P2Data FD;
  typedef gf2n_short Scalar;

  static const int MAX_N_BITS = 64;
  static const int N_BYTES = sizeof(a);
  static const int DEFAULT_LENGTH = 40;

  static void init_field(int nn = 0);
  static void reset() { n = 0; }
  static int degree() { return n; }
  static int default_degree() { return 40; }
  static int get_nterms() { return nterms; }
  static int get_t(int i) 
    { if (i==0)      { return t1; }
      else if (i==1) { return t2; }
      else if (i==2) { return t3; }
      return -1;
    }

  static DataFieldType field_type() { return DATA_GF2N; }
  static char type_char() { return '2'; }
  static string type_short() { return "2"; }
  static string type_string() { return "gf2n"; }

  static int size() { return sizeof(a); }
  static int size_in_bits() { return sizeof(a) * 8; }

  static int length()         { return n == 0 ? DEFAULT_LENGTH : n; }

  static bool allows(Dtype type) { (void) type; return true; }

  static const true_type invertible;
  static const true_type characteristic_two;

  static gf2n_short cut(int128 x) { return x.get_lower(); }

  static gf2n_short Mul(gf2n_short a, gf2n_short b) { return a * b; }

  word get() const { return a; }
  word get_word() const { return a; }

  const void* get_ptr() const { return &a; }

  void assign_zero()             { a=0; }
  void assign_one()              { a=1; } 
  void assign_x()                { a=2; }
  void assign(const void* aa)    { a = *(word*) aa & mask; }

  void normalize()               { a &= mask; }

  int get_bit(int i) const
    { return (a>>i)&1; }
  void set_bit(int i,unsigned int b)
  { if (b==1)
      { a |= (1UL<<i); }
    else
      { a &= ~(1UL<<i); }
  }
  
  gf2n_short() : a(0)       {}
  gf2n_short(word a) : a(a & mask) {}
  gf2n_short(long a) : gf2n_short(word(a)) {}
  gf2n_short(int a) : gf2n_short(word(unsigned(a))) {}
  gf2n_short(const int128& a) { reduce(a.get_upper(), a.get_lower()); }
  template<class T>
  gf2n_short(IntBase<T> a) : a(a.get()) {}

  int is_zero() const            { return (a==0); }
  int is_one()  const            { return (a==1); }
  bool operator==(const gf2n_short& y) const { return a==y.a; }
  bool operator!=(const gf2n_short& y) const { return a!=y.a; }

  // x+y
  void add(const gf2n_short& x,const gf2n_short& y)
    { a=x.a^y.a; }  
  void add(octet* x)
    { a^=*(word*)(x); }
  void add(octetStream& os)
    { add(os.consume(size())); }
  void sub(const gf2n_short& x,const gf2n_short& y)
    { a=x.a^y.a; }
  // = x * y
  void mul(const gf2n_short& x,const gf2n_short& y);
  // x * y when one of x,y is a bit
  void mul_by_bit(const gf2n_short& x, const gf2n_short& y)   { a = x.a * y.a; }

  gf2n_short lazy_add(const gf2n_short& x) const { return *this + x; }
  gf2n_short lazy_mul(const gf2n_short& x) const { return *this * x; }

  gf2n_short operator+(const gf2n_short& x) const { gf2n_short res; res.add(*this, x); return res; }
  gf2n_short operator*(const gf2n_short& x) const { gf2n_short res; res.mul(*this, x); return res; }
  gf2n_short& operator+=(const gf2n_short& x) { add(*this, x); return *this; }
  gf2n_short& operator*=(const gf2n_short& x) { mul(*this, x); return *this; }
  gf2n_short operator-(const gf2n_short& x) const { gf2n_short res; res.add(*this, x); return res; }
  gf2n_short& operator-=(const gf2n_short& x) { sub(*this, x); return *this; }
  gf2n_short operator/(const gf2n_short& x) const { return *this * x.invert(); }

  gf2n_short operator*(const Bit& x) const;

  gf2n_short invert() const;
  void negate() { return; }

  /* Bitwise Ops */
  gf2n_short operator&(const gf2n_short& x) const { return a & x.a; }
  gf2n_short operator^(const gf2n_short& x) const { return a ^ x.a; }
  gf2n_short operator|(const gf2n_short& x) const { return a | x.a; }
  gf2n_short operator~() const { return ~a; }
  gf2n_short operator<<(int i) const { return a << i; }
  gf2n_short operator>>(int i) const { return a >> i; }

  gf2n_short& operator&=(const gf2n_short& x) { *this = *this & x; return *this; }
  gf2n_short& operator>>=(int i) { *this = *this >> i; return *this; }
  gf2n_short& operator<<=(int i) { *this = *this << i; return *this; }

  /* Crap RNG */
  void randomize(PRNG& G, int n = -1);
  // compatibility with gfp
  void almost_randomize(PRNG& G)        { randomize(G); }

  void force_to_bit() { a &= 1; }

  void output(ostream& s,bool human) const;
  void input(istream& s,bool human);

  friend ostream& operator<<(ostream& s,const gf2n_short& x)
    { s << hex << showbase << x.a << dec;
      return s;
    }
  friend istream& operator>>(istream& s,gf2n_short& x)
    { s >> hex >> x.a >> dec;
      return s;
    }


  // Pack and unpack in native format
  //   i.e. Dont care about conversion to human readable form
  void pack(octetStream& o, int n = -1) const
    { (void) n; o.append((octet*) &a,sizeof(word)); }
  void unpack(octetStream& o, int n = -1)
    { (void) n; o.consume((octet*) &a,sizeof(word)); }
};

#ifdef USE_GF2N_LONG
typedef gf2n_long gf2n;
#else
typedef gf2n_short gf2n;
#endif

#endif
