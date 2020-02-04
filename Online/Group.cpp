#include "Group.h"

void G1Op::add_plain_aff(G1_Affine_Coordinates &c,
                         const G1_Affine_Coordinates &a,
                         const G1_Affine_Coordinates_Plain &b)
{
    Share dy, dx;
    sub_plain(dy, a.y, b.y);
    sub_plain(dx, a.x, b.x);

    Share m, mm;
    div(m, dy, dx);
    sqr(mm, m);

    Share cx;
    sub(cx, mm, a.x);
    sub_plain(cx, cx, b.x);

    Share acx;
    sub(acx, a.x, cx);
    mul(acx, m, acx);

    sub(c.y, acx, a.y);
    c.x = cx;
}

void G1Op::add_aff(G1_Affine_Coordinates &c,
                   const G1_Affine_Coordinates &a,
                   const G1_Affine_Coordinates &b)
{
    Share dy, dx;
    sub(dy, a.y, b.y);
    sub(dx, a.x, b.x);

    Share m, mm;
    div(m, dy, dx);
    sqr(mm, m);

    Share cx;
    sub(cx, mm, a.x);
    sub(cx, cx, b.x);

    Share acx;
    sub(acx, a.x, cx);
    mul(acx, m, acx);

    sub(c.y, acx, a.y);
    c.x = cx;
}

void G1Op::add_plain_jac(G1_Jacobian_Coordinates &c,
                         const G1_Jacobian_Coordinates &a,
                         const G1_Jacobian_Coordinates_Plain &b)
{
}

void G1Op::add_jac(G1_Jacobian_Coordinates &c,
                   const G1_Jacobian_Coordinates &a,
                   const G1_Jacobian_Coordinates &b)
{
}