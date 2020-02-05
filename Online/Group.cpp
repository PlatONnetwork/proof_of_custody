#include "Group.h"

//the operations could be optimized as templates//

//------G1 ops------//

void G1Op::add_plain_aff(G1_Affine_Coordinates &c,
                         const G1_Affine_Coordinates &a,
                         const G1_Affine_Coordinates_Plain &b)
{
    Share dx;
    Share m, mm;
    sub_plain(m, a.y, b.y);
    sub_plain(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub_plain(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G1Op::add_aff(G1_Affine_Coordinates &c,
                   const G1_Affine_Coordinates &a,
                   const G1_Affine_Coordinates &b)
{
    Share dx;
    Share m, mm;
    sub(m, a.y, b.y);
    sub(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G1Op::add_plain_jac(G1_Jacobian_Coordinates &c,
                         const G1_Jacobian_Coordinates &a,
                         const G1_Jacobian_Coordinates_Plain &b)
{
    gfp bZ3;
    Share S1, S2, U1, U2;
    sqr(S2, a.Z);
    bZ3 = b.Z * b.Z;

    mul_plain(U2, S2, b.X);  //U2 = X2*Z1^2
    mul_plain(U1, a.X, bZ3); //U1 = X1*Z2^2

    Share S;
    mul_plain(S, S2, b.Y);
    mul(S2, S, a.Z); // S2 = Y2*Z1^3

    bZ3 = bZ3 * b.Z;
    mul_plain(S1, a.Y, bZ3); // S1 = Y1*Z2^3

    Share R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul_plain(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Share tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G1Op::add_jac(G1_Jacobian_Coordinates &c,
                   const G1_Jacobian_Coordinates &a,
                   const G1_Jacobian_Coordinates &b)
{
    Share S1, S2, U1, U2;
    sqr(S2, a.Z);
    sqr(S1, b.Z);

    mul(U2, b.X, S2); //U2 = X2*Z1^2
    mul(U1, a.X, S1); //U1 = X1*Z2^2

    mul_inplace(S2, b.Y);
    mul_inplace(S2, a.Z); // S2 = Y2*Z1^3

    mul_inplace(S1, a.Y);
    mul_inplace(S1, b.Z); // S1 = Y1*Z2^3

    Share R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Share tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G1Op::add_plain_proj(G1_Projective_Coordinates &c,
                          const G1_Projective_Coordinates &a,
                          const G1_Projective_Coordinates_Plain &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Share U1, U2, S1, S2;
    mul_plain(S2, a.Z, b.Y); //S2 = Y2 * Z1
    mul_plain(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul_plain(U2, a.Z, b.X); //U2 = X2 * Z1
    mul_plain(U1, a.X, b.Z); //U1 = X1 * Z2

    Share T, H, R;
    sub(R, S1, S2);         //R = S1 - S2
    sub(H, U1, U2);         //H = U1 - U2
    mul_plain(T, a.Z, b.Z); //T = Z1 * Z2

    Share G, V, W;
    Share R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(G, H2, H); // G = H^3

    mul(c.Z, T, G); //Z3 = T * G

    Share tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

void G1Op::add_proj(G1_Projective_Coordinates &c,
                    const G1_Projective_Coordinates &a,
                    const G1_Projective_Coordinates &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Share U1, U2, S1, S2;
    mul(S2, b.Y, a.Z); //S2 = Y2 * Z1
    mul(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul(U2, b.X, a.Z); //U2 = X2 * Z1
    mul(U1, a.X, b.Z); //U1 = X1 * Z2

    Share T, H, R;
    sub(R, S1, S2);   //R = S1 - S2
    sub(H, U1, U2);   //H = U1 - U2
    mul(T, a.Z, b.Z); //T = Z1 * Z2

    Share G, V, W;
    Share R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(G, H2, H); // G = H^3

    mul(c.Z, T, G); //Z3 = T * G

    Share tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

//------G2 ops------//

void G2Op::add_plain_aff(G2_Affine_Coordinates &c,
                         const G2_Affine_Coordinates &a,
                         const G2_Affine_Coordinates_Plain &b)
{
    Complex dx;
    Complex m, mm;
    sub_plain(m, a.y, b.y);
    sub_plain(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub_plain(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G2Op::add_aff(G2_Affine_Coordinates &c,
                   const G2_Affine_Coordinates &a,
                   const G2_Affine_Coordinates &b)
{
    Complex dx;
    Complex m, mm;
    sub(m, a.y, b.y);
    sub(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void mul_complex_plain(Complex_Plain &c, const Complex_Plain &a, const Complex_Plain &b)
{
    gfp r2, i2;
    r2 = a.real * b.real;
    i2 = a.imag * b.imag;
    c.real = r2 - i2;

    r2 = a.real * b.imag;
    i2 = a.imag * b.real;
    c.imag = r2 + i2;
}

void mul_complex_plain_inplace(Complex_Plain &c, const Complex_Plain &a)
{
    Complex_Plain tmp;
    mul_complex_plain(tmp, c, a);
    c = tmp;
}

void G2Op::add_plain_jac(G2_Jacobian_Coordinates &c,
                         const G2_Jacobian_Coordinates &a,
                         const G2_Jacobian_Coordinates_Plain &b)
{
    Complex_Plain bZ3;
    Complex S1, S2, U1, U2;
    sqr(S2, a.Z);
    mul_complex_plain(bZ3, b.Z, b.Z);

    mul_plain(U2, S2, b.X);  //U2 = X2*Z1^2
    mul_plain(U1, a.X, bZ3); //U1 = X1*Z2^2

    Complex S;
    mul_plain(S, S2, b.Y);
    mul(S2, S, a.Z); // S2 = Y2*Z1^3

    mul_complex_plain_inplace(bZ3, b.Z);

    mul_plain(S1, a.Y, bZ3); // S1 = Y1*Z2^3

    Complex R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul_plain(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Complex tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G2Op::add_jac(G2_Jacobian_Coordinates &c,
                   const G2_Jacobian_Coordinates &a,
                   const G2_Jacobian_Coordinates &b)
{
    Complex S1, S2, U1, U2;
    sqr(S2, a.Z);
    sqr(S1, b.Z);

    mul(U2, b.X, S2); //U2 = X2*Z1^2
    mul(U1, a.X, S1); //U1 = X1*Z2^2

    mul_inplace(S2, b.Y);
    mul_inplace(S2, a.Z); // S2 = Y2*Z1^3

    mul_inplace(S1, a.Y);
    mul_inplace(S1, b.Z); // S1 = Y1*Z2^3

    Complex R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Complex tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G2Op::add_plain_proj(G2_Projective_Coordinates &c,
                          const G2_Projective_Coordinates &a,
                          const G2_Projective_Coordinates_Plain &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Complex U1, U2, S1, S2;
    mul_plain(S2, a.Z, b.Y); //S2 = Y2 * Z1
    mul_plain(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul_plain(U2, a.Z, b.X); //U2 = X2 * Z1
    mul_plain(U1, a.X, b.Z); //U1 = X1 * Z2

    Complex T, H, R;
    sub(R, S1, S2);         //R = S1 - S2
    sub(H, U1, U2);         //H = U1 - U2
    mul_plain(T, a.Z, b.Z); //T = Z1 * Z2

    Complex G, V, W;
    Complex R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(G, H2, H); // G = H^3

    mul(c.Z, T, G); //Z3 = T * G

    Complex tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

void G2Op::add_proj(G2_Projective_Coordinates &c,
                    const G2_Projective_Coordinates &a,
                    const G2_Projective_Coordinates &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Complex U1, U2, S1, S2;
    mul(S2, b.Y, a.Z); //S2 = Y2 * Z1
    mul(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul(U2, b.X, a.Z); //U2 = X2 * Z1
    mul(U1, a.X, b.Z); //U1 = X1 * Z2

    Complex T, H, R;
    sub(R, S1, S2);   //R = S1 - S2
    sub(H, U1, U2);   //H = U1 - U2
    mul(T, a.Z, b.Z); //T = Z1 * Z2

    Complex G, V, W;
    Complex R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(G, H2, H); // G = H^3

    mul(c.Z, T, G); //Z3 = T * G

    Complex tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}