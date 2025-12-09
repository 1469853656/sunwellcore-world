#pragma once

#ifdef WITH_PROFILER
#include "easy/profiler.h"
#include "Hash/FNV1A.h"
#endif

#ifdef WITH_PROFILER
#define PROFILE_SCOPE(name) EASY_BLOCK(name, Fnv(name))
#define PROFILE_FUNCTION(...) EASY_FUNCTION(__VA_ARGS__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION(...)
#endif

