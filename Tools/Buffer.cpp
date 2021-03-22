/*
 * Buffer.cpp
 *
 */

#include "Tools/Buffer.h"

#include <unistd.h>

bool BufferBase::rewind = false;


void BufferBase::setup(ifstream* f, int length, const string& filename,
        const char* type, const string& field)
{
    file = f;
    tuple_length = length;
    data_type = type;
    field_type = field;
    this->filename = filename;
}

void BufferBase::seekg(int pos)
{
    if (not file)
        file = open();
    file->seekg(pos * tuple_length);
    if (file->eof() || file->fail())
    {
        // let it go in case we don't need it anyway
        if (pos != 0)
            try_rewind();
    }
    next = BUFFER_SIZE;
}

void BufferBase::try_rewind()
{
#ifndef INSECURE
    string type;
    if (field_type.size() and data_type.size())
        type = (string)" of " + field_type + " " + data_type;
    throw not_enough_to_buffer(type);
#endif
    file->clear(); // unset EOF flag
    file->seekg(0);
    if (file->peek() == ifstream::traits_type::eof())
        throw runtime_error("empty file: " + filename);
    if (!rewind)
        cerr << "REWINDING - ONLY FOR BENCHMARKING" << endl;
    rewind = true;
    eof = true;
}

void BufferBase::prune()
{
    if (file and file->tellg() != 0)
    {
        cerr << "Pruning " << filename << endl;
        string tmp_name = filename + ".new";
        ofstream tmp(tmp_name.c_str());
        tmp << file->rdbuf();
        if (tmp.fail())
            throw runtime_error("problem writing to " + tmp_name);
        tmp.close();
        file->close();
        rename(tmp_name.c_str(), filename.c_str());
        file->open(filename.c_str(), ios::in | ios::binary);
    }
}

void BufferBase::purge()
{
    if (file)
    {
        cerr << "Removing " << filename << endl;
        unlink(filename.c_str());
        file->close();
        file = 0;
    }
}

void BufferBase::check_tuple_length(int tuple_length)
{
    if (tuple_length != this->tuple_length)
        throw Processor_Error("inconsistent tuple length");
}
