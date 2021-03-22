/*
 * vargfp.h
 *
 */

#ifndef MATH_GFPVAR_H_
#define MATH_GFPVAR_H_

#include "modp.h"
#include "Zp_Data.h"
#include "Setup.h"

class FFT_Data;

class gfpvar
{
    typedef modp_<MAX_MOD_SZ / 2> modp_type;

    static Zp_Data ZpD;

    modp_type a;

public:
    typedef gfpvar Scalar;
    typedef FFT_Data FD;

    typedef void Square;
    typedef void next;

    static const int MAX_N_BITS = modp_type::MAX_N_BITS;
    static const int MAX_EDABITS = modp_type::MAX_N_BITS;
    static const int N_LIMBS = modp_type::N_LIMBS;

    static const true_type invertible;
    static const true_type prime_field;
    static const false_type characteristic_two;

    static string type_string();
    static string type_short();
    static char type_char();

    static int length();
    static int size();

    static bool allows(Dtype dtype);
    static DataFieldType field_type();

    static void init_field(bigint prime, bool montgomery = true);
    static void init_default(int lgp, bool montgomery = true);

    static const Zp_Data& get_ZpD();
    static const bigint& pr();

    template<class T>
    static void generate_setup(string prep_data_prefix, int nplayers, int lgp);
    static void check_setup(string dir);
    static void write_setup(string dir);
    template<class T>
    static void write_setup(int nplayers)
    {
        write_setup(get_prep_sub_dir<T>(nplayers));
    }

    gfpvar();
    gfpvar(int other);
    gfpvar(const bigint& other);
    gfpvar(const modp& other);

    template<int X, int L>
    gfpvar(const gfp_<X, L>& other)
    {
        assert(pr() == other.pr());
        a = other.get();
    }

    void assign(const char* buffer);

    void assign_zero();
    void assign_one();

    bool is_zero();
    bool is_one();

    modp_type get() const;

    gfpvar operator+(const gfpvar& other) const;
    gfpvar operator-(const gfpvar& other) const;
    gfpvar operator*(const gfpvar& other) const;
    gfpvar operator/(const gfpvar& other) const;

    gfpvar& operator+=(const gfpvar& other);
    gfpvar& operator-=(const gfpvar& other);
    gfpvar& operator*=(const gfpvar& other);

    bool operator==(const gfpvar& other) const;
    bool operator!=(const gfpvar& other) const;

    void add(octetStream& other);

    void negate();

    gfpvar invert() const;

    gfpvar sqrRoot() const;

    void randomize(PRNG& G, int n_bits = -1);
    void almost_randomize(PRNG& G);

    void pack(octetStream& os, int n_bits = -1) const;
    void unpack(octetStream& os, int n_bits = -1);

    void output(ostream& o, bool human) const;
    void input(istream& o, bool human);
};

ostream& operator<<(ostream& o, const gfpvar& x);
istream& operator>>(istream& i, gfpvar& x);

typedef gfpvar gfp;

#endif /* MATH_GFPVAR_H_ */
