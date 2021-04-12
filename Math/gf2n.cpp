
#include "Math/gf2n.h"
#include "Math/Bit.h"

#include "Tools/Exceptions.h"

#include <stdint.h>
#include <wmmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

const false_type ValueInterface::characteristic_two;
const false_type ValueInterface::prime_field;
const false_type ValueInterface::invertible;

const true_type gf2n_short::characteristic_two;
const true_type gf2n_long::characteristic_two;
const true_type gf2n_long::invertible;

int gf2n_short::n = 0;
int gf2n_short::t1;
int gf2n_short::t2;
int gf2n_short::t3;
int gf2n_short::l0;
int gf2n_short::l1;
int gf2n_short::l2;
int gf2n_short::l3;
int gf2n_short::nterms;
word gf2n_short::mask;
bool gf2n_short::useC;

word gf2n_short_table[256][256];

#define num_2_fields 4

/* Require
 *  2*(n-1)-64+t1<64
 */
int fields_2[num_2_fields][4] = { 
	{4,1,0,0},{8,4,3,1},{28,1,0,0},{40,20,15,10}
    };


void gf2n_short::init_tables()
{
  if (sizeof(word)!=8)
    { cout << "Word size is wrong" << endl; 
      throw not_implemented();
    }
  int i,j;
  for (i=0; i<256; i++)
    { for (j=0; j<256; j++)
        { word ii=i,jj=j;
          gf2n_short_table[i][j]=0;
          while (ii!=0)
            { if ((ii&1)==1) { gf2n_short_table[i][j]^=jj; }
              jj<<=1;
              ii>>=1;
            }
         }
    }
}


void gf2n_short::init_field(int nn)
{
  if (nn == 0)
    {
      nn = DEFAULT_LENGTH;
#ifdef VERBOSE
      cerr << "Using GF(2^" << nn << ")" << endl;
#endif
    }

  if (nn == n)
      return;

  assert(n == 0);

  gf2n_short::init_tables();
  int i,j=-1;
  for (i=0; i<num_2_fields && j==-1; i++)
    { if (nn==fields_2[i][0]) { j=i; } }
  if (j==-1)
    {
      if (nn == 128)
	throw runtime_error("need to compile with USE_GF2N_LONG = 1; "
			    "remember to make clean");
      else
	throw runtime_error("field size not supported");
    }

  n=nn;
  nterms=1;
  l0=64-n;  
  t1=fields_2[j][1];
  l1=64+t1-n; 
  if (fields_2[j][2]!=0)
    { nterms=3;
      t2=fields_2[j][2];
      l2=64+t2-n; 
      t3=fields_2[j][3];
      l3=64+t3-n; 
    }
  if (2*(n-1)-64+t1>=64) { throw invalid_params(); }

  mask=(1ULL<<n)-1;

#ifdef __PCLMUL__
  useC = not cpu_has_pclmul();
#else
  useC = true;
#endif
}
  


/* Takes 16bit x and y and returns the 32 bit product in c1 and c0
      ans = (c1<<16)^c0
   where c1 and c0 are 16 bit
*/
inline void mul16(word x,word y,word& c0,word& c1)
{
  word a1=x&(0xFF), b1=y&(0xFF);
  word a2=x>>8,     b2=y>>8;

  c0=gf2n_short_table[a1][b1];
  c1=gf2n_short_table[a2][b2];
  word te=gf2n_short_table[a1][b2]^gf2n_short_table[a2][b1];
  c0^=(te&0xFF)<<8;
  c1^=te>>8;
}

/* Takes 16 bit x and y and returns the 32 bit product */
inline word mul16(word x,word y)
{
  word a1=x&(0xFF), b1=y&(0xFF);
  word a2=x>>8,     b2=y>>8;

  word ans=gf2n_short_table[a2][b2]<<8;
  ans^=gf2n_short_table[a1][b2]^gf2n_short_table[a2][b1];
  ans<<=8;
  ans^=gf2n_short_table[a1][b1];

  return ans;
}



void gf2n_short::reduce_trinomial(word xh,word xl)
{
  // Deal with xh first
  a=xl;
  a^=(xh<<l0); 
  a^=(xh<<l1); 

  // Now deal with last word
  word hi=a>>n;
  while (hi!=0)
    { a&=mask;

      a^=hi;
      a^=(hi<<t1);
      hi=a>>n;
    }
}

void gf2n_short::reduce_pentanomial(word xh,word xl)
{
  // Deal with xh first
  a=xl;
  a^=(xh<<l0);   
  a^=(xh<<l1);
  a^=(xh<<l2);
  a^=(xh<<l3);

  // Now deal with last word
  word hi=a>>n;
  while (hi!=0)
    { a&=mask;

      a^=hi;
      a^=(hi<<t1);
      a^=(hi<<t2);
      a^=(hi<<t3);

      hi=a>>n;
    }
}


void mul32(word x,word y,word& ans)
{
  word a1=x&(0xFFFF),b1=y&(0xFFFF);
  word a2=x>>16,     b2=y>>16;

  word c0,c1;

  ans=mul16(a1,b1);
  word upp=mul16(a2,b2);

  mul16(a1,b2,c0,c1);
  ans^=c0<<16;       upp^=c1;

  mul16(a2,b1,c0,c1);
  ans^=c0<<16;       upp^=c1;

  ans^=(upp<<32);
}


void mul64(word x, word y, word& lo, word& hi)
{
   word c,d,e,t;
   word xl=x&0xFFFFFFFF,yl=y&0xFFFFFFFF;
   word xh=x>>32,yh=y>>32;
   mul32(xl,yl,c);
   mul32(xh,yh,d);
   mul32((xl^xh),(yl^yh),e);
   t=c^e^d;
   lo=c^(t<<32);
   hi=d^(t>>32);
}


void gf2n_short::mul(const gf2n_short& x,const gf2n_short& y)
{
  word hi,lo;
  
  if (gf2n_short::useC)
    { /* Uses Karatsuba */
      mul64(x.a, y.a, lo, hi);
    }
  else
    { /* Use Intel Instructions */
#ifdef __PCLMUL__
      __m128i xx,yy,zz;
      uint64_t c[] __attribute__((aligned (16))) = { 0,0 };
      xx=_mm_set1_epi64x(x.a);
      yy=_mm_set1_epi64x(y.a);
      zz=_mm_clmulepi64_si128(xx,yy,0);
      _mm_store_si128((__m128i*)c,zz);
      lo=c[0];
      hi=c[1];
#else
      throw runtime_error("need to compile with PCLMUL support");
#endif
    }

  reduce(hi,lo);
}

gf2n_short gf2n_short::operator*(const Bit& x) const
{
  return x.get() * a;
}


gf2n_short gf2n_short::invert() const
{
  if (is_one())  { return *this; }
  if (is_zero()) { throw division_by_zero(); }

  word u,v=a,B=0,D=1,mod=1;

  mod^=(1ULL<<n);
  mod^=(1ULL<<t1);
  if (nterms==3)
    { mod^=(1ULL<<t2);
      mod^=(1ULL<<t3);
    }
  u=mod; v=a;
  
  while (u!=0)
    { while ((u&1)==0)
	    { u>>=1;
          if ((B&1)!=0) { B^=mod; }
 	      B>>=1;
        }
      while ((v&1)==0 && v!=0)
	    { v>>=1;
	      if ((D&1)!=0) { D^=mod; }
		  D>>=1;
	    }

      if (u>=v) { u=u^v; B=B^D; }
	  else      { v=v^u; D=D^B; }
   }

  return D;
}


void gf2n_short::randomize(PRNG& G, int n)
{
  (void) n;
  a=G.get_uint();
  a=(a<<32)^G.get_uint();
  a&=mask;
}


void gf2n_short::output(ostream& s,bool human) const
{
  if (human)
    { s << hex << showbase << a << dec << " "; }
  else
    { s.write((char*) &a,sizeof(word)); }
}

void gf2n_short::input(istream& s,bool human)
{
  if (s.peek() == EOF)
    { if (s.tellg() == 0)
        { cout << "IO problem. Empty file?" << endl;
          throw file_error("gf2n_short input");
        }
      throw end_of_file("gf2n_short");
    }

  if (human)
    { s >> hex >> a >> dec; } 
  else
    { s.read((char*) &a,sizeof(word)); }

  a &= mask;
}

// Expansion is by x=y^5+1 (as we embed GF(256) into GF(2^40)
void expand_byte(gf2n_short& a,int b)
{
  gf2n_short x,xp;
  x = (32+1);
  xp.assign_one();
  a.assign_zero();

  while (b!=0)
    { if ((b&1)==1)
        { a.add(a,xp); }
      xp *= (x);
      b>>=1;
    }
}


// Have previously worked out the linear equations we need to solve
void collapse_byte(int& b,const gf2n_short& aa)
{
  word w=aa.get();
  int e35=(w>>35)&1;
  int e30=(w>>30)&1;
  int e25=(w>>25)&1;
  int e20=(w>>20)&1;
  int e15=(w>>15)&1;
  int e10=(w>>10)&1;
  int  e5=(w>>5)&1;
  int  e0=w&1;
  int a[8];
  a[7]=e35;
  a[6]=e30^a[7];
  a[5]=e25^a[7];
  a[4]=e20^a[5]^a[6]^a[7];
  a[3]=e15^a[7];
  a[2]=e10^a[3]^a[6]^a[7];
  a[1]=e5^a[3]^a[5]^a[7];
  a[0]=e0^a[1]^a[2]^a[3]^a[4]^a[5]^a[6]^a[7];

  b=0;
  for (int i=7; i>=0; i--)
    { b=b<<1;
      b+=a[i];
    }
}
