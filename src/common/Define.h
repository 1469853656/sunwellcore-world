
#ifndef TRINITY_DEFINE_H
#define TRINITY_DEFINE_H

#include "CompilerDefs.h"

#include <cstddef>
#include <cinttypes>

#if PLATFORM == PLATFORM_WINDOWS
#define TRINITY_PATH_MAX MAX_PATH
#ifndef DECLSPEC_NORETURN
#define DECLSPEC_NORETURN __declspec(noreturn)
#endif //DECLSPEC_NORETURN
#ifndef DECLSPEC_DEPRECATED
#define DECLSPEC_DEPRECATED __declspec(deprecated)
#endif //DECLSPEC_DEPRECATED
#else  //PLATFORM != PLATFORM_WINDOWS
#include <limits.h>
#define TRINITY_PATH_MAX PATH_MAX
#define DECLSPEC_NORETURN
#define DECLSPEC_DEPRECATED
#endif //PLATFORM

#if !defined(COREDEBUG)
#define TRINITY_INLINE inline
#else //COREDEBUG
#if !defined(TRINITY_DEBUG)
#define TRINITY_DEBUG
#endif //TRINITY_DEBUG
#define TRINITY_INLINE
#endif //!COREDEBUG

// g++ can do some optimizations and checks for flagged functions
#if defined(__GNUC__)
#define ATTR_NORETURN __attribute__((noreturn))
#define ATTR_PRINTF(F, V) __attribute__((format(printf, F, V)))
#define ATTR_DEPRECATED __attribute__((deprecated))
#else
#define ATTR_NORETURN
#define ATTR_PRINTF(F, V)
#define ATTR_DEPRECATED
#endif

#define UI64FMTD "%" PRIuLEAST64
#define XI64FMTD "%016" PRIXLEAST64
#define UI64LIT(N) N ## ll

#define SIZEFMTD "%zu"

typedef int_least64_t int64;
typedef int_least32_t int32;
typedef int_least16_t int16;
typedef int_least8_t int8;
typedef uint_least64_t uint64;
typedef uint_least32_t uint32;
typedef uint_least16_t uint16;
typedef uint_least8_t uint8;

#endif //TRINITY_DEFINE_H
