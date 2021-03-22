/*
 * Exceptions.cpp
 *
 */

#include "Exceptions.h"
#include "Math/bigint.h"

IO_Error::IO_Error(const string& m)
{
    ans = "IO-Error : " + m;
}

file_error::file_error(const string& m)
{
    ans = "File Error : " + m;
}

Processor_Error::Processor_Error(const string& m)
{
    msg = "Processor-Error : " + m;
}

Processor_Error::Processor_Error(const char* m) :
        Processor_Error(string(m))
{
}

wrong_gfp_size::wrong_gfp_size(const char* name, const bigint& p,
        const char* symbol, int n_limbs) :
        runtime_error(
                string() + name + " wrong size for modulus " + to_string(p)
                        + ". Maybe change " + symbol + " to "
                        + to_string(n_limbs) + ".")
{
}

overflow::overflow(const char* name, size_t i, size_t n) :
        runtime_error(string(name) + " overflow: " + to_string(i) + "/" + to_string(n))
{
}

unknown_input_type::unknown_input_type(int type) :
        runtime_error("unkown type: " + to_string(type))
{
}

invalid_opcode::invalid_opcode(int opcode) :
        runtime_error("invalid opcode: " + to_string(opcode))
{
}

input_error::input_error(const char* name, const string& filename,
        istream& input_file)
{
    input_file.clear();
    string token;
    input_file >> token;
    msg += string() + "cannot read " + name + " from " + filename
            + ", problem with '" + token + "'";
}
