#pragma once

// 位置放的不对导致编译错误
//#define UBPA_USE_SIMD
//#include <UGM/UGM.h>
//using namespace Ubpa;


#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <omp.h>

#ifndef _DEBUG
#define NDEBUG
#endif

#include <cassert>



#include "Math.h"

// frequently used type
using std::string;
using std::vector;
using std::array;
using std::function;
using std::ostringstream;
using std::shared_ptr;
using std::make_shared;
using std::swap;
