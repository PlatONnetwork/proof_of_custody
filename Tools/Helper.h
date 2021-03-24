#pragma once

#include <typeinfo>
using namespace std;
#ifndef TYPENAME
#if defined(__GNUC__)
#include <cxxabi.h> // abi::__cxa_demangle
#define TYPENAME(typeid_name) abi::__cxa_demangle(typeid_name, nullptr, nullptr, nullptr)
#else
#define TYPENAME(typeid_name) typeid_name
#endif
#endif
