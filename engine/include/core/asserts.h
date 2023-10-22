#pragma once

#include <string>

// disable assertions by commenting line
#define R_ASSERTIONS_ENABLED

#ifdef R_ASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

void report_assertion_failure(std::string expression, std::string message, std::string file, int line);

#define R_ASSERT(expr)                                               \
    {                                                                \
        if (expr)                                                    \
        {                                                            \
        }                                                            \
        else                                                         \
        {                                                            \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }

#define R_ASSERT_MSG(expr, message)                                       \
    {                                                                     \
        if (expr)                                                         \
        {                                                                 \
        }                                                                 \
        else                                                              \
        {                                                                 \
            report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            debugBreak();                                                 \
        }                                                                 \
    }

#ifdef _DEBUG
#define R_ASSERT_DEBUG(expr)                                         \
    {                                                                \
        if (expr)                                                    \
        {                                                            \
        }                                                            \
        else                                                         \
        {                                                            \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }
#else
#define R_ASSERT_DEBUG(expr)
#endif

#else
#define R_ASSERT(expr)
#define R_ASSERT_MSG(expr)
#define R_ASSERT_DEBUG(expr)
#endif