#pragma once

#include "defines.h"

// disable assertions by commenting out below line
#define RASSERTIONS_ENABLED

#ifdef RASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

RAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

#define RASSERT(expr)                                               \
{                                                                   \
    if(expr){                                                       \
    } else {                                                        \
        report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
        debugBreak();                                               \
    }                                                               \
}                                                                   

#define RASSERT_MSG(expr, message)                                      \
{                                                                       \
    if(expr){                                                           \
    } else {                                                            \
        report_assertion_failure(#expr, message, __FILE__, __LINE__);   \
        debugBreak();                                                   \
    }                                                                   \
}                                                                       

#ifdef _DEBUG
#define RASSERT_DEBUG(expr)                                             \
{                                                                       \
    if(expr){                                                           \
    } else {                                                            \
        report_assertion_failure(#expr, "", __FILE__, __LINE__);        \
        debugBreak();                                                   \
    }                                                                   \
}                                                                       
#else
#define RASSERT_DEBUG(expr)
#endif

#else
#define RASSERT(expr)
#define RASSERT_MSG(expr, message)
#define RASSERT_DEBUG(expr)
#endif
