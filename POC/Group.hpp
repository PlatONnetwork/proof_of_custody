#include "Group.h"

#define OK_CODE 0

//the operations could be optimized as templates//

//------G1 ops------//

template <class T>
void G1Op<T>::add_plain_aff(G1_Affine_Coordinates<T> &c,
                            const G1_Affine_Coordinates<T> &a,
                            const G1_Affine_Coordinates_Plain<T> &b)
{
    T dx;
    T m, mm;
    this->sub_plain(m, a.y, b.y);
    this->sub_plain(dx, a.x, b.x);

    this->div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    this->sqr(mm, m);         // mm = m^2

    this->sub_inplace(mm, a.x);
    this->sub_plain(c.x, mm, b.x); // x3 = m^2-x1-x2

    this->sub(c.y, a.x, c.x);
    this->mul_inplace(c.y, m);
    this->sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

template <class T>
void G1Op<T>::add_plain_aff_inplace(G1_Affine_Coordinates<T> &c,
                                    const G1_Affine_Coordinates_Plain<T> &a)
{
    G1_Affine_Coordinates<T> tmp;
    add_plain_aff(tmp, c, a);
    c = tmp;
}
template <class T>
void G1Op<T>::add_aff(G1_Affine_Coordinates<T> &c,
                      const G1_Affine_Coordinates<T> &a,
                      const G1_Affine_Coordinates<T> &b)
{
    T dx;
    T m, mm;
    this->sub(m, a.y, b.y);
    this->sub(dx, a.x, b.x);

    this->div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    this->sqr(mm, m);         // mm = m^2

    this->sub_inplace(mm, a.x);
    this->sub(c.x, mm, b.x); // x3 = m^2-x1-x2

    this->sub(c.y, a.x, c.x);
    this->mul_inplace(c.y, m);
    this->sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

template <class T>
void G1Op<T>::add_aff_inplace(G1_Affine_Coordinates<T> &c, const G1_Affine_Coordinates<T> &a)
{
    G1_Affine_Coordinates<T> tmp;
    add_aff(tmp, c, a);
    c = tmp;
}

template <class T>
void G1Op<T>::add_plain_jac(G1_Jacobian_Coordinates<T> &c,
                            const G1_Jacobian_Coordinates<T> &a,
                            const G1_Jacobian_Coordinates_Plain<T> &b)
{
    clear bZ3;
    T S1, S2, U1, U2;
    this->sqr(S2, a.Z);
    bZ3 = b.Z * b.Z;

    this->mul_plain(U2, S2, b.X);  //U2 = X2*Z1^2
    this->mul_plain(U1, a.X, bZ3); //U1 = X1*Z2^2

    T S;
    this->mul_plain(S, S2, b.Y);
    this->mul(S2, S, a.Z); // S2 = Y2*Z1^3

    bZ3 = bZ3 * b.Z;
    this->mul_plain(S1, a.Y, bZ3); // S1 = Y1*Z2^3

    T R, H, V, G;
    this->sub(R, S1, S2); //R = S1-S2
    this->sub(H, U1, U2); // H = U1-U2

    this->sqr(V, H);
    this->mul(G, V, H);       //G = H^3
    this->mul_inplace(V, U1); // V = U1*H^2

    this->mul_plain(c.Z, a.Z, b.Z);
    this->mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    T tmp;
    this->sqr(tmp, R);

    this->add(c.X, tmp, G);
    this->sub_inplace(c.X, V);
    this->sub_inplace(c.X, V); //X3 = R^2+G-2V

    this->mul(tmp, S1, G);
    this->sub(c.Y, V, c.X);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

template <class T>
void G1Op<T>::add_plain_jac_inplace(G1_Jacobian_Coordinates<T> &c,
                                    const G1_Jacobian_Coordinates_Plain<T> &a)
{
    G1_Jacobian_Coordinates<T> tmp;
    add_plain_jac(tmp, c, a);
    c = tmp;
}
template <class T>
void G1Op<T>::add_jac(G1_Jacobian_Coordinates<T> &c,
                      const G1_Jacobian_Coordinates<T> &a,
                      const G1_Jacobian_Coordinates<T> &b)
{
    T S1, S2, U1, U2;
    this->sqr(S2, a.Z);
    this->sqr(S1, b.Z);

    this->mul(U2, b.X, S2); //U2 = X2*Z1^2
    this->mul(U1, a.X, S1); //U1 = X1*Z2^2

    this->mul_inplace(S2, b.Y);
    this->mul_inplace(S2, a.Z); // S2 = Y2*Z1^3

    this->mul_inplace(S1, a.Y);
    this->mul_inplace(S1, b.Z); // S1 = Y1*Z2^3

    T R, H, V, G;
    this->sub(R, S1, S2); //R = S1-S2
    this->sub(H, U1, U2); // H = U1-U2

    this->sqr(V, H);
    this->mul(G, V, H);       //G = H^3
    this->mul_inplace(V, U1); // V = U1*H^2

    this->mul(c.Z, a.Z, b.Z);
    this->mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    T tmp;
    this->sqr(tmp, R);

    this->add(c.X, tmp, G);
    this->sub_inplace(c.X, V);
    this->sub_inplace(c.X, V); //X3 = R^2+G-2V

    this->mul(tmp, S1, G);
    this->sub(c.Y, V, c.X);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

template <class T>
void G1Op<T>::add_jac_inplace(G1_Jacobian_Coordinates<T> &c,
                              const G1_Jacobian_Coordinates<T> &a)
{
    G1_Jacobian_Coordinates<T> tmp;
    add_jac(tmp, c, a);
    c = tmp;
}

template <class T>
void G1Op<T>::add_plain_proj(G1_Projective_Coordinates<T> &c,
                             const G1_Projective_Coordinates<T> &a,
                             const G1_Projective_Coordinates_Plain<T> &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    T U1, U2, S1, S2;
    this->mul_plain(S2, a.Z, b.Y); //S2 = Y2 * Z1
    this->mul_plain(S1, a.Y, b.Z); //S1 = Y1 * Z2
    this->mul_plain(U2, a.Z, b.X); //U2 = X2 * Z1
    this->mul_plain(U1, a.X, b.Z); //U1 = X1 * Z2

    T T_, H, R;
    this->sub(R, S1, S2);          //R = S1 - S2
    this->sub(H, U1, U2);          //H = U1 - U2
    this->mul_plain(T_, a.Z, b.Z); //T = Z1 * Z2

    T G, V, W;
    T R2, H2;

    this->sqr(H2, H);     //H2 = H^2
    this->mul(V, U1, H2); //V = U1*H^2

    this->mul(G, H2, H); // G = H^3

    this->sqr(R2, R); //R2 = R^2
    this->mul(W, R2, T_);
    this->add_inplace(W, G);
    this->sub_inplace(W, V);
    this->sub_inplace(W, V); // W = R^2*T + G - 2V

    this->mul(c.Z, T_, G); //Z3 = T * G

    T tmp;
    this->mul(tmp, S1, G);
    this->sub(c.Y, V, W);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    this->mul(c.X, H, W); // X3 = H * W
}

template <class T>
void G1Op<T>::add_plain_proj_inplace(G1_Projective_Coordinates<T> &c,
                                     const G1_Projective_Coordinates_Plain<T> &a)
{
    G1_Projective_Coordinates<T> tmp;
    add_plain_proj(tmp, c, a);
    c = tmp;
}

template <class T>
void G1Op<T>::add_proj(G1_Projective_Coordinates<T> &c,
                       const G1_Projective_Coordinates<T> &a,
                       const G1_Projective_Coordinates<T> &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    T U1, U2, S1, S2;
    this->mul(S2, b.Y, a.Z); //S2 = Y2 * Z1
    this->mul(S1, a.Y, b.Z); //S1 = Y1 * Z2
    this->mul(U2, b.X, a.Z); //U2 = X2 * Z1
    this->mul(U1, a.X, b.Z); //U1 = X1 * Z2

    T T_, H, R;
    this->sub(R, S1, S2);    //R = S1 - S2
    this->sub(H, U1, U2);    //H = U1 - U2
    this->mul(T_, a.Z, b.Z); //T = Z1 * Z2

    T G, V, W;
    T R2, H2;

    this->sqr(H2, H);     //H2 = H^2
    this->mul(V, U1, H2); //V = U1*H^2

    this->mul(G, H2, H); // G = H^3

    this->sqr(R2, R); //R2 = R^2
    this->mul(W, R2, T_);
    this->add_inplace(W, G);
    this->sub_inplace(W, V);
    this->sub_inplace(W, V); // W = R^2*T + G - 2V

    this->mul(c.Z, T_, G); //Z3 = T * G

    T tmp;
    this->mul(tmp, S1, G);
    this->sub(c.Y, V, W);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    this->mul(c.X, H, W); // X3 = H * W
}

template <class T>
void G1Op<T>::add_proj_inplace(G1_Projective_Coordinates<T> &c,
                               const G1_Projective_Coordinates<T> &a)
{
    G1_Projective_Coordinates<T> tmp;
    add_proj(tmp, c, a);
    c = tmp;
}

template <class T>
template <typename V>
void G1Op<T>::reveal_and_print(const V p)
{
    cout << "X: " << endl;
    OnlineOp<T>::reveal_and_print({p.X});
    cout << "Y: " << endl;
    OnlineOp<T>::reveal_and_print({p.Y});
    cout << "Z: " << endl;
    OnlineOp<T>::reveal_and_print({p.Z});
}

template <class T>
void G1Op<T>::reveal_and_print(const G1_Affine_Coordinates<T> ac)
{
    cout << "x: " << endl;
    OnlineOp<T>::reveal_and_print({ac.x});
    cout << "y: " << endl;
    OnlineOp<T>::reveal_and_print({ac.y});
}

//------test--------//

template <class T>
void G1Op<T>::G1_test_add_aff()
{
    PRINT_TEST_BEG();

    vector<string> point1(2), point2(2);
    point1[0] = "1295728164626534415618781265254580659926272354843985135401485454504777372546472596033371574568093826491865410771001";
    point1[1] = "3839565232484830821203974136028462429409860797185329370952032679694793040839037122415038227769061259923986030367015";

    point2[0] = "613192939246165145697364032654702068835570358685595826913421400569692251883551998352417059167240540423077723627092";
    point2[1] = "1939834878592542483572653782999203248604368035458557492969812954708733328780181922866866198381425602016362784974612";

    clear g1x, g1y, g2x, g2y;
    this->str_to_gfp(g1x, point1[0]);
    this->str_to_gfp(g1y, point1[1]);
    this->str_to_gfp(g2x, point2[0]);
    this->str_to_gfp(g2y, point2[1]);

    G1_Affine_Coordinates_Plain<T> acp(g2x, g2y);

    vector<G1_Affine_Coordinates<T>> ac(2);
    this->get_inputs(0, ac[0].x, g1x);
    this->get_inputs(0, ac[0].y, g1y);

    this->get_inputs(1, ac[1].x, g2x);
    this->get_inputs(1, ac[1].y, g2y);
    cout << "--------test add_affine--------" << endl;
    G1_Affine_Coordinates<T> res(ac[0]);
    reveal_and_print(res);

    add_aff_inplace(res, ac[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = ac[0];
    add_plain_aff_inplace(res, acp);
    reveal_and_print(res);

    PRINT_TEST_END();
}

template <class T>
void G1Op<T>::G1_test_add_jac()
{
    PRINT_TEST_BEG();

    vector<string> point1(3), point2(3);
    point1[0] = "3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507";
    point1[1] = "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569";
    point1[2] = "1";

    point2[0] = "506495629725590588476438736938928437216878988401200917968428110806855219037361470594787775621162927277631758167243";
    point2[1] = "3215784584818841779393380451963501913485474208604528267666791755940205624806737197312591247682567521662969376111781";
    point2[2] = "2679013089888952946040942759883842443169867751876699240853087472833022847912667012945449310706733069984783512883138";

    clear g1x, g1y, g1z, g2x, g2y, g2z;
    this->str_to_gfp(g1x, point1[0]);
    this->str_to_gfp(g1y, point1[1]);
    this->str_to_gfp(g1z, point1[2]);
    this->str_to_gfp(g2x, point2[0]);
    this->str_to_gfp(g2y, point2[1]);
    this->str_to_gfp(g2z, point2[2]);

    G1_Jacobian_Coordinates_Plain<T> jcp(g2x, g2y, g2z);

    vector<G1_Jacobian_Coordinates<T>> jc(2);
    this->get_inputs(0, jc[0].X, g1x);
    this->get_inputs(0, jc[0].Y, g1y);
    this->get_inputs(0, jc[0].Z, g1z);

    this->get_inputs(1, jc[1].X, g2x);
    this->get_inputs(1, jc[1].Y, g2y);
    this->get_inputs(1, jc[1].Z, g2z);
    cout << "--------test add_jacobian--------" << endl;
    G1_Jacobian_Coordinates<T> res(jc[0]);
    reveal_and_print(res);

    add_jac_inplace(res, jc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_jacobian--------" << endl;

    res = jc[0];
    reveal_and_print(res);
    add_plain_jac_inplace(res, jcp);
    reveal_and_print(res);

    PRINT_TEST_END();
}

template <class T>
void G1Op<T>::G1_test_add_proj()
{
    PRINT_TEST_BEG();

    vector<string> point1(3), point2(3);
    point1[0] = "904218658502494312590159247105554874787512476566126303635147587402923829641584622770449558420966780868207684298157";
    point1[1] = "3215784584818841779393380451963501913485474208604528267666791755940205624806737197312591247682567521662969376111781";
    point1[2] = "645039760431336131708729805154580111675108263123058424684373273230141681090362840405422204836635344492771939114786";

    point2[0] = "2879263871569338206639216416817100835819581766640957126361400946233890995357542276319529492445765922250816725975001";
    point2[1] = "3670911477192382580091235683783430788523954119523923806817352411742228949842957523061890692823815211520838369778994";
    point2[2] = "1616122912214980288291576325462461695684418736110628553500087472020914353750538673566640769330086294264352298565201";

    clear g1x, g1y, g1z, g2x, g2y, g2z;
    this->str_to_gfp(g1x, point1[0]);
    this->str_to_gfp(g1y, point1[1]);
    this->str_to_gfp(g1z, point1[2]);
    this->str_to_gfp(g2x, point2[0]);
    this->str_to_gfp(g2y, point2[1]);
    this->str_to_gfp(g2z, point2[2]);

    G1_Projective_Coordinates_Plain<T> pcp(g2x, g2y, g2z);

    vector<G1_Projective_Coordinates<T>> pc(2);
    this->get_inputs(0, pc[0].X, g1x);
    this->get_inputs(0, pc[0].Y, g1y);
    this->get_inputs(0, pc[0].Z, g1z);

    this->get_inputs(1, pc[1].X, g2x);
    this->get_inputs(1, pc[1].Y, g2y);
    this->get_inputs(1, pc[1].Z, g2z);
    cout << "--------test add_projective--------" << endl;
    G1_Projective_Coordinates<T> res(pc[0]);
    reveal_and_print(res);

    add_proj_inplace(res, pc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_projective--------" << endl;

    res = pc[0];
    add_plain_proj_inplace(res, pcp);
    reveal_and_print(res);

    PRINT_TEST_END();
}

//------G2 ops------//

template <class T>
void G2Op<T>::add_plain_aff(G2_Affine_Coordinates<T> &c,
                            const G2_Affine_Coordinates<T> &a,
                            const G2_Affine_Coordinates_Plain<T> &b)
{
    Complex<T> dx;
    Complex<T> m, mm;
    this->sub_plain(m, a.y, b.y);
    this->sub_plain(dx, a.x, b.x);

    this->div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    this->sqr(mm, m);         // mm = m^2

    this->sub_inplace(mm, a.x);
    this->sub_plain(c.x, mm, b.x); // x3 = m^2-x1-x2

    this->sub(c.y, a.x, c.x);
    this->mul_inplace(c.y, m);
    this->sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

template <class T>
void G2Op<T>::add_plain_aff_inplace(G2_Affine_Coordinates<T> &c,
                                    const G2_Affine_Coordinates_Plain<T> &a)
{
    G2_Affine_Coordinates<T> tmp;
    add_plain_aff(tmp, c, a);
    c = tmp;
}
template <class T>
void G2Op<T>::add_aff(G2_Affine_Coordinates<T> &c,
                      const G2_Affine_Coordinates<T> &a,
                      const G2_Affine_Coordinates<T> &b)
{
    Complex<T> dx;
    Complex<T> m, mm;

    this->sub(m, a.y, b.y);
    this->sub(dx, a.x, b.x);

    this->div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    this->sqr(mm, m);         // mm = m^2

    this->sub_inplace(mm, a.x);
    this->sub(c.x, mm, b.x); // x3 = m^2-x1-x2

    this->sub(c.y, a.x, c.x);
    this->mul_inplace(c.y, m);
    this->sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

template <class T>
void G2Op<T>::add_aff_inplace(G2_Affine_Coordinates<T> &c,
                              const G2_Affine_Coordinates<T> &a)
{
    G2_Affine_Coordinates<T> tmp;
    add_aff(tmp, c, a);
    c = tmp;
}

template <class T>
void mul_complex_plain(Complex_Plain<T> &c, const Complex_Plain<T> &a, const Complex_Plain<T> &b)
{
    typename T::clear r2, i2;
    r2 = a.real * b.real;
    i2 = a.imag * b.imag;
    c.real = r2 - i2;

    r2 = a.real * b.imag;
    i2 = a.imag * b.real;
    c.imag = r2 + i2;
}

template <class T>
void mul_complex_plain_inplace(Complex_Plain<T> &c, const Complex_Plain<T> &a)
{
    Complex_Plain<T> tmp;
    mul_complex_plain(tmp, c, a);
    c = tmp;
}

template <class T>
void G2Op<T>::add_plain_jac(G2_Jacobian_Coordinates<T> &c,
                            const G2_Jacobian_Coordinates<T> &a,
                            const G2_Jacobian_Coordinates_Plain<T> &b)
{
    Complex_Plain<T> bZ3;
    Complex<T> S1, S2, U1, U2;
    this->sqr(S2, a.Z);
    mul_complex_plain(bZ3, b.Z, b.Z);

    this->mul_plain(U2, S2, b.X);  //U2 = X2*Z1^2
    this->mul_plain(U1, a.X, bZ3); //U1 = X1*Z2^2

    Complex<T> S;
    this->mul_plain(S, S2, b.Y);
    this->mul(S2, S, a.Z); // S2 = Y2*Z1^3

    mul_complex_plain_inplace(bZ3, b.Z);

    this->mul_plain(S1, a.Y, bZ3); // S1 = Y1*Z2^3

    Complex<T> R, H, V, G;
    this->sub(R, S1, S2); //R = S1-S2
    this->sub(H, U1, U2); // H = U1-U2

    this->sqr(V, H);
    this->mul(G, V, H);       //G = H^3
    this->mul_inplace(V, U1); // V = U1*H^2

    this->mul_plain(c.Z, a.Z, b.Z);
    this->mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Complex<T> tmp;
    this->sqr(tmp, R);

    this->add(c.X, tmp, G);
    this->sub_inplace(c.X, V);
    this->sub_inplace(c.X, V); //X3 = R^2+G-2V

    this->mul(tmp, S1, G);
    this->sub(c.Y, V, c.X);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

template <class T>
void G2Op<T>::add_plain_jac_inplace(G2_Jacobian_Coordinates<T> &c,
                                    const G2_Jacobian_Coordinates_Plain<T> &a)
{
    G2_Jacobian_Coordinates<T> tmp;
    add_plain_jac(tmp, c, a);
    c = tmp;
}

template <class T>
void G2Op<T>::add_jac(G2_Jacobian_Coordinates<T> &c,
                      const G2_Jacobian_Coordinates<T> &a,
                      const G2_Jacobian_Coordinates<T> &b)
{
    Complex<T> S1, S2, U1, U2;
    this->sqr(S2, a.Z);
    this->sqr(S1, b.Z);

    this->mul(U2, b.X, S2); //U2 = X2*Z1^2
    this->mul(U1, a.X, S1); //U1 = X1*Z2^2

    this->mul_inplace(S2, b.Y);
    this->mul_inplace(S2, a.Z); // S2 = Y2*Z1^3

    this->mul_inplace(S1, a.Y);
    this->mul_inplace(S1, b.Z); // S1 = Y1*Z2^3

    Complex<T> R, H, V, G;
    this->sub(R, S1, S2); //R = S1-S2
    this->sub(H, U1, U2); // H = U1-U2

    this->sqr(V, H);
    this->mul(G, V, H);       //G = H^3
    this->mul_inplace(V, U1); // V = U1*H^2

    this->mul(c.Z, a.Z, b.Z);
    this->mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Complex<T> tmp;
    this->sqr(tmp, R);

    this->add(c.X, tmp, G);
    this->sub_inplace(c.X, V);
    this->sub_inplace(c.X, V); //X3 = R^2+G-2V

    this->mul(tmp, S1, G);
    this->sub(c.Y, V, c.X);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

template <class T>
void G2Op<T>::add_jac_inplace(G2_Jacobian_Coordinates<T> &c,
                              const G2_Jacobian_Coordinates<T> &a)
{
    G2_Jacobian_Coordinates<T> tmp;
    add_jac(tmp, c, a);
    c = tmp;
}

template <class T>
void G2Op<T>::add_plain_proj(G2_Projective_Coordinates<T> &c,
                             const G2_Projective_Coordinates<T> &a,
                             const G2_Projective_Coordinates_Plain<T> &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Complex<T> U1, U2, S1, S2;
    this->mul_plain(S2, a.Z, b.Y); //S2 = Y2 * Z1
    this->mul_plain(S1, a.Y, b.Z); //S1 = Y1 * Z2
    this->mul_plain(U2, a.Z, b.X); //U2 = X2 * Z1
    this->mul_plain(U1, a.X, b.Z); //U1 = X1 * Z2

    Complex<T> T_, H, R;
    this->sub(R, S1, S2);          //R = S1 - S2
    this->sub(H, U1, U2);          //H = U1 - U2
    this->mul_plain(T_, a.Z, b.Z); //T = Z1 * Z2

    Complex<T> G, V, W;
    Complex<T> R2, H2;

    this->sqr(H2, H);     //H2 = H^2
    this->mul(V, U1, H2); //V = U1*H^2

    this->mul(G, H2, H); // G = H^3

    this->sqr(R2, R); //R2 = R^2
    this->mul(W, R2, T_);
    this->add_inplace(W, G);
    this->sub_inplace(W, V);
    this->sub_inplace(W, V); // W = R^2*T + G - 2V

    this->mul(c.Z, T_, G); //Z3 = T * G

    Complex<T> tmp;
    this->mul(tmp, S1, G);
    this->sub(c.Y, V, W);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    this->mul(c.X, H, W); // X3 = H * W
}

template <class T>
void G2Op<T>::add_plain_proj_inplace(G2_Projective_Coordinates<T> &c,
                                     const G2_Projective_Coordinates_Plain<T> &a)
{
    G2_Projective_Coordinates<T> tmp;
    add_plain_proj(tmp, c, a);
    c = tmp;
}

template <class T>
void G2Op<T>::add_proj(G2_Projective_Coordinates<T> &c,
                       const G2_Projective_Coordinates<T> &a,
                       const G2_Projective_Coordinates<T> &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Complex<T> U1, U2, S1, S2;
    this->mul(S2, b.Y, a.Z); //S2 = Y2 * Z1
    this->mul(S1, a.Y, b.Z); //S1 = Y1 * Z2
    this->mul(U2, b.X, a.Z); //U2 = X2 * Z1
    this->mul(U1, a.X, b.Z); //U1 = X1 * Z2

    Complex<T> T_, H, R;
    this->sub(R, S1, S2);    //R = S1 - S2
    this->sub(H, U1, U2);    //H = U1 - U2
    this->mul(T_, a.Z, b.Z); //T = Z1 * Z2

    Complex<T> G, V, W;
    Complex<T> R2, H2;

    this->sqr(H2, H);     //H2 = H^2
    this->mul(V, U1, H2); //V = U1*H^2

    this->mul(G, H2, H); // G = H^3

    this->sqr(R2, R); //R2 = R^2
    this->mul(W, R2, T_);
    this->add_inplace(W, G);
    this->sub_inplace(W, V);
    this->sub_inplace(W, V); // W = R^2*T + G - 2V

    this->mul(c.Z, T_, G); //Z3 = T * G

    Complex<T> tmp;
    this->mul(tmp, S1, G);
    this->sub(c.Y, V, W);
    this->mul_inplace(c.Y, R);
    this->sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    this->mul(c.X, H, W); // X3 = H * W
}

template <class T>
void G2Op<T>::add_proj_inplace(G2_Projective_Coordinates<T> &c,
                               const G2_Projective_Coordinates<T> &a)
{
    G2_Projective_Coordinates<T> tmp;
    add_proj(tmp, c, a);
    c = tmp;
}

template <class T>
void G2Op<T>::reveal_and_print(const G2_Affine_Coordinates<T> ac)
{
    cout << "x: " << endl;
    OnlineOp<T>::reveal_and_print({ac.x});
    cout << "y: " << endl;
    OnlineOp<T>::reveal_and_print({ac.y});
}

template <typename T>
template <class V>
void G2Op<T>::reveal_and_print(const V p)
{
    cout << "X: " << endl;
    OnlineOp<T>::reveal_and_print({p.X});
    cout << "Y: " << endl;
    OnlineOp<T>::reveal_and_print({p.Y});
    cout << "Z: " << endl;
    OnlineOp<T>::reveal_and_print({p.Z});
}

//------test--------//
template <class T>
void G2Op<T>::G2_test_add_aff()
{
    PRINT_TEST_BEG();

    vector<vector<string>> point1(2, vector<string>(2)), point2(2, vector<string>(2));
    point1[0][0] = "2480630520447434459745452474224673571710954591933549968516737967418913165838049948371936430061723054690854233264184";
    point1[0][1] = "3644469270405057610286350249996588958371264130321941529490169899587077020220364734985787921562794068692573854501481";
    point1[1][0] = "1331800231700325429192215098048620074430720732324609804590563532772919675450158559950696523029296904480866114179517";
    point1[1][1] = "588900668981719074254304107998469524184236332635267773028573814801942986454206480288099393514359687714746219871656";

    point2[0][0] = "2536380239466922628470206732310665688174601869351732874387718117230652721298290649516310514212623300628740169159805";
    point2[0][1] = "680798372151742087638344386270749882234991936103419105455749081205553938870445287693249697293445242032096602347154";
    point2[1][0] = "2496542435097006551520642381398918251422033459118315432610085277783037480240104245041384280741212331277150387647414";
    point2[1][1] = "2036988673135752780119671948264754463514104930634625481515583855300300477160332414971334065417292937999677606499282";

    Complex_Plain<T> g1x, g1y, g2x, g2y;
    this->str_to_gfp(g1x.real, point1[0][0]);
    this->str_to_gfp(g1x.imag, point1[0][1]);
    this->str_to_gfp(g1y.real, point1[1][0]);
    this->str_to_gfp(g1y.imag, point1[1][1]);

    this->str_to_gfp(g2x.real, point2[0][0]);
    this->str_to_gfp(g2x.imag, point2[0][1]);
    this->str_to_gfp(g2y.real, point2[1][0]);
    this->str_to_gfp(g2y.imag, point2[1][1]);

    G2_Affine_Coordinates_Plain<T> acp(g2x, g2y);

    vector<G2_Affine_Coordinates<T>> ac(2);
    this->get_inputs(0, ac[0].x, g1x);
    this->get_inputs(0, ac[0].y, g1y);

    this->get_inputs(1, ac[1].x, g2x);
    this->get_inputs(1, ac[1].y, g2y);

    cout << "--------test add_affine--------" << endl;
    G2_Affine_Coordinates<T> res(ac[0]);
    reveal_and_print(res);
    cout << "*******\n";
    reveal_and_print(ac[1]);
    cout << "*******\n";
    add_aff_inplace(res, ac[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = ac[0];
    add_plain_aff_inplace(res, acp);
    reveal_and_print(res);

    PRINT_TEST_END();
}

template <class T>
void G2Op<T>::G2_test_add_jac()
{
    PRINT_TEST_BEG();

    vector<vector<string>> point1(3, vector<string>(2)), point2(3, vector<string>(2));
    point1[0][0] = "3639014772220302969239478380372569697853575812585274288911649265451082883565179365094575093493765591320603987709986";
    point1[0][1] = "3627009162346736306815369238116921210498219699819586442010150505939503082090644552148957421881475528065715370270354";
    point1[1][0] = "581916622597845089745272661946997319695944788597773787799484754377746100203623348158129927080369032982420039191898";
    point1[1][1] = "2060995263553188122589792738829838608534517245971325324573983541174135610487090208942105316431342337272892000140574";
    point1[2][0] = "1379037705404560079908242963533160741216777276968477553205923477910255218220226843227833291221018680599293919690139";
    point1[2][1] = "1222016848140062171998821910705868456778091862872885965746947445153530098486346123336986882945598612334224853939670";

    point2[0][0] = "2057737302707558765860213632248591496828971052531394884004744357683286127161347650010073361956086815484639976049181";
    point2[0][1] = "2100089152630115104004378918944334063291538741442441275914861643724932134229574826973214215180219764947165540975080";
    point2[1][0] = "2761046348939817006370345598619575389565977388607130703090367184754327681186304224286762185646017408040537184404026";
    point2[1][1] = "3591610773756270231096263094409160844816105827629891713449617733426148420079417406316994217582407782801806318076355";
    point2[2][0] = "361245890999868516264908086411891038264258441593807793882550805776209472941545014795641271868722093733067921730065";
    point2[2][1] = "578345463844648614472854569382514076495684244079273140322468994353640333794696894886030050136893687175790979496041";

    Complex_Plain<T> g1x, g1y, g1z, g2x, g2y, g2z;
    this->str_to_gfp(g1x.real, point1[0][0]);
    this->str_to_gfp(g1x.imag, point1[0][1]);
    this->str_to_gfp(g1y.real, point1[1][0]);
    this->str_to_gfp(g1y.imag, point1[1][1]);
    this->str_to_gfp(g1z.real, point1[2][0]);
    this->str_to_gfp(g1z.imag, point1[2][1]);

    this->str_to_gfp(g2x.real, point2[0][0]);
    this->str_to_gfp(g2x.imag, point2[0][1]);
    this->str_to_gfp(g2y.real, point2[1][0]);
    this->str_to_gfp(g2y.imag, point2[1][1]);
    this->str_to_gfp(g2z.real, point2[2][0]);
    this->str_to_gfp(g2z.imag, point2[2][1]);

    G2_Jacobian_Coordinates_Plain<T> jcp(g2x, g2y, g2z);

    vector<G2_Jacobian_Coordinates<T>> jc(2);
    this->get_inputs(0, jc[0].X, g1x);
    this->get_inputs(0, jc[0].Y, g1y);
    this->get_inputs(0, jc[0].Z, g1z);

    this->get_inputs(1, jc[1].X, g2x);
    this->get_inputs(1, jc[1].Y, g2y);
    this->get_inputs(1, jc[1].Z, g2z);
    cout << "--------test add_affine--------" << endl;
    G2_Jacobian_Coordinates<T> res(jc[0]);
    reveal_and_print(res);
    cout << "*******\n";
    reveal_and_print(jc[1]);
    cout << "*******\n";
    add_jac_inplace(res, jc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = jc[0];
    add_plain_jac_inplace(res, jcp);
    reveal_and_print(res);

    PRINT_TEST_END();
}

template <class T>
void G2Op<T>::G2_test_add_proj()
{
    PRINT_TEST_BEG();

    vector<vector<string>> point1(3, vector<string>(2)), point2(3, vector<string>(2));
    point2[0][0] = "1742093039426388910190435408185630482619497668278122082509857459325558330158451672629801719804701484028492682526291";
    point2[0][1] = "844703880925914646774744081548865505412209193553910985816318417485674697200984506935957160322668798027043802952303";
    point2[1][0] = "886002647127108382434773171638950877214868523779104236547288994419587243302051950401110151999988279507265248235686";
    point2[1][1] = "3377670356375302327459278403529354819258797950989429935213121031663801556068424700975975120253176749808691093552048";
    point2[2][0] = "2630044192524139568755829768930914914221922846505374541443537841762745333099249593410606002407292719971660309540974";
    point2[2][1] = "3867069001915091933191910692347501925462258957313662369839348613667730277546742556080996614417222707372671590205646";

    point1[0][0] = "3905495996787692004730516086895821758010202147727762943368890547004751861471157171212649017967856603544200875724790";
    point1[0][1] = "1157834469049248978164483191512688211576959690666440817852480432121023210997444090346792959493600922482036051129174";
    point1[1][0] = "3341032349751895186884236415391138660409838976968288192840177736900302021192360735422013174728454024568655317845563";
    point1[1][1] = "3403535857021018318686095448339710033717697760753446140800389264251499680756335325809264393987506974305023600038344";
    point1[2][0] = "1785740032758445692831522852578074777924927255666024331680384440328664732774812966283795321631473509691496372046987";
    point1[2][1] = "3434379341704883699773086001133464230110209518970604638631997996547497569874240408424089912586478220784695002343712";

    Complex_Plain<T> g1x, g1y, g1z, g2x, g2y, g2z;
    this->str_to_gfp(g1x.real, point1[0][0]);
    this->str_to_gfp(g1x.imag, point1[0][1]);
    this->str_to_gfp(g1y.real, point1[1][0]);
    this->str_to_gfp(g1y.imag, point1[1][1]);
    this->str_to_gfp(g1z.real, point1[2][0]);
    this->str_to_gfp(g1z.imag, point1[2][1]);

    this->str_to_gfp(g2x.real, point2[0][0]);
    this->str_to_gfp(g2x.imag, point2[0][1]);
    this->str_to_gfp(g2y.real, point2[1][0]);
    this->str_to_gfp(g2y.imag, point2[1][1]);
    this->str_to_gfp(g2z.real, point2[2][0]);
    this->str_to_gfp(g2z.imag, point2[2][1]);

    G2_Projective_Coordinates_Plain<T> pcp(g2x, g2y, g2z);

    vector<G2_Projective_Coordinates<T>> pc(2);
    this->get_inputs(0, pc[0].X, g1x);
    this->get_inputs(0, pc[0].Y, g1y);
    this->get_inputs(0, pc[0].Z, g1z);

    this->get_inputs(1, pc[1].X, g2x);
    this->get_inputs(1, pc[1].Y, g2y);
    this->get_inputs(1, pc[1].Z, g2z);
    cout << "--------test add_affine--------" << endl;
    G2_Projective_Coordinates<T> res(pc[0]);
    reveal_and_print(res);
    cout << "*******\n";
    reveal_and_print(pc[1]);
    cout << "*******\n";
    add_proj_inplace(res, pc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = pc[0];
    add_plain_proj_inplace(res, pcp);
    reveal_and_print(res);

    PRINT_TEST_END();
}