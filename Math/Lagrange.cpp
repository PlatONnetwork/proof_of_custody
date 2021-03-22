#include "Lagrange.h"

void get_lagrange_coeff(elem_type &lc, unsigned int n, unsigned int pos)
{
    if (pos <= 0 || pos > n)
    {
        throw invalid_length();
    }
    else
    {
        mclBnFr nume_prod, deno_prod, xi, xk;

        mclBnFr_setInt32(&nume_prod, 1);
        mclBnFr_setInt32(&deno_prod, 1);
        mclBnFr_setInt32(&xi, pos);

        for (int k = 1; k <= n; k++)
        {
            if (k != pos)
            {
                mclBnFr_setInt32(&xk, -k);
                mclBnFr_mul(&nume_prod, &nume_prod, &xk);
                mclBnFr_add(&xk, &xi, &xk);
                mclBnFr_mul(&deno_prod, &deno_prod, &xk);
            }
        }
        mclBnFr_inv(&lc, &deno_prod);
        mclBnFr_mul(&lc, &lc, &nume_prod);
    }
}

void set_share(vector<elem_type> &sh, const elem_type &s, unsigned int n, unsigned int t)
{
    vector<elem_type> coeff(t + 1);
    coeff[0] = s;
    for (int k = 1; k < coeff.size(); k++)
    {
        mclBnFr_setByCSPRNG(&coeff[k]);
        //mclBnFr_setInt32(&coeff[k],k+1);
    }

    for (int pos = 0; pos < n; pos++)
    {
        elem_type xi;
        mclBnFr_setInt32(&xi, pos + 1);
        sh[pos] = coeff.back();
        for (int k = coeff.size() - 1; k > 0; k--)
        {
            mclBnFr_mul(&sh[pos], &sh[pos], &xi);
            mclBnFr_add(&sh[pos], &sh[pos], &coeff[k - 1]);
        }
    }
}

void recover_share(elem_type &secret, const vector<elem_type> &coeff, const vector<elem_type> &share)
{
    if (coeff.size() != share.size())
    {
        throw invalid_length();
    }
    else
    {
        mclBnFr_mul(&secret, &coeff[0], &share[0]);
        mclBnFr tmp;
        for (int i = 1; i < coeff.size(); i++)
        {
            mclBnFr_mul(&tmp, &coeff[i], &share[i]);
            mclBnFr_add(&secret, &secret, &tmp);
        }
    }
}