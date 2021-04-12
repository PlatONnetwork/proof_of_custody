/*
 * gfpvar.cpp
 *
 */

#include "gfpvar.h"
#include "Setup.h"
#include "Protocols/Share.h"

#include "gfp.hpp"

Zp_Data gfpvar::ZpD;

const true_type gfpvar::invertible;
const true_type gfpvar::prime_field;

string gfpvar::type_string()
{
    return "gfpvar";
}

string gfpvar::type_short()
{
    return "p";
}

char gfpvar::type_char()
{
    return 'p';
}

int gfpvar::length()
{
    return ZpD.pr_bit_length;
}

int gfpvar::size()
{
    return ZpD.pr_byte_length;
}

bool gfpvar::allows(Dtype dtype)
{
    return gfp_<0, 0>::allows(dtype);
}

DataFieldType gfpvar::field_type()
{
    return gfp_<0, 0>::field_type();
}

void gfpvar::init_field(bigint prime, bool montgomery)
{
    ZpD.init(prime, montgomery);
    if (ZpD.get_t() > N_LIMBS)
        throw wrong_gfp_size("gfpvar", prime, "MAX_MOD_SZ", ZpD.get_t() * 2);
}

void gfpvar::init_default(int lgp, bool montgomery)
{
    init_field(SPDZ_Data_Setup_Primes(lgp), montgomery);
}

const Zp_Data& gfpvar::get_ZpD()
{
    return ZpD;
}

const bigint& gfpvar::pr()
{
    return ZpD.pr;
}

template<>
void gfpvar::generate_setup<Share<gfpvar>>(string prep_data_prefix,
    int nplayers, int lgp)
{
    generate_prime_setup<Share<gfpvar>>(prep_data_prefix, nplayers, lgp);
}

void gfpvar::check_setup(string dir)
{
    ::check_setup(dir, pr());
}

void gfpvar::write_setup(string dir)
{
    write_online_setup(dir, pr());
}

gfpvar::gfpvar()
{
}

gfpvar::gfpvar(int other)
{
    to_modp(a, other, ZpD);
}

gfpvar::gfpvar(const bigint& other)
{
    to_modp(a, other, ZpD);
}

gfpvar::gfpvar(const modp& other) :
        a(other)
{
}

void gfpvar::assign(const char* buffer)
{
    a.assign(buffer, ZpD.get_t());
}

void gfpvar::assign_zero()
{
    *this = {};
}

void gfpvar::assign_one()
{
    assignOne(a, ZpD);
}

bool gfpvar::is_zero()
{
    return isZero(a, ZpD);
}

bool gfpvar::is_one()
{
    return isOne(a, ZpD);
}

gfpvar::modp_type gfpvar::get() const
{
    return a;
}

gfpvar gfpvar::operator +(const gfpvar& other) const
{
    gfpvar res;
    Add(res.a, a, other.a, ZpD);
    return res;
}

gfpvar gfpvar::operator -(const gfpvar& other) const
{
    gfpvar res;
    Sub(res.a, a, other.a, ZpD);
    return res;
}

gfpvar gfpvar::operator *(const gfpvar& other) const
{
    gfpvar res;
    Mul(res.a, a, other.a, ZpD);
    return res;
}

gfpvar gfpvar::operator /(const gfpvar& other) const
{
    return *this * other.invert();
}

gfpvar& gfpvar::operator +=(const gfpvar& other)
{
    Add(a, a, other.a, ZpD);
    return *this;
}

gfpvar& gfpvar::operator -=(const gfpvar& other)
{
    Sub(a, a, other.a, ZpD);
    return *this;
}

gfpvar& gfpvar::operator *=(const gfpvar& other)
{
    Mul(a, a, other.a, ZpD);
    return *this;
}

bool gfpvar::operator ==(const gfpvar& other) const
{
    return areEqual(a, other.a, ZpD);
}

bool gfpvar::operator !=(const gfpvar& other) const
{
    return not (*this == other);
}

void gfpvar::add(octetStream& other)
{
    *this += other.get<gfpvar>();
}

void gfpvar::negate()
{
    *this = gfpvar() - *this;
}

gfpvar gfpvar::invert() const
{
    gfpvar res;
    Inv(res.a, a, ZpD);
    return res;
}

gfpvar gfpvar::sqrRoot() const
{
    bigint ti = *this;
    ti = sqrRootMod(ti, ZpD.pr);
    if (!isOdd(ti))
        ti = ZpD.pr - ti;
    return ti;
}

void gfpvar::randomize(PRNG& G, int)
{
    a.randomize(G, ZpD);
}

void gfpvar::almost_randomize(PRNG& G)
{
    randomize(G);
}

void gfpvar::pack(octetStream& os, int) const
{
    a.pack(os, ZpD);
}

void gfpvar::unpack(octetStream& os, int)
{
    a.unpack(os, ZpD);
}

void gfpvar::output(ostream& o, bool human) const
{
    a.output(o, ZpD, human);
}

void gfpvar::input(istream& i, bool human)
{
    a.input(i, ZpD, human);
}

ostream& operator <<(ostream& o, const gfpvar& x)
{
    x.output(o, true);
    return o;
}

istream& operator >>(istream& i, gfpvar& x)
{
    x.input(i, true);
    return i;
}
