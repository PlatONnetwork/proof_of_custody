/*
 * instructions.h
 *
 */

#ifndef PROCESSOR_INSTRUCTIONS_H_
#define PROCESSOR_INSTRUCTIONS_H_

#include "Instruction.h"

#define ARITHMETIC_INSTRUCTIONS \
    X(LDI, auto dest = &Procp.get_C()[r[0]]; typename sint::clear tmp = int(n), \
            *dest++ = tmp) \
    X(LDSI, auto dest = &Procp.get_S()[r[0]]; \
            auto tmp = sint::constant(int(n), Proc.P.my_num(), Procp.MC.get_alphai()), \
            *dest++ = tmp) \
    X(LDMS, auto dest = &Procp.get_S()[r[0]]; auto source = &Proc.machine.Mp.MS[n], \
            *dest++ = *source++) \
    X(STMS, auto source = &Procp.get_S()[r[0]]; auto dest = &Proc.machine.Mp.MS[n], \
            *dest++ = *source++) \
    X(LDMSI, auto dest = &Procp.get_S()[r[0]]; auto source = &Proc.get_Ci()[r[1]], \
            *dest++ = Proc.machine.Mp.read_S(*source++)) \
    X(STMSI, auto source = &Procp.get_S()[r[0]]; auto dest = &Proc.get_Ci()[r[1]], \
            Proc.machine.Mp.write_S(*dest++, *source++)) \
    X(MOVS, auto dest = &Procp.get_S()[r[0]]; auto source = &Procp.get_S()[r[1]], \
            *dest++ = *source++) \
    X(ADDS, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = &Procp.get_S()[r[2]], \
            *dest++ = *op1++ + *op2++) \
    X(ADDM, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ + sint::constant(*op2++, Proc.P.my_num(), Procp.MC.get_alphai())) \
    X(ADDSI, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]], \
            *dest++ = *op1++ + sint::constant(int(n), Proc.P.my_num(), Procp.MC.get_alphai())) \
    X(ADDC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ + *op2++) \
    X(ADDCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ + op2) \
    X(SUBS, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = &Procp.get_S()[r[2]], \
            *dest++ = *op1++ - *op2++) \
    X(SUBSI, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = sint::constant(int(n), Proc.P.my_num(), Procp.MC.get_alphai()), \
            *dest++ = *op1++ - op2) \
    X(SUBSFI, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = sint::constant(int(n), Proc.P.my_num(), Procp.MC.get_alphai()), \
            *dest++ = op2 - *op1++) \
    X(SUBML, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ - sint::constant(*op2++, Proc.P.my_num(), Procp.MC.get_alphai())) \
    X(SUBMR, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_S()[r[2]], \
            *dest++ = sint::constant(*op1++, Proc.P.my_num(), Procp.MC.get_alphai()) - *op2++) \
    X(SUBC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ - *op2++) \
    X(SUBCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ - op2) \
    X(SUBCFI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = op2 - *op1++) \
    X(MULM, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ * *op2++) \
    X(MULC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ * *op2++) \
    X(MULCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ * op2) \
    X(MULSI, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ * op2) \
    X(ANDC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ & *op2++) \
    X(XORC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ ^ *op2++) \
    X(ORC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ | *op2++) \
    X(ANDCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ & op2) \
    X(XORCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ ^ op2) \
    X(ORCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            typename sint::clear op2 = int(n), \
            *dest++ = *op1++ | op2) \
    X(SHLC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ << *op2++) \
    X(SHRC, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]]; \
            auto op2 = &Procp.get_C()[r[2]], \
            *dest++ = *op1++ >> *op2++) \
    X(SHLCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]], \
            *dest++ = *op1++ << n) \
    X(SHRCI, auto dest = &Procp.get_C()[r[0]]; auto op1 = &Procp.get_C()[r[1]], \
            *dest++ = *op1++ >> n) \
    X(TRIPLE, auto a = &Procp.get_S()[r[0]]; auto b = &Procp.get_S()[r[1]]; \
            auto c = &Procp.get_S()[r[2]], \
            Procp.DataF.get_three(DATA_TRIPLE, *a++, *b++, *c++)) \
    X(BIT, auto dest = &Procp.get_S()[r[0]], \
            Procp.DataF.get_one(DATA_BIT, *dest++)) \
    X(RANDOMFULLS, auto dest = &Procp.get_S()[r[0]], \
            *dest++ = Procp.DataF.get_random()) \
    X(GLDSI, auto dest = &Proc2.get_S()[r[0]]; \
            auto tmp = sgf2n::constant(int(n), Proc.P.my_num(), Proc2.MC.get_alphai()), \
            *dest++ = tmp) \
    X(GLDMS, auto dest = &Proc2.get_S()[r[0]]; auto source = &Proc.machine.M2.MS[n], \
            *dest++ = *source++) \
    X(GSTMS, auto source = &Proc2.get_S()[r[0]]; auto dest = &Proc.machine.M2.MS[n], \
            *dest++ = *source++) \
    X(GLDMSI, auto dest = &Proc2.get_S()[r[0]]; auto source = &Proc.get_Ci()[r[1]], \
            *dest++ = Proc.machine.M2.read_S(*source++)) \
    X(GSTMSI, auto source = &Proc2.get_S()[r[0]]; auto dest = &Proc.get_Ci()[r[1]], \
            Proc.machine.M2.write_S(*dest++, *source++)) \
    X(GMOVS, auto dest = &Proc2.get_S()[r[0]]; auto source = &Proc2.get_S()[r[1]], \
            *dest++ = *source++) \
    X(GADDS, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = &Proc2.get_S()[r[2]], \
            *dest++ = *op1++ + *op2++) \
    X(GADDM, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = &Proc2.get_C()[r[2]], \
            *dest++ = *op1++ + sgf2n::constant(*op2++, Proc.P.my_num(), Proc2.MC.get_alphai())) \
    X(GADDSI, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]], \
            *dest++ = *op1++ + sgf2n::constant(int(n), Proc.P.my_num(), Proc2.MC.get_alphai())) \
    X(GSUBS, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = &Proc2.get_S()[r[2]], \
            *dest++ = *op1++ - *op2++) \
    X(GSUBSI, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = sgf2n::constant(int(n), Proc.P.my_num(), Proc2.MC.get_alphai()), \
            *dest++ = *op1++ - op2) \
    X(GSUBSFI, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = sgf2n::constant(int(n), Proc.P.my_num(), Proc2.MC.get_alphai()), \
            *dest++ = op2 - *op1++) \
    X(GSUBML, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = &Proc2.get_C()[r[2]], \
            *dest++ = *op1++ - sgf2n::constant(*op2++, Proc.P.my_num(), Proc2.MC.get_alphai())) \
    X(GSUBMR, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_C()[r[1]]; \
            auto op2 = &Proc2.get_S()[r[2]], \
            *dest++ = sgf2n::constant(*op1++, Proc.P.my_num(), Proc2.MC.get_alphai()) - *op2++) \
    X(GMULM, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            auto op2 = &Proc2.get_C()[r[2]], \
            *dest++ = *op1++ * *op2++) \
    X(GMULSI, auto dest = &Proc2.get_S()[r[0]]; auto op1 = &Proc2.get_S()[r[1]]; \
            typename sgf2n::clear op2 = int(n), \
            *dest++ = *op1++ * op2) \
    X(GTRIPLE, auto a = &Proc2.get_S()[r[0]]; auto b = &Proc2.get_S()[r[1]]; \
            auto c = &Proc2.get_S()[r[2]], \
            Proc2.DataF.get_three(DATA_TRIPLE, *a++, *b++, *c++)) \
    X(GBIT, auto dest = &Proc2.get_S()[r[0]], \
            Proc2.DataF.get_one(DATA_BIT, *dest++)) \
    X(CONVINT, auto dest = &Procp.get_C()[r[0]]; auto source = &Proc.get_Ci()[r[1]], \
            *dest++ = *source++) \
    X(GCONVINT, auto dest = &Proc2.get_C()[r[0]]; auto source = &Proc.get_Ci()[r[1]], \
            *dest++ = *source++) \

#define REGINT_INSTRUCTIONS \
    X(LDMINT, auto dest = &Proc.get_Ci()[r[0]]; auto source = &Mi[n], \
            *dest++ = (*source).get(); source++) \
    X(STMINT, auto dest = &Mi[n]; auto source = &Proc.get_Ci()[r[0]], \
            *dest++ = *source++) \
    X(LDMINTI, auto dest = &Proc.get_Ci()[r[0]]; auto source = &Ci[r[1]], \
            *dest++ = Mi[*source].get(); source++) \
    X(STMINTI, auto dest = &Proc.get_Ci()[r[1]]; auto source = &Ci[r[0]], \
            Mi[*dest] = *source++; dest++) \
    X(MOVINT, auto dest = &Proc.get_Ci()[r[0]]; auto source = &Ci[r[1]], \
            *dest++ = *source++) \
    X(PUSHINT, Proc.pushi(Ci[r[0]]),) \
    X(POPINT, auto dest = &Ci[r[0]], Proc.popi(*dest++)) \
    X(LDTN, auto dest = &Ci[r[0]], *dest++ = Proc.get_thread_num()) \
    X(LDARG, auto dest = &Ci[r[0]], *dest++ = Proc.get_arg()) \
    X(STARG, Proc.set_arg(Ci[r[0]]),) \
    X(LDINT, auto dest = &Proc.get_Ci()[r[0]], \
            *dest++ = int(n)) \
    X(ADDINT, auto dest = &Proc.get_Ci()[r[0]]; auto op1 = &Proc.get_Ci()[r[1]]; \
            auto op2 = &Proc.get_Ci()[r[2]], \
            *dest++ = *op1++ + *op2++) \
    X(SUBINT, auto dest = &Proc.get_Ci()[r[0]]; auto op1 = &Proc.get_Ci()[r[1]]; \
            auto op2 = &Proc.get_Ci()[r[2]], \
            *dest++ = *op1++ - *op2++) \
    X(MULINT, auto dest = &Proc.get_Ci()[r[0]]; auto op1 = &Proc.get_Ci()[r[1]]; \
            auto op2 = &Proc.get_Ci()[r[2]], \
            *dest++ = *op1++ * *op2++) \
    X(DIVINT, auto dest = &Proc.get_Ci()[r[0]]; auto op1 = &Proc.get_Ci()[r[1]]; \
            auto op2 = &Proc.get_Ci()[r[2]], \
            *dest++ = *op1++ / *op2++) \
    X(INCINT, auto dest = &Proc.get_Ci()[r[0]]; auto base = Proc.get_Ci()[r[1]], \
            int inc = (i / start[0]) % start[1]; *dest++ = base + inc * int(n)) \
    X(EQZC, auto dest = &Ci[r[0]]; auto source = &Ci[r[1]], *dest++ = *source++ == 0) \
    X(LTZC, auto dest = &Ci[r[0]]; auto source = &Ci[r[1]], *dest++ = *source++ < 0) \
    X(LTC, auto dest = &Ci[r[0]]; auto op1 = &Ci[r[1]]; auto op2 = &Ci[r[2]], \
            *dest++ = *op1++ < *op2++) \
    X(GTC, auto dest = &Ci[r[0]]; auto op1 = &Ci[r[1]]; auto op2 = &Ci[r[2]], \
            *dest++ = *op1++ > *op2++) \
    X(EQC, auto dest = &Ci[r[0]]; auto op1 = &Ci[r[1]]; auto op2 = &Ci[r[2]], \
            *dest++ = *op1++ == *op2++) \
    X(PRINTINT, Proc.out << Proc.read_Ci(r[0]) << flush,) \
    X(PRINTFLOATPREC, Proc.out << setprecision(n),) \
    X(PRINTSTR, Proc.out << string((char*)&n,sizeof(n)) << flush,) \
    X(PRINTCHR, Proc.out << string((char*)&n,1) << flush,) \
    X(SHUFFLE, shuffle(Proc),) \
    X(BITDECINT, bitdecint(Proc),) \
    X(RAND, auto dest = &Ci[r[0]]; auto source = &Ci[r[1]], \
            *dest++ = Proc.shared_prng.get_uint() % (1 << *source++)) \

#define CLEAR_GF2N_INSTRUCTIONS \
    X(GLDI, auto dest = &C2[r[0]]; cgf2n tmp = int(n), \
            *dest++ = tmp) \
    X(GLDMC, auto dest = &C2[r[0]]; auto source = &M2C[n], \
            *dest++ = (*source).get(); source++) \
    X(GSTMC, auto dest = &M2C[n]; auto source = &C2[r[0]], \
            *dest++ = *source++) \
    X(GLDMCI, auto dest = &C2[r[0]]; auto source = &Proc.get_Ci()[r[1]], \
            *dest++ = M2C[*source++]) \
    X(GSTMCI, auto dest = &Proc.get_Ci()[r[1]]; auto source = &C2[r[0]], \
            M2C[*dest++] = *source++) \
    X(GMOVC, auto dest = &C2[r[0]]; auto source = &C2[r[1]], \
            *dest++ = *source++) \
    X(GADDC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            auto op2 = &C2[r[2]], \
            *dest++ = *op1++ + *op2++) \
    X(GADDCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = *op1++ + op2) \
    X(GSUBC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            auto op2 = &C2[r[2]], \
            *dest++ = *op1++ - *op2++) \
    X(GSUBCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = *op1++ - op2) \
    X(GSUBCFI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = op2 - *op1++) \
    X(GMULC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            auto op2 = &C2[r[2]], \
            *dest++ = *op1++ * *op2++) \
    X(GMULCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = *op1++ * op2) \
    X(GANDC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            auto op2 = &C2[r[2]], \
            *dest++ = *op1++ & *op2++) \
    X(GXORC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            auto op2 = &C2[r[2]], \
            *dest++ = *op1++ ^ *op2++) \
    X(GORC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            auto op2 = &C2[r[2]], \
            *dest++ = *op1++ | *op2++) \
    X(GANDCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = *op1++ & op2) \
    X(GXORCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = *op1++ ^ op2) \
    X(GORCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]]; \
            cgf2n op2 = int(n), \
            *dest++ = *op1++ | op2) \
    X(GNOTC, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]], \
            *dest++ = ~*op1++) \
    X(GSHLCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]], \
            *dest++ = *op1++ << n) \
    X(GSHRCI, auto dest = &C2[r[0]]; auto op1 = &C2[r[1]], \
            *dest++ = *op1++ >> n) \
    X(GPRINTREG, auto source = &C2[r[0]], \
            Proc.out << "Reg[" << r[0] << "] = " << *source++ \
            << " # " << string((char*)&n,sizeof(n)) << endl) \
    X(GPRINTREGPLAIN, auto source = &C2[r[0]], \
            Proc.out << *source++ << flush) \
    X(GBITDEC, gbitdec(C2),) \
    X(GBITCOM, gbitcom(C2),) \
    X(GCONVGF2N, auto dest = &Proc.get_Ci()[r[0]]; auto source = &C2[r[1]], \
            *dest++ = source->get_word(); source++) \
    X(GRAWOUTPUT, auto source = &C2[r[0]], \
            (*source++).output(Proc.public_output, false)) \

#endif /* PROCESSOR_INSTRUCTIONS_H_ */
