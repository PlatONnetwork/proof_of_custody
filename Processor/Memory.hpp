#include "Processor/Memory.h"
#include "Processor/Instruction.h"

#include <fstream>

template<class T>
void Memory<T>::minimum_size(RegType secret_type, RegType clear_type,
    const Program &program, const string& threadname)
{
  (void) threadname;
  unsigned sizes[MAX_SECRECY_TYPE];
  sizes[SECRET]= program.direct_mem(secret_type);
  sizes[CLEAR] = program.direct_mem(clear_type);
  if (sizes[SECRET] > size_s())
    {
#ifdef DEBUG_MEMORY
      cerr << threadname << " needs more secret " << T::type_string() << " memory, resizing to "
          << sizes[SECRET] << endl;
#endif
      resize_s(sizes[SECRET]);
    }
  if (sizes[CLEAR] > size_c())
    {
#ifdef DEBUG_MEMORY
      cerr << threadname << " needs more clear " << T::type_string() << " memory, resizing to "
          << sizes[CLEAR] << endl;
#endif
      resize_c(sizes[CLEAR]);
    }
}


template<class T>
ostream& operator<<(ostream& s,const Memory<T>& M)
{
  s << M.MS.size() << endl;
  s << M.MC.size() << endl;

#ifdef OUTPUT_HUMAN_READABLE_MEMORY
  for (unsigned int i=0; i<M.MS.size(); i++)
    { M.MS[i].output(s,true); s << endl; }
  s << endl;

  for (unsigned int i=0; i<M.MC.size(); i++)
    {  M.MC[i].output(s,true); s << endl; }
  s << endl;
#else
  for (unsigned int i=0; i<M.MS.size(); i++)
    { M.MS[i].output(s,false); }

  for (unsigned int i=0; i<M.MC.size(); i++)
    { M.MC[i].output(s,false); }
#endif

  return s;
}


template<class T>
istream& operator>>(istream& s,Memory<T>& M)
{
  int len;

  s >> len;  
  M.resize_s(len);
  s >> len;
  M.resize_c(len);
  s.seekg(1, istream::cur);

  for (unsigned int i=0; i<M.MS.size(); i++)
    { M.MS[i].input(s,false);  }

  for (unsigned int i=0; i<M.MC.size(); i++)
    { M.MC[i].input(s,false); }

  return s;
}
