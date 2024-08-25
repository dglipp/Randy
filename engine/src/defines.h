#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef float f32;
typedef double f64;

typedef int b32;
typedef char b8;

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 byte");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 byte");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 byte");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 byte");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 byte");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 byte");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 byte");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 byte");

#define TRUE 1
#define FALSE 0

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) 
#define RPLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define RPLATFORM_LINUX 1
#if defined(__ANDROID__)
#define RPLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define RPLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define RPLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define RPLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define RPLATFORM_IOS 1
#define RPLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define RPLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef REXPORT
// Exports
#ifdef _MSC_VER
#define RAPI __declspec(dllexport)
#else
#define RAPI __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define RAPI __declspec(dllimport)
#else
#define RAPI
#endif
#endif