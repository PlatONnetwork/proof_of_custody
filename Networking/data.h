#ifndef _Data
#define _Data

#include <string.h>

#include "Tools/Exceptions.h"
#include "Tools/avx_memcpy.h"
#include "Tools/int.h"

#ifdef __APPLE__
# include <libkern/OSByteOrder.h>
#define htole64(x) OSSwapHostToLittleInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)
#endif

#ifdef __linux__
#include <endian.h>
#endif


inline void encode_length(octet *buff, size_t len, size_t n_bytes)
{
    if (n_bytes > 8)
        throw invalid_length("length field cannot be more than 64 bits");
    if (n_bytes < 8)
    {
        long long upper = len;
        upper >>= (8 * n_bytes);
        if (upper != 0 and upper != -1)
            throw invalid_length("length too large for length field");
    }
    // use little-endian for optimization
    uint64_t tmp = htole64(len);
    avx_memcpy(buff, (void*)&tmp, n_bytes);
}

inline size_t decode_length(octet *buff, size_t n_bytes)
{
    if (n_bytes > 8)
        throw invalid_length("length field cannot be more than 64 bits");
    uint64_t tmp = 0;
    avx_memcpy((void*)&tmp, buff, n_bytes);
    return le64toh(tmp);
}

#endif
