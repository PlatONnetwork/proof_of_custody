#ifndef PROCESSOR_PROCESSOR_HPP_
#define PROCESSOR_PROCESSOR_HPP_

#include "Processor/Processor.h"
#include "Processor/Program.h"
#include "GC/square64.h"

#include "Protocols/ReplicatedInput.hpp"
#include "Protocols/ReplicatedPrivateOutput.hpp"
#include "Processor/ProcessorBase.hpp"
#include "GC/Processor.hpp"
#include "GC/ShareThread.hpp"

#include <sodium.h>
#include <string>

template <class T>
SubProcessor<T>::SubProcessor(ArithmeticProcessor& Proc, typename T::MAC_Check& MC,
    Preprocessing<T>& DataF, Player& P) :
    SubProcessor<T>(MC, DataF, P, &Proc)
{
}

template <class T>
SubProcessor<T>::SubProcessor(typename T::MAC_Check& MC,
    Preprocessing<T>& DataF, Player& P, ArithmeticProcessor* Proc) :
    Proc(Proc), MC(MC), P(P), DataF(DataF), protocol(P), input(*this, MC),
    bit_prep(bit_usage)
{
  DataF.set_proc(this);
  DataF.set_protocol(protocol);
  protocol.init_mul(this);
  bit_usage.set_num_players(P.num_players());
  personal_bit_preps.resize(P.num_players());
  for (int i = 0; i < P.num_players(); i++)
    personal_bit_preps[i] = new typename BT::LivePrep(bit_usage, i);
}

template<class T>
SubProcessor<T>::~SubProcessor()
{
  protocol.check();

  for (size_t i = 0; i < personal_bit_preps.size(); i++)
    {
      auto& x = personal_bit_preps[i];
#ifdef VERBOSE
      if (x->data_sent())
        cerr << "Sent for personal bit preprocessing threads of player " << i << ": " <<
              x->data_sent() * 1e-6 << " MB" << endl;
#endif
      delete x;
    }
#ifdef VERBOSE
  if (bit_prep.data_sent())
    cerr << "Sent for global bit preprocessing threads: " <<
        bit_prep.data_sent() * 1e-6 << " MB" << endl;
  if (not bit_usage.empty())
    {
      cerr << "Mixed-circuit preprocessing cost:" << endl;
      bit_usage.print_cost();
    }
#endif
}

template<class sint, class sgf2n>
Processor<sint, sgf2n>::Processor(int thread_num,Player& P,
        typename sgf2n::MAC_Check& MC2,typename sint::MAC_Check& MCp,
        Machine<sint, sgf2n>& machine,
        const Program& program)
: ArithmeticProcessor(machine.opts, thread_num),DataF(machine, &Procp, &Proc2),P(P),
  MC2(MC2),MCp(MCp),machine(machine),
  share_thread(machine.get_N(), machine.opts, P, machine.get_bit_mac_key(), DataF.usage),
  Procb(machine.bit_memories),
  Proc2(*this,MC2,DataF.DataF2,P),Procp(*this,MCp,DataF.DataFp,P),
  privateOutput2(Proc2),privateOutputp(Procp),
  external_clients(P.my_num()),
  binary_file_io(Binary_File_IO())
{
  reset(program,0);

  public_input_filename = get_filename("Programs/Public-Input/",false);
  public_input.open(public_input_filename);
  private_input_filename = (get_filename(PREP_DIR "Private-Input-",true));
  private_input.open(private_input_filename.c_str());
  public_output.open(get_filename(PREP_DIR "Public-Output-",true).c_str(), ios_base::out);
  private_output.open(get_filename(PREP_DIR "Private-Output-",true).c_str(), ios_base::out);

  open_input_file(P.my_num(), thread_num, machine.opts.cmd_private_input_file);

  secure_prng.ReSeed();
  shared_prng.SeedGlobally(P);

  // only output on party 0 if not interactive
  bool output = P.my_num() == 0 or machine.opts.interactive;
  out.activate(output);
  Procb.out.activate(output);
  setup_redirection(P.my_num(), thread_num, opts);

  if (stdout_redirect_file.is_open())
  {
    out.redirect_to_file(stdout_redirect_file);
    Procb.out.redirect_to_file(stdout_redirect_file);
  }

}


template<class sint, class sgf2n>
Processor<sint, sgf2n>::~Processor()
{
  share_thread.post_run();
#ifdef VERBOSE
  if (sent)
    cerr << "Opened " << sent << " elements in " << rounds << " rounds" << endl;
#endif
}

template<class sint, class sgf2n>
string Processor<sint, sgf2n>::get_filename(const char* prefix, bool use_number)
{
  stringstream filename;
  filename << prefix;
  if (!use_number)
    filename << machine.progname;
  if (use_number)
    filename << P.my_num();
  if (thread_num > 0)
    filename << "-" << thread_num;
#ifdef DEBUG_FILES
  cerr << "Opening file " << filename.str() << endl;
#endif
  return filename.str();
}


template<class sint, class sgf2n>
void Processor<sint, sgf2n>::reset(const Program& program,int arg)
{
  Proc2.get_S().resize(program.num_reg(SGF2N));
  Proc2.get_C().resize(program.num_reg(CGF2N));
  Procp.get_S().resize(program.num_reg(SINT));
  Procp.get_C().resize(program.num_reg(CINT));
  Ci.resize(program.num_reg(INT));
  this->arg = arg;
  Procb.reset(program);
}

template<class sint, class sgf2n>
void Processor<sint, sgf2n>::dabit(const Instruction& instruction)
{
  int size = instruction.get_size();
  int unit = sint::bit_type::default_length;
  for (int i = 0; i < DIV_CEIL(size, unit); i++)
  {
    Procb.S[instruction.get_r(1) + i] = {};
  }
  for (int i = 0; i < size; i++)
  {
    typename sint::bit_type tmp;
    Procp.DataF.get_dabit(Procp.get_S_ref(instruction.get_r(0) + i), tmp);
    Procb.S[instruction.get_r(1) + i / unit] ^= tmp << (i % unit);
  }
}

template<class sint, class sgf2n>
void Processor<sint, sgf2n>::edabit(const Instruction& instruction, bool strict)
{
  auto& regs = instruction.get_start();
  int size = instruction.get_size();
  Procp.DataF.get_edabits(strict, size,
          &Procp.get_S_ref(instruction.get_r(0)), Procb.S, regs);
}

template<class sint, class sgf2n>
void Processor<sint, sgf2n>::convcbitvec(const Instruction& instruction)
{
  for (size_t i = 0; i < instruction.get_n(); i++)
    {
      int i1 = i / GC::Clear::N_BITS;
      int i2 = i % GC::Clear::N_BITS;
      Ci[instruction.get_r(0) + i] = Procb.C[instruction.get_r(1) + i1].get_bit(i2);
    }
}

template<class sint, class sgf2n>
void Processor<sint, sgf2n>::convcintvec(const Instruction& instruction)
{
  int unit = GC::Clear::N_BITS;
  assert(unit == 64);
  int n_inputs = instruction.get_size();
  int n_bits = instruction.get_start().size();
  for (int i = 0; i < DIV_CEIL(n_inputs, unit); i++)
    {
      for (int j = 0; j < DIV_CEIL(n_bits, unit); j++)
        {
          square64 square;
          int n_rows = min(n_inputs - i * unit, unit);
          int n_cols = min(n_bits - j * unit, unit);
          for (int k = 0; k < n_rows; k++)
            square.rows[k] =
                Integer(Procp.C[instruction.get_r(0) + i * unit + k]
                                >> (j * unit)).get();
          square.transpose(n_rows, n_cols);
          for (int k = 0; k < n_cols; k++)
            Procb.C[instruction.get_start()[k + j * unit] + i] = square.rows[k];
        }
    }
}

template<class sint, class sgf2n>
void Processor<sint, sgf2n>::convcbit2s(const Instruction& instruction)
{
  int unit = GC::Clear::N_BITS;
  for (int i = 0; i < DIV_CEIL(instruction.get_n(), unit); i++)
    Procb.S[instruction.get_r(0) + i] = sint::bit_type::constant(
        Procb.C[instruction.get_r(1) + i], P.my_num(),
        share_thread.MC->get_alphai());
}

template<class sint, class sgf2n>
void Processor<sint, sgf2n>::split(const Instruction& instruction)
{
  int n = instruction.get_n();
  assert (instruction.get_start().size() % n == 0);
  int unit = GC::Clear::N_BITS;
  assert(unit == 64);
  int n_inputs = instruction.get_size();
  int n_bits = instruction.get_start().size() / n;
  assert(share_thread.protocol != 0);
  sint::split(Procb.S, instruction.get_start(), n_bits,
      &read_Sp(instruction.get_r(0)), n_inputs, *share_thread.protocol);
}


#include "Networking/sockets.h"
#include "Math/Setup.h"

// Write socket (typically SPDZ engine -> external client), for different register types.
// RegType and SecrecyType determines how registers are read and the socket stream is packed.
// If message_type is > 0, send message_type in bytes 0 - 3, to allow an external client to
//  determine the data structure being sent in a message.
template<class sint, class sgf2n>
void Processor<sint, sgf2n>::write_socket(const RegType reg_type, const SecrecyType secrecy_type, const bool send_macs,
                             int socket_id, int message_type, const vector<int>& registers)
{
  int m = registers.size();
  socket_stream.reset_write_head();

  //First 4 bytes is message_type (unless indicate not needed)
  if (message_type != 0) {
    socket_stream.store(message_type);
  }

  for (int i = 0; i < m; i++)
  {
    if (reg_type == MODP && secrecy_type == SECRET) {
      // Send vector of secret shares and optionally macs
      if (send_macs)
        get_Sp_ref(registers[i]).pack(socket_stream);
      else
        get_Sp_ref(registers[i]).pack(socket_stream,
            sint::get_rec_factor(P.my_num(), P.num_players()));
    }
    else if (reg_type == MODP && secrecy_type == CLEAR) {
      // Send vector of clear public field elements
      get_Cp_ref(registers[i]).pack(socket_stream);
    }
    else if (reg_type == INT && secrecy_type == CLEAR) {
      // Send vector of 32-bit clear ints
      socket_stream.store((int&)get_Ci_ref(registers[i]));
    } 
    else {
      stringstream ss;
      ss << "Write socket instruction with unknown reg type " << reg_type << 
        " and secrecy type " << secrecy_type << "." << endl;      
      throw Processor_Error(ss.str());
    }
  }

  try {
    socket_stream.Send(external_clients.get_socket(socket_id));
  }
    catch (bad_value& e) {
    cerr << "Send error thrown when writing " << m << " values of type " << reg_type << " to socket id " 
      << socket_id << "." << endl;
  }
}


// Receive vector of 32-bit clear ints
template<class sint, class sgf2n>
void Processor<sint, sgf2n>::read_socket_ints(int client_id, const vector<int>& registers)
{
  int m = registers.size();
  socket_stream.reset_write_head();
  socket_stream.Receive(external_clients.get_socket(client_id));
  for (int i = 0; i < m; i++)
  {
    int val;
    socket_stream.get(val);
    write_Ci(registers[i], (long)val);
  }
}

// Receive vector of public field elements
template<class sint, class sgf2n>
void Processor<sint, sgf2n>::read_socket_vector(int client_id, const vector<int>& registers)
{
  int m = registers.size();
  socket_stream.reset_write_head();
  socket_stream.Receive(external_clients.get_socket(client_id));
  for (int i = 0; i < m; i++)
  {
    get_Cp_ref(registers[i]) = socket_stream.get<typename sint::open_type>();
  }
}

// Receive vector of field element shares over private channel
template<class sint, class sgf2n>
void Processor<sint, sgf2n>::read_socket_private(int client_id, const vector<int>& registers, bool read_macs)
{
  int m = registers.size();
  socket_stream.reset_write_head();
  socket_stream.Receive(external_clients.get_socket(client_id));

  for (int i = 0; i < m; i++)
  {
    get_Sp_ref(registers[i]).unpack(socket_stream, read_macs);
  }
}


// Read share data from a file starting at file_pos until registers filled.
// file_pos_register is written with new file position (-1 is eof).
// Tolerent to no file if no shares yet persisted.
template<class sint, class sgf2n>
void Processor<sint, sgf2n>::read_shares_from_file(int start_file_posn, int end_file_pos_register, const vector<int>& data_registers) {
  string filename;
  filename = "Persistence/Transactions-P" + to_string(P.my_num()) + ".data";

  unsigned int size = data_registers.size();

  vector< sint > outbuf(size);

  int end_file_posn = start_file_posn;

  try {
    binary_file_io.read_from_file(filename, outbuf, start_file_posn, end_file_posn);

    for (unsigned int i = 0; i < size; i++)
    {
      get_Sp_ref(data_registers[i]) = outbuf[i];
    }

    write_Ci(end_file_pos_register, (long)end_file_posn);    
  }
  catch (file_missing& e) {
    cerr << "Got file missing error, will return -2. " << e.what() << endl;
    write_Ci(end_file_pos_register, (long)-2);
  }
}

// Append share data in data_registers to end of file. Expects Persistence directory to exist.
template<class sint, class sgf2n>
void Processor<sint, sgf2n>::write_shares_to_file(const vector<int>& data_registers) {
  string dir = "Persistence";
  mkdir_p(dir.c_str());

  string filename;
  filename = dir + "/Transactions-P" + to_string(P.my_num()) + ".data";

  unsigned int size = data_registers.size();

  vector< sint > inpbuf (size);

  for (unsigned int i = 0; i < size; i++)
  {
    inpbuf[i] = get_Sp_ref(data_registers[i]);
  }

  binary_file_io.write_to_file(filename, inpbuf);
}

template <class T>
void SubProcessor<T>::POpen(const vector<int>& reg,const Player& P,int size)
{
  assert(reg.size() % 2 == 0);
  int sz=reg.size() / 2;
  MC.init_open(P, sz * size);
  for (auto it = reg.begin() + 1; it < reg.end(); it += 2)
    for (int i = 0; i < size; i++)
      MC.prepare_open(S[*it + i]);
  MC.exchange(P);
  for (auto it = reg.begin(); it < reg.end(); it += 2)
    for (int i = 0; i < size; i++)
      C[*it + i] = MC.finalize_open();

  if (Proc != 0)
    {
      Proc->sent += sz * size;
      Proc->rounds++;
    }
}

template<class T>
void SubProcessor<T>::muls(const vector<int>& reg, int size)
{
    assert(reg.size() % 3 == 0);
    int n = reg.size() / 3;

    SubProcessor<T>& proc = *this;
    protocol.init_mul(&proc);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < size; j++)
        {
            auto& x = proc.S[reg[3 * i + 1] + j];
            auto& y = proc.S[reg[3 * i + 2] + j];
            protocol.prepare_mul(x, y);
        }
    protocol.exchange();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < size; j++)
        {
            proc.S[reg[3 * i] + j] = protocol.finalize_mul();
        }

    protocol.counter += n * size;
}

template<class T>
void SubProcessor<T>::mulrs(const vector<int>& reg)
{
    assert(reg.size() % 4 == 0);
    int n = reg.size() / 4;

    SubProcessor<T>& proc = *this;
    protocol.init_mul(&proc);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < reg[4 * i]; j++)
        {
            auto& x = proc.S[reg[4 * i + 2] + j];
            auto& y = proc.S[reg[4 * i + 3]];
            protocol.prepare_mul(x, y);
        }
    protocol.exchange();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < reg[4 * i]; j++)
        {
            proc.S[reg[4 * i + 1] + j] = protocol.finalize_mul();
        }
        protocol.counter += reg[4 * i];
    }
}

template<class T>
void SubProcessor<T>::dotprods(const vector<int>& reg, int size)
{
    protocol.init_dotprod(this);
    for (int i = 0; i < size; i++)
    {
        auto it = reg.begin();
        while (it != reg.end())
        {
            auto next = it + *it;
            it += 2;
            while (it != next)
            {
                protocol.prepare_dotprod(S[*it + i], S[*(it + 1) + i]);
                it += 2;
            }
            protocol.next_dotprod();
        }
    }
    protocol.exchange();
    for (int i = 0; i < size; i++)
    {
        auto it = reg.begin();
        while (it != reg.end())
        {
            auto next = it + *it;
            it++;
            S[*it + i] = protocol.finalize_dotprod((next - it) / 2);
            it = next;
        }
    }
}

template<class T>
void SubProcessor<T>::matmuls(const vector<T>& source,
        const Instruction& instruction, int a, int b)
{
    auto& dim = instruction.get_start();
    auto A = source.begin() + a;
    auto B = source.begin() + b;
    auto C = S.begin() + (instruction.get_r(0));
    assert(A + dim[0] * dim[1] <= source.end());
    assert(B + dim[1] * dim[2] <= source.end());
    assert(C + dim[0] * dim[2] <= S.end());

    protocol.init_dotprod(this);
    for (int i = 0; i < dim[0]; i++)
        for (int j = 0; j < dim[2]; j++)
        {
            for (int k = 0; k < dim[1]; k++)
                protocol.prepare_dotprod(*(A + i * dim[1] + k),
                        *(B + k * dim[2] + j));
            protocol.next_dotprod();
        }
    protocol.exchange();
    for (int i = 0; i < dim[0]; i++)
        for (int j = 0; j < dim[2]; j++)
            *(C + i * dim[2] + j) = protocol.finalize_dotprod(dim[1]);
}

template<class T>
void SubProcessor<T>::matmulsm(const CheckVector<T>& source,
        const Instruction& instruction, int a, int b)
{
    auto& dim = instruction.get_start();
    auto C = S.begin() + (instruction.get_r(0));
    assert(C + dim[0] * dim[2] <= S.end());
    assert(Proc);

    protocol.init_dotprod(this);
    for (int i = 0; i < dim[0]; i++)
    {
        auto ii = Proc->get_Ci().at(dim[3] + i);
        for (int j = 0; j < dim[2]; j++)
        {
            auto jj = Proc->get_Ci().at(dim[6] + j);
            for (int k = 0; k < dim[1]; k++)
            {
                auto kk = Proc->get_Ci().at(dim[4] + k);
                auto ll = Proc->get_Ci().at(dim[5] + k);
                protocol.prepare_dotprod(source.at(a + ii * dim[7] + kk),
                        source.at(b + ll * dim[8] + jj));
            }
            protocol.next_dotprod();
        }
    }
    protocol.exchange();
    for (int i = 0; i < dim[0]; i++)
        for (int j = 0; j < dim[2]; j++)
            *(C + i * dim[2] + j) = protocol.finalize_dotprod(dim[1]);
}

template<class T>
void SubProcessor<T>::conv2ds(const Instruction& instruction)
{
    protocol.init_dotprod(this);
    auto& args = instruction.get_start();
    int output_h = args[0], output_w = args[1];
    int inputs_h = args[2], inputs_w = args[3];
    int weights_h = args[4], weights_w = args[5];
    int stride_h = args[6], stride_w = args[7];
    int n_channels_in = args[8];
    int padding_h = args[9];
    int padding_w = args[10];
    int r0 = instruction.get_r(0);
    int r1 = instruction.get_r(1);
    int r2 = instruction.get_r(2);
    int lengths[output_h][output_w];
    memset(lengths, 0, sizeof(lengths));

    for (int out_y = 0; out_y < output_h; out_y++)
        for (int out_x = 0; out_x < output_w; out_x++)
        {
            int in_x_origin = (out_x * stride_w) - padding_w;
            int in_y_origin = (out_y * stride_h) - padding_h;

            for (int filter_y = 0; filter_y < weights_h; filter_y++)
            {
                int in_y = in_y_origin + filter_y;
                if ((0 <= in_y) and (in_y < inputs_h))
                    for (int filter_x = 0; filter_x < weights_w; filter_x++)
                    {
                        int in_x = in_x_origin + filter_x;
                        if ((0 <= in_x) and (in_x < inputs_w))
                        {
                            for (int in_c = 0; in_c < n_channels_in; in_c++)
                                protocol.prepare_dotprod(
                                        S[r1 + (in_y * inputs_w + in_x) *
                                          n_channels_in + in_c],
                                        S[r2 + (filter_y * weights_w + filter_x) *
                                          n_channels_in + in_c]);
                            lengths[out_y][out_x] += n_channels_in;
                        }
                    }
            }

            protocol.next_dotprod();
        }

    protocol.exchange();

    for (int out_y = 0; out_y < output_h; out_y++)
        for (int out_x = 0; out_x < output_w; out_x++)
        {
            S[r0 + out_y * output_w + out_x] = protocol.finalize_dotprod(
                    lengths[out_y][out_x]);
        }
}

template<class sint, class sgf2n>
typename sint::clear Processor<sint, sgf2n>::get_inverse2(unsigned m)
{
  for (unsigned i = inverses2m.size(); i <= m; i++)
    inverses2m.push_back((cint(1) << i).invert());
  return inverses2m[m];
}

template<class sint, class sgf2n>
ostream& operator<<(ostream& s,const Processor<sint, sgf2n>& P)
{
  s << "Processor State" << endl;
  s << "Char 2 Registers" << endl;
  s << "Val\tClearReg\tSharedReg" << endl;
  for (int i=0; i<P.reg_max2; i++)
    { s << i << "\t";
      P.read_C2(i).output(s,true);
      s << "\t";
      P.read_S2(i).output(s,true);
      s << endl;
    }
  s << "Char p Registers" << endl;
  s << "Val\tClearReg\tSharedReg" << endl;
  for (int i=0; i<P.reg_maxp; i++)
    { s << i << "\t";
      P.read_Cp(i).output(s,true);
      s << "\t";
      P.read_Sp(i).output(s,true);
      s << endl;
    }

  return s;
}

#endif
