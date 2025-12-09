
#ifndef SUNWELLCORE_COMMON_H
#define SUNWELLCORE_COMMON_H

#include "Define.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <memory>
#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#if PLATFORM == PLATFORM_WINDOWS
// XP winver - needed to compile with standard leak check in MemoryLeaks.h
// uncomment later if needed
//#define _WIN32_WINNT 0x0501
#include <ws2tcpip.h>
//#undef WIN32_WINNT
#else
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#endif

inline float finiteAlways(float f) { return std::isfinite(f) ? f : 0.0f; }

constexpr uint32 MINUTE = 60;
constexpr uint32 HOUR = MINUTE * 60;
constexpr uint32 DAY = HOUR * 24;
constexpr uint32 WEEK = DAY * 7;
constexpr uint32 MONTH = DAY * 30;
constexpr uint32 YEAR = MONTH * 12;
constexpr uint32 IN_MILLISECONDS = 1000;

enum AccountTypes
{
    SEC_PLAYER        = 0,
    SEC_MODERATOR     = 1,
    SEC_GAMEMASTER    = 2,
    SEC_ADMINISTRATOR = 3,
    SEC_CONSOLE       = 4 // must be always last in list, accounts must have less security level always also
};

enum LocaleConstant
{
    LOCALE_enUS = 0,
    LOCALE_koKR = 1,
    LOCALE_frFR = 2,
    LOCALE_deDE = 3,
    LOCALE_zhCN = 4,
    LOCALE_zhTW = 5,
    LOCALE_esES = 6,
    LOCALE_esMX = 7,
    LOCALE_ruRU = 8,

    TOTAL_LOCALES
};

#define DEFAULT_LOCALE LOCALE_enUS

#define MAX_LOCALES 8
#define MAX_ACCOUNT_TUTORIAL_VALUES 8

extern char const* localeNames[TOTAL_LOCALES];

LocaleConstant GetLocaleByName(const std::string& name);
void CleanStringForMysqlQuery(std::string& str);

typedef std::vector<std::string> StringVector;

// we always use stdlibc++ std::max/std::min, undefine some not C++ standard defines (Win API and some other platforms)
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define MAX_QUERY_LEN 32 * 1024

#endif
