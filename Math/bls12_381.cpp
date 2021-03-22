#include "bls12_381.h"
#include <stdio.h>
#include "Tools/Exceptions.h"

void getBasePointG1(mclBnG1 &basePoint)
{
    mclBnG1_setStr(&basePoint, (char *)G1_P.c_str(), G1_P.size(), 10);
}

void mclFr_to_G1(mclBnG1 &out, const mclBnFr &in)
{
    mclBnG1 basePoint;
    getBasePointG1(basePoint);
    mclBnG1_mul(&out, &basePoint, &in);
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

void mclBnG2_to_str(vector<string> &out, const mclBnG2 &a)
{
    if (out.size() != 4)
    {
        out.resize(4);
    }
    char buf[512];
    int len = mclBnG2_getStr(buf, 512, &a, 10);
    string str(buf + 2, buf + len);
    len = len - 2;

    int l;

    for (int i = 0; i < out.size() - 1; i++)
    {
        l = str.find(" ");
        out[i] = str.substr(0, l);
        str = str.substr(l + 1);
    }
    out.back() = str;
}

// void mclBnG2_to_gfp(vector<gfp> &out, const mclBnG2 &a)
// {
//     vector<string> str;
//     mclBnG2_to_str(str, a);
//     out.resize(str.size());
    
//     for (int i = 0; i < out.size(); i++)
//     {
//         bigint bn(str[i], 10);
//         to_gfp(out[i], bn);
//     }
// }

void print_mclBnG2(const mclBnG2 &a)
{
    vector<string> out;
    mclBnG2_to_str(out,a);
    for(int i = 0; i < out.size(); i++)
    {
        cout<<out[i]<<endl;
    }
}