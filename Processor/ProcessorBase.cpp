/*
 * ProcessorBase.cpp
 *
 */

#include "ProcessorBase.hpp"

string ProcessorBase::get_parameterized_filename(int my_num, int thread_num, const string& prefix)
{
    string filename = prefix + "-P" + to_string(my_num) + "-" + to_string(thread_num);
    return filename;
}

void ProcessorBase::open_input_file(int my_num, int thread_num,
        const string& prefix)
{
    string tmp = prefix;
    if (prefix.empty())
        tmp = "Player-Data/Input";

    open_input_file(get_parameterized_filename(my_num, thread_num, tmp));
}

void ProcessorBase::setup_redirection(int my_num, int thread_num,
		OnlineOptions& opts)
{
    if (not opts.cmd_private_output_file.empty())
    {
        const string stdout_filename = get_parameterized_filename(my_num,
                thread_num, opts.cmd_private_output_file);
        stdout_redirect_file.open(stdout_filename.c_str(), ios_base::out);
    }
}
