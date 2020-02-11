#include "bls12_381.h"
#include <stdio.h>

void getBasePointG1(mclBnG1 &basePoint)
{
    mclBnG1_setStr(&basePoint, (char *)G1_P.c_str(), G1_P.size(), 10);
}

void mclFr_to_G1(mclBnG1 &out, const mclBnFr &in)
{
    mclBnG1 basePoint;
    getBasePointG1(basePoint);
    mclBnG1_mul(&out,&basePoint,&in);
}

void print_mclBnFr(const mclBnFr &a)
{
    char buf[128];
    mclBnFr_getStr(buf, sizeof(buf), &a, 10);
    printf("%s\n", buf);
}

void print_mclBnG1(const mclBnG1 &a)
{
    char buf[256];
    mclBnG1_getStr(buf, sizeof(buf), &a, 10);
    printf("%s\n", buf);
}

void mclBnFr_to_str(string &str, const mclBnFr &a)
{
    str.clear();
    char buf[mclBn_getFrByteSize()];
    mclBnFr_serialize(buf, sizeof(buf), &a);
    str = string(buf, buf + sizeof(buf));
}

void mclBnG1_to_str(string &str, const mclBnG1 &a)
{
    str.clear();
    char buf[mclBn_getG1ByteSize()];
    mclBnG1_serialize(buf, sizeof(buf), &a);
    str = string(buf, buf + sizeof(buf));
}

void str_to_mclBnFr(mclBnFr &out, const string &str)
{
    mclBnFr_deserialize(&out, str.c_str(), mclBn_getFrByteSize());
}

void str_to_mclBnG1(mclBnG1 &out, const string &str)
{
    mclBnG1_deserialize(&out, str.c_str(), mclBn_getG1ByteSize());
}