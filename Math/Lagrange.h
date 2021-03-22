#pragma once
#include "bls12_381.h"

typedef mclBnFr elem_type;

// the Lagrange Coefficient of i for x = 0, i.e., l_i(0)

void get_lagrange_coeff(elem_type &lc, unsigned int n, unsigned int i);

void set_share(vector<elem_type> &sh, const elem_type &s, unsigned int n, unsigned int t);

void recover_share(elem_type &secret, const vector<elem_type> &coeff, const vector<elem_type> &share);
