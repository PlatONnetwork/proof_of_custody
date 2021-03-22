/*
 * gf2nSquare.h
 *
 */

#ifndef MATH_SQUARE_H_
#define MATH_SQUARE_H_

#include "Tools/BitVector.h"

template<class U>
class Square
{
public:
    typedef U RowType;

    static const int N_ROWS = U::MAX_N_BITS;
    static const int N_ROWS_ALLOCATED = N_ROWS;
    static const int N_COLUMNS = N_ROWS;
    static const int N_ROW_BYTES = N_ROWS / 8;

    static size_t size() { return N_ROWS * U::size(); }

    U rows[N_ROWS];

    Square& sub(const Square& other);
    Square& rsub(const Square& other);
    Square& sub(const void* other);

    void randomize(int row, PRNG& G) { rows[row].randomize(G); }
    void conditional_add(BitVector& conditions, Square& other,
            int offset);
    void to(U& result);
    void to(U& result, false_type);
    template<int X, int L>
    void to(gfp_<X, L>& result, true_type);

    void pack(octetStream& os) const;
    void unpack(octetStream& os);
};

#endif /* MATH_SQUARE_H_ */
