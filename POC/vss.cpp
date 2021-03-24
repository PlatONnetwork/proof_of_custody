#include "vss.h"

void VSS::set_secret(mclBnFr _s, uint32_t pn, uint32_t tn)
{
    s = _s;
    nparty = pn;
    threshold = tn;
}

void VSS::set_secret(mclBnFr _s)
{
    s = _s;
}

mclBnFr VSS::get_secret()
{
    return s;
}

void VSS::rnd_secret()
{
    mclBnFr_setByCSPRNG(&s);
}

void VSS::gen_share(vector<mclBnFr> &shares, vector<mclBnG1> &aux)
{
    mclBnFr *coeff = new mclBnFr[threshold + 1];
    mclBnFr out;
    mclBnFr num;
    mclBnG1 tmpG1;
    mclBnG1 basePoint;
    getBasePointG1(basePoint);

    coeff[0] = s;
    mclBnG1_mul(&tmpG1, &basePoint, &coeff[0]);
    aux.push_back(tmpG1);

    for (int i = 1; i <= threshold; i++)
    {
        mclBnFr_setByCSPRNG(&coeff[i]);
        mclBnG1_mul(&tmpG1, &basePoint, &coeff[i]);
        aux.push_back(tmpG1);
    }

    for (int i = 1; i <= nparty; i++)
    {
        mclBnFr_setInt32(&num, i);
        mclBn_FrEvaluatePolynomial(&out, coeff, threshold + 1, &num);
        shares.push_back(out);
    }

    delete[] coeff;
}

bool VSS::verify_share(mclBnFr share, vector<mclBnG1> aux, uint32_t n)
{
    mclBnG1 ret, retEval;
    mclBnFr s_num;
    mclBnG1 basePoint;
    getBasePointG1(basePoint);

    mclBnG1_mul(&ret, &basePoint, &share);

    mclBnG1 *pcoeff = new mclBnG1[aux.size()];

    for (int i = 0; i < aux.size(); i++)
    {
        pcoeff[i] = aux[i];
    }

    mclBnFr_setInt32(&s_num, n);

    mclBn_G1EvaluatePolynomial(&retEval, pcoeff, aux.size(), &s_num);

    bool res = mclBnG1_isEqual(&ret, &retEval) == 1 ? true : false;

    delete[] pcoeff;

    return res;
}

void recover_share(mclBnFr &out, const vector<mclBnFr> shares)
{
    int len = shares.size();
    mclBnFr *xVec = new mclBnFr[len];
    mclBnFr *yVec = new mclBnFr[len];
    for (int i = 0; i < len; i++)
    {
        mclBnFr_setInt32(&xVec[i], i + 1);
        yVec[i] = shares[i];
    }
    mclBn_FrLagrangeInterpolation(&out, xVec, yVec, len);

    delete[] xVec;
    delete[] yVec;
}

void lagrange_coeff(vector<mclBnFr> &lag_coeff, vector<int> val)
{ //val has non-zero and distinct elements, not optimized.

    vector<mclBnFr> Fr_val;
    mclBnFr nume_prod, deno_prod, tmp, xi;

    for (int i = 0; i < val.size(); i++)
    {
        if (val[i] == 0)
        {
            throw "Elements in val should be non-zero and distinct!";
        }
        mclBnFr_setInt32(&tmp, (-1) * val[i]);
        Fr_val.push_back(tmp);
    }

    for (int i = 0; i < Fr_val.size(); i++)
    {
        mclBnFr_setInt32(&nume_prod, 1);
        mclBnFr_setInt32(&deno_prod, 1);
        mclBnFr_setInt32(&xi, val[i]);

        for (int j = 0; j < Fr_val.size(); j++)
        {
            if (j != i)
            {
                mclBnFr_mul(&nume_prod, &nume_prod, &Fr_val[j]);
                mclBnFr_add(&tmp, &xi, &Fr_val[j]);
                mclBnFr_mul(&deno_prod, &deno_prod, &tmp);
            }
        }
        mclBnFr_inv(&deno_prod, &deno_prod);
        mclBnFr_mul(&nume_prod, &nume_prod, &deno_prod);
        lag_coeff.push_back(nume_prod);
    }
}