#pragma once

#include "OnlineOp.h"

class G1_Affine_Coordinates
{
public:
    Share x;
    Share y;
    G1_Affine_Coordinates() {}
    G1_Affine_Coordinates(Share &x_, Share &y_) : x(x_), y(y_) {}
};

class G1_Affine_Coordinates_Plain
{
public:
    gfp x;
    gfp y;
    G1_Affine_Coordinates_Plain() {}
    G1_Affine_Coordinates_Plain(gfp &x_, gfp &y_) : x(x_), y(y_) {}
};

class G1_Jacobian_Coordinates
{
public:
    Share X;
    Share Y;
    Share Z;
    G1_Jacobian_Coordinates() {}
    G1_Jacobian_Coordinates(Share &X_, Share &Y_, Share &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G1_Jacobian_Coordinates_Plain
{
public:
    gfp X;
    gfp Y;
    gfp Z;
    G1_Jacobian_Coordinates_Plain() {}
    G1_Jacobian_Coordinates_Plain(gfp &X_, gfp &Y_, gfp &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G1_Projective_Coordinates
{
public:
    Share X;
    Share Y;
    Share Z;
    G1_Projective_Coordinates() {}
    G1_Projective_Coordinates(Share &X_, Share &Y_, Share &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G1_Projective_Coordinates_Plain
{
public:
    gfp X;
    gfp Y;
    gfp Z;
    G1_Projective_Coordinates_Plain() {}
    G1_Projective_Coordinates_Plain(gfp &X_, gfp &Y_, gfp &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G1Op : public OnlineOp
{
    G1Op(Processor &Proc_, int online_num_, Player &P_,
         offline_control_data &OCD_, Machine &machine_)
        : OnlineOp(Proc_, online_num_, P_, OCD_, machine_) {}

    void add_plain_aff(G1_Affine_Coordinates &c,
                       const G1_Affine_Coordinates &a,
                       const G1_Affine_Coordinates_Plain &b);

    void add_aff(G1_Affine_Coordinates &c,
                 const G1_Affine_Coordinates &a,
                 const G1_Affine_Coordinates &b);

    void add_plain_jac(G1_Jacobian_Coordinates &c,
                       const G1_Jacobian_Coordinates &a,
                       const G1_Jacobian_Coordinates_Plain &b);

    void add_jac(G1_Jacobian_Coordinates &c,
                 const G1_Jacobian_Coordinates &a,
                 const G1_Jacobian_Coordinates &b);

    void add_plain_proj(G1_Projective_Coordinates &c,
                        const G1_Projective_Coordinates &a,
                        const G1_Projective_Coordinates_Plain &b);

    void add_proj(G1_Projective_Coordinates &c,
                  const G1_Projective_Coordinates &a,
                  const G1_Projective_Coordinates &b);
};

class G2_Affine_Coordinates
{
public:
    Complex x;
    Complex y;
    G2_Affine_Coordinates() {}
    G2_Affine_Coordinates(Complex &x_, Complex &y_) : x(x_), y(y_) {}
};

class G2_Affine_Coordinates_Plain
{
public:
    Complex_Plain x;
    Complex_Plain y;
    G2_Affine_Coordinates_Plain() {}
    G2_Affine_Coordinates_Plain(Complex_Plain &x_, Complex_Plain &y_) : x(x_), y(y_) {}
};

class G2_Jacobian_Coordinates
{
public:
    Complex X;
    Complex Y;
    Complex Z;
    G2_Jacobian_Coordinates() {}
    G2_Jacobian_Coordinates(Complex &X_, Complex &Y_, Complex &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G2_Jacobian_Coordinates_Plain
{
public:
    Complex_Plain X;
    Complex_Plain Y;
    Complex_Plain Z;
    G2_Jacobian_Coordinates_Plain() {}
    G2_Jacobian_Coordinates_Plain(Complex_Plain &X_, Complex_Plain &Y_, Complex_Plain &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G2_Projective_Coordinates
{
public:
    Complex X;
    Complex Y;
    Complex Z;
    G2_Projective_Coordinates() {}
    G2_Projective_Coordinates(Complex &X_, Complex &Y_, Complex &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G2_Projective_Coordinates_Plain
{
public:
    Complex_Plain X;
    Complex_Plain Y;
    Complex_Plain Z;
    G2_Projective_Coordinates_Plain() {}
    G2_Projective_Coordinates_Plain(Complex_Plain &X_, Complex_Plain &Y_, Complex_Plain &Z_) : X(X_), Y(Y_), Z(Z_) {}
};

class G2Op : public OnlineOp
{
    G2Op(Processor &Proc_, int online_num_, Player &P_,
         offline_control_data &OCD_, Machine &machine_)
        : OnlineOp(Proc_, online_num_, P_, OCD_, machine_) {}

    void add_plain_aff(G2_Affine_Coordinates &c,
                       const G2_Affine_Coordinates &a,
                       const G2_Affine_Coordinates_Plain &b);

    void add_aff(G2_Affine_Coordinates &c,
                 const G2_Affine_Coordinates &a,
                 const G2_Affine_Coordinates &b);

    void add_plain_jac(G2_Jacobian_Coordinates &c,
                       const G2_Jacobian_Coordinates &a,
                       const G2_Jacobian_Coordinates_Plain &b);

    void add_jac(G2_Jacobian_Coordinates &c,
                 const G2_Jacobian_Coordinates &a,
                 const G2_Jacobian_Coordinates &b);

    void add_plain_proj(G2_Projective_Coordinates &c,
                        const G2_Projective_Coordinates &a,
                        const G2_Projective_Coordinates_Plain &b);

    void add_proj(G2_Projective_Coordinates &c,
                  const G2_Projective_Coordinates &a,
                  const G2_Projective_Coordinates &b);
};