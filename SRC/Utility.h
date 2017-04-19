
/*
 * Utility helper.
 * Written by ..., 2014/9/19
 */
#include <iostream>
#include <string>
#include <time.h>

#include "Config.h"

#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifndef RELEASE

#define CHECK_POINTER(p) if(NULL == p) { std::cerr<<"NULL Pointer"<<__FILE__<<" "<<__LINE__<<std::endl; return; }
#define CHECK_POINTER_EX(p, ret) if(NULL == p) { std::cerr<<"NULL Pointer"<<__FILE__<<" "<<__LINE__<<std::endl; return ret; }
#define DBGPRINTF printf

#else

#define CHECK_POINTER(p) if(NULL == p) { return; }
#define CHECK_POINTER_EX(p, ret) if(NULL == p) { return ret; }
#define DBGPRINTF(t, ...)

#endif


#ifdef WINDOWS

#define SNPRINTF sprintf_s
#define VSNPRINTF(t1, t2, t3, ...) vsnprintf_s(t1, t2, t2, t3, __VA_ARGS__)
#define LOCALTIME localtime_s
#define SSCANF sscanf_s

#else

#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define LOCALTIME(p,q) p=localtime(q)
#define SSCANF sscanf

#endif

/*
 * C-Style DateTime helper function.
 */
const std::string ConvertToString(const time_t&);

bool ConvertToTimet(const char*, time_t&);

void GetTime(time_t&);

#endif
