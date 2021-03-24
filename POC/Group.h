#pragma once

#include "OnlineOp.h"

template <class T>
class G1_Affine_Coordinates
{
    typedef typename T::clear clear;

public:
    T x;
    T y;
    G1_Affine_Coordinates() {}
    G1_Affine_Coordinates(T &x_, T &y_) : x(x_), y(y_) {}
};

template <class T>
class G1_Affine_Coordinates_Plain
{
    typedef typename T::clear clear;

public:
    clear x;
    clear y;
    G1_Affine_Coordinates_Plain() {}
    G1_Affine_Coordinates_Plain(clear &x_, clear &y_) : x(x_), y(y_) {}
};

template <class T>
class G1_Jacobian_Coordinates
{
    typedef typename T::clear clear;

public:
    T X;
    T Y;
    T Z;
    G1_Jacobian_Coordinates() {}
    G1_Jacobian_Coordinates(T &X_, T &Y_, T &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G1_Jacobian_Coordinates_Plain
{
    typedef typename T::clear clear;

public:
    clear X;
    clear Y;
    clear Z;
    G1_Jacobian_Coordinates_Plain() {}
    G1_Jacobian_Coordinates_Plain(clear &X_, clear &Y_, clear &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G1_Projective_Coordinates
{
    typedef typename T::clear clear;

public:
    T X;
    T Y;
    T Z;
    G1_Projective_Coordinates() {}
    G1_Projective_Coordinates(T &X_, T &Y_, T &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G1_Projective_Coordinates_Plain
{
    typedef typename T::clear clear;

public:
    clear X;
    clear Y;
    clear Z;
    G1_Projective_Coordinates_Plain() {}
    G1_Projective_Coordinates_Plain(clear &X_, clear &Y_, clear &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G1Op : public OnlineOp<T>
{
    typedef typename T::clear clear;

public:
    using OnlineOp<T>::OnlineOp;

    // G1Op(Processor &Proc_, int online_num_, Player &P_,
    //      offline_control_data &OCD_, Machine &machine_)
    //     : OnlineOp(Proc_, online_num_, P_, OCD_, machine_) {}

    void add_plain_aff(G1_Affine_Coordinates<T> &c,
                       const G1_Affine_Coordinates<T> &a,
                       const G1_Affine_Coordinates_Plain<T> &b);

    void add_plain_aff_inplace(G1_Affine_Coordinates<T> &c,
                               const G1_Affine_Coordinates_Plain<T> &a);

    void add_aff(G1_Affine_Coordinates<T> &c,
                 const G1_Affine_Coordinates<T> &a,
                 const G1_Affine_Coordinates<T> &b);

    void add_aff_inplace(G1_Affine_Coordinates<T> &c,
                         const G1_Affine_Coordinates<T> &a);

    void add_plain_jac(G1_Jacobian_Coordinates<T> &c,
                       const G1_Jacobian_Coordinates<T> &a,
                       const G1_Jacobian_Coordinates_Plain<T> &b);

    void add_plain_jac_inplace(G1_Jacobian_Coordinates<T> &c,
                               const G1_Jacobian_Coordinates_Plain<T> &a);

    void add_jac(G1_Jacobian_Coordinates<T> &c,
                 const G1_Jacobian_Coordinates<T> &a,
                 const G1_Jacobian_Coordinates<T> &b);

    void add_jac_inplace(G1_Jacobian_Coordinates<T> &c,
                         const G1_Jacobian_Coordinates<T> &a);

    void add_plain_proj(G1_Projective_Coordinates<T> &c,
                        const G1_Projective_Coordinates<T> &a,
                        const G1_Projective_Coordinates_Plain<T> &b);

    void add_plain_proj_inplace(G1_Projective_Coordinates<T> &c,
                                const G1_Projective_Coordinates_Plain<T> &a);

    void add_proj(G1_Projective_Coordinates<T> &c,
                  const G1_Projective_Coordinates<T> &a,
                  const G1_Projective_Coordinates<T> &b);

    void add_proj_inplace(G1_Projective_Coordinates<T> &c,
                          const G1_Projective_Coordinates<T> &a);

    void reveal_and_print(const G1_Affine_Coordinates<T> ac);

    template <typename V>
    void reveal_and_print(const V p);

    void G1_test_add_aff();
    void G1_test_add_jac();
    void G1_test_add_proj();
};

template <class T>
class G2_Affine_Coordinates
{
    typedef typename T::clear clear;

public:
    Complex<T> x;
    Complex<T> y;
    G2_Affine_Coordinates() {}
    G2_Affine_Coordinates(Complex<T> &x_, Complex<T> &y_) : x(x_), y(y_) {}
};

template <class T>
class G2_Affine_Coordinates_Plain
{
    typedef typename T::clear clear;

public:
    Complex_Plain<T> x;
    Complex_Plain<T> y;
    G2_Affine_Coordinates_Plain() {}
    G2_Affine_Coordinates_Plain(Complex_Plain<T> &x_, Complex_Plain<T> &y_) : x(x_), y(y_) {}
};

template <class T>
class G2_Jacobian_Coordinates
{
    typedef typename T::clear clear;

public:
    Complex<T> X;
    Complex<T> Y;
    Complex<T> Z;
    G2_Jacobian_Coordinates() {}
    G2_Jacobian_Coordinates(Complex<T> &X_, Complex<T> &Y_, Complex<T> &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G2_Jacobian_Coordinates_Plain
{
    typedef typename T::clear clear;

public:
    Complex_Plain<T> X;
    Complex_Plain<T> Y;
    Complex_Plain<T> Z;
    G2_Jacobian_Coordinates_Plain() {}
    G2_Jacobian_Coordinates_Plain(Complex_Plain<T> &X_, Complex_Plain<T> &Y_, Complex_Plain<T> &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G2_Projective_Coordinates
{
    typedef typename T::clear clear;

public:
    Complex<T> X;
    Complex<T> Y;
    Complex<T> Z;
    G2_Projective_Coordinates() {}
    G2_Projective_Coordinates(Complex<T> &X_, Complex<T> &Y_, Complex<T> &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G2_Projective_Coordinates_Plain
{
    typedef typename T::clear clear;

public:
    Complex_Plain<T> X;
    Complex_Plain<T> Y;
    Complex_Plain<T> Z;
    G2_Projective_Coordinates_Plain() {}
    G2_Projective_Coordinates_Plain(Complex_Plain<T> &X_, Complex_Plain<T> &Y_, Complex_Plain<T> &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

template <class T>
class G2Op : public OnlineOp<T>
{
    typedef typename T::clear clear;

public:
    using OnlineOp<T>::OnlineOp;

    void add_plain_aff(G2_Affine_Coordinates<T> &c,
                       const G2_Affine_Coordinates<T> &a,
                       const G2_Affine_Coordinates_Plain<T> &b);

    void add_plain_aff_inplace(G2_Affine_Coordinates<T> &c,
                               const G2_Affine_Coordinates_Plain<T> &a);

    void add_aff(G2_Affine_Coordinates<T> &c,
                 const G2_Affine_Coordinates<T> &a,
                 const G2_Affine_Coordinates<T> &b);

    void add_aff_inplace(G2_Affine_Coordinates<T> &c,
                         const G2_Affine_Coordinates<T> &a);

    void add_plain_jac(G2_Jacobian_Coordinates<T> &c,
                       const G2_Jacobian_Coordinates<T> &a,
                       const G2_Jacobian_Coordinates_Plain<T> &b);

    void add_plain_jac_inplace(G2_Jacobian_Coordinates<T> &c,
                               const G2_Jacobian_Coordinates_Plain<T> &a);

    void add_jac(G2_Jacobian_Coordinates<T> &c,
                 const G2_Jacobian_Coordinates<T> &a,
                 const G2_Jacobian_Coordinates<T> &b);

    void add_jac_inplace(G2_Jacobian_Coordinates<T> &c,
                         const G2_Jacobian_Coordinates<T> &a);

    void add_plain_proj(G2_Projective_Coordinates<T> &c,
                        const G2_Projective_Coordinates<T> &a,
                        const G2_Projective_Coordinates_Plain<T> &b);

    void add_plain_proj_inplace(G2_Projective_Coordinates<T> &c,
                                const G2_Projective_Coordinates_Plain<T> &a);

    void add_proj(G2_Projective_Coordinates<T> &c,
                  const G2_Projective_Coordinates<T> &a,
                  const G2_Projective_Coordinates<T> &b);

    void add_proj_inplace(G2_Projective_Coordinates<T> &c,
                          const G2_Projective_Coordinates<T> &a);

    void reveal_and_print(const G2_Affine_Coordinates<T> ac);

    template <typename V>
    void reveal_and_print(const V p);

    void G2_test_add_aff();
    void G2_test_add_jac();
    void G2_test_add_proj();
};
#include "Group.hpp"
