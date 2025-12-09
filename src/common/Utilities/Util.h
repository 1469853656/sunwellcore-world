

#ifndef _UTIL_H
#define _UTIL_H

#include "Define.h"
#include "Errors.h"

#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <future>
#include <chrono>
#include <optional>
#include <boost/lexical_cast.hpp>

// Searcher for map of structs
template<typename T, class S>
struct Finder
{
    T val_;
    T S::*idMember_;

    Finder(T val, T S::*idMember)
        : val_(val)
        , idMember_(idMember)
    {
    }
    bool operator()(const std::pair<int, S>& obj) { return obj.second.*idMember_ == val_; }
};

class Tokenizer
{
public:
    typedef std::vector<char const*> StorageType;

    typedef StorageType::size_type size_type;

    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::reference reference;
    typedef StorageType::const_reference const_reference;

public:
    Tokenizer(const std::string& src, char const sep, uint32 vectorReserve = 0);
    ~Tokenizer() { delete[] m_str; }

    const_iterator begin() const { return m_storage.begin(); }
    const_iterator end() const { return m_storage.end(); }

    size_type size() const { return m_storage.size(); }

    reference operator[](size_type i) { return m_storage[i]; }
    const_reference operator[](size_type i) const { return m_storage[i]; }

private:
    char* m_str;
    StorageType m_storage;
};

struct tm* localtime_r(time_t const* time, struct tm* result);

void stripLineInvisibleChars(std::string& src);

int32 MoneyStringToMoney(const std::string& moneyString);

std::string secsToTimeString(uint64 timeInSecs, bool shortText = false);
uint32 TimeStringToSecs(const std::string& timestring);
std::string TimeToTimestampStr(time_t t);
std::string TimeToTimestampStr_Safe(time_t time);
std::string TimeToHumanReadable(time_t t);

std::mt19937& GetGenerator();

/* Return a random number in the range min..max. */
int32 irand(int32 min, int32 max);

/* Return a random number in the range min..max (inclusive). */
uint32 urand(uint32 min, uint32 max);
std::chrono::seconds randseconds(uint32 min, uint32 max);

/* Return a random number in the range 0 .. UINT32_MAX. */
uint32 rand32();

/* Return a random number in the range min..max */
float frand(float min, float max);

/* Return a random double from 0.0 to 1.0 (exclusive). */
double rand_norm();

/* Return a random double from 0.0 to 100.0 (exclusive). */
double rand_chance();

/* Return true if a random roll fits in the specified chance (range 0-100). */
inline bool roll_chance_f(float chance)
{
    return chance > rand_chance();
}

/* Return true if a random roll fits in the specified chance (range 0-100). */
inline bool roll_chance_i(int32 chance)
{
    return chance > irand(0, 99);
}

inline void ApplyPercentModFloatVar(float& var, float val, bool apply)
{
    if (val == -100.0f) // prevent set var to zero
        val = -99.99f;
    var *= (apply ? (100.0f + val) / 100.0f : 100.0f / (100.0f + val));
}

// Percentage calculation
template<class T, class U>
inline T CalculatePct(T base, U pct)
{
    return T(base * static_cast<float>(pct) / 100.0f);
}

template<class T, class U>
inline T AddPct(T& base, U pct)
{
    return base += CalculatePct(base, pct);
}

template<class T, class U>
inline T ApplyPct(T& base, U pct)
{
    return base = CalculatePct(base, pct);
}

template<class T>
inline T RoundToInterval(T& num, T floor, T ceil)
{
    return num = std::min(std::max(num, floor), ceil);
}

bool IsChatAllowedCharacter(wchar_t wchar);
bool IsChatAllowedString(std::string_view utf8String);

// UTF8 handling
bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr);

bool WStrToUtf8(const std::wstring& wstr, std::string& utf8str);

size_t Utf8Length(std::string const& utf8str); // set string to "" if invalid utf8 sequence
void Utf8Truncate(std::string& utf8str, size_t len);

inline bool isBasicLatinCharacter(wchar_t wchar)
{
    if (wchar >= L'a' && wchar <= L'z') // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
        return true;
    if (wchar >= L'A' && wchar <= L'Z') // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
        return true;
    return false;
}

inline bool isExtendedLatinCharacter(wchar_t wchar)
{
    if (isBasicLatinCharacter(wchar))
        return true;
    if (wchar >= 0x00C0 && wchar <= 0x00D6) // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
        return true;
    if (wchar >= 0x00D8 && wchar <= 0x00DE) // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
        return true;
    if (wchar == 0x00DF) // LATIN SMALL LETTER SHARP S
        return true;
    if (wchar >= 0x00E0 && wchar <= 0x00F6) // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
        return true;
    if (wchar >= 0x00F8 && wchar <= 0x00FE) // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
        return true;
    if (wchar >= 0x0100 && wchar <= 0x012F) // LATIN CAPITAL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
        return true;
    if (wchar == 0x1E9E) // LATIN CAPITAL LETTER SHARP S
        return true;
    return false;
}

inline bool isCyrillicCharacter(wchar_t wchar)
{
    if (wchar >= 0x0410 && wchar <= 0x044F) // CYRILLIC CAPITAL LETTER A - CYRILLIC SMALL LETTER YA
        return true;
    if (wchar == 0x0401 || wchar == 0x0451) // CYRILLIC CAPITAL LETTER IO, CYRILLIC SMALL LETTER IO
        return true;
    return false;
}

inline bool isEastAsianCharacter(wchar_t wchar)
{
    if (wchar >= 0x1100 && wchar <= 0x11F9) // Hangul Jamo
        return true;
    if (wchar >= 0x3041 && wchar <= 0x30FF) // Hiragana + Katakana
        return true;
    if (wchar >= 0x3131 && wchar <= 0x318E) // Hangul Compatibility Jamo
        return true;
    if (wchar >= 0x31F0 && wchar <= 0x31FF) // Katakana Phonetic Ext.
        return true;
    if (wchar >= 0x3400 && wchar <= 0x4DB5) // CJK Ideographs Ext. A
        return true;
    if (wchar >= 0x4E00 && wchar <= 0x9FC3) // Unified CJK Ideographs
        return true;
    if (wchar >= 0xAC00 && wchar <= 0xD7A3) // Hangul Syllables
        return true;
    if (wchar >= 0xFF01 && wchar <= 0xFFEE) // Halfwidth forms
        return true;
    return false;
}

inline bool isNumeric(wchar_t wchar)
{
    return (wchar >= L'0' && wchar <= L'9');
}

inline bool isNumeric(char c)
{
    return (c >= '0' && c <= '9');
}

inline bool isNumeric(char const* str)
{
    for (char const* c = str; *c; ++c)
        if (!isNumeric(*c))
            return false;

    return true;
}

inline bool isNumericOrSpace(wchar_t wchar)
{
    return isNumeric(wchar) || wchar == L' ';
}

inline bool isBasicLatinString(const std::wstring& wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isBasicLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline bool isExtendedLatinString(const std::wstring& wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isExtendedLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline bool isCyrillicString(const std::wstring& wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isCyrillicCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline bool isEastAsianString(const std::wstring& wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isEastAsianCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline wchar_t wcharToUpper(wchar_t wchar)
{
    if (wchar >= L'a' && wchar <= L'z') // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
        return wchar_t(uint16(wchar) - 0x0020);
    if (wchar == 0x00DF) // LATIN SMALL LETTER SHARP S
        return wchar_t(0x1E9E);
    if (wchar >= 0x00E0 && wchar <= 0x00F6) // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
        return wchar_t(uint16(wchar) - 0x0020);
    if (wchar >= 0x00F8 && wchar <= 0x00FE) // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
        return wchar_t(uint16(wchar) - 0x0020);
    if (wchar >= 0x0101 && wchar <= 0x012F) // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
    {
        if (wchar % 2 == 1)
            return wchar_t(uint16(wchar) - 0x0001);
    }
    if (wchar >= 0x0430 && wchar <= 0x044F) // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
        return wchar_t(uint16(wchar) - 0x0020);
    if (wchar == 0x0451) // CYRILLIC SMALL LETTER IO
        return wchar_t(0x0401);

    return wchar;
}

inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
{
    return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
}

inline wchar_t wcharToLower(wchar_t wchar)
{
    if (wchar >= L'A' && wchar <= L'Z') // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
        return wchar_t(uint16(wchar) + 0x0020);
    if (wchar >= 0x00C0 && wchar <= 0x00D6) // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
        return wchar_t(uint16(wchar) + 0x0020);
    if (wchar >= 0x00D8 && wchar <= 0x00DE) // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
        return wchar_t(uint16(wchar) + 0x0020);
    if (wchar >= 0x0100 && wchar <= 0x012E) // LATIN CAPITAL LETTER A WITH MACRON - LATIN CAPITAL LETTER I WITH OGONEK (only %2=0)
    {
        if (wchar % 2 == 0)
            return wchar_t(uint16(wchar) + 0x0001);
    }
    if (wchar == 0x1E9E) // LATIN CAPITAL LETTER SHARP S
        return wchar_t(0x00DF);
    if (wchar == 0x0401) // CYRILLIC CAPITAL LETTER IO
        return wchar_t(0x0451);
    if (wchar >= 0x0410 && wchar <= 0x042F) // CYRILLIC CAPITAL LETTER A - CYRILLIC CAPITAL LETTER YA
        return wchar_t(uint16(wchar) + 0x0020);

    return wchar;
}

inline void wstrToUpper(std::wstring& str)
{
    std::transform(str.begin(), str.end(), str.begin(), wcharToUpper);
}

inline void wstrToLower(std::wstring& str)
{
    std::transform(str.begin(), str.end(), str.begin(), wcharToLower);
}

std::wstring GetMainPartOfName(std::wstring const& wname, uint32 declension);

bool utf8ToConsole(const std::string& utf8str, std::string& conStr);
bool consoleToUtf8(const std::string& conStr, std::string& utf8str);
bool Utf8FitTo(const std::string& str, std::wstring const& search);
void utf8printf(FILE* out, const char* str, ...);
void vutf8printf(FILE* out, const char* str, va_list* ap);

uint32 CreatePIDFile(const std::string& filename);

std::string ByteArrayToHexStr(uint8 const* bytes, uint32 length, bool reverse = false);
std::string GenerateRandomIdentifier(size_t /*length*/, std::string const& /*charset*/);
std::string GenerateRandomIdentifier(size_t /*length*/, std::function<char(void)>& /*randomCharacter*/);
#endif

//handler for operations on large flags
#ifndef _FLAG96
#define _FLAG96

// simple class for not-modifyable list
template<typename T>
class HookList
{
    typedef typename std::list<T>::iterator ListIterator;

private:
    typename std::list<T> m_list;

public:
    HookList<T>& operator+=(T t)
    {
        m_list.push_back(t);
        return *this;
    }
    HookList<T>& operator-=(T t)
    {
        m_list.remove(t);
        return *this;
    }
    size_t size()
    {
        return m_list.size();
    }
    ListIterator begin()
    {
        return m_list.begin();
    }
    ListIterator end()
    {
        return m_list.end();
    }
};

class flag96
{
private:
    uint32 part[3];

public:
    flag96(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
    }

    flag96(uint64 p1, uint32 p2)
    {
        part[0] = (uint32)(p1 & UI64LIT(0x00000000FFFFFFFF));
        part[1] = (uint32)((p1 >> 32) & UI64LIT(0x00000000FFFFFFFF));
        part[2] = p2;
    }

    inline bool IsEqual(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0) const
    {
        return (part[0] == p1 && part[1] == p2 && part[2] == p3);
    }

    inline bool HasFlag(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0) const
    {
        return (part[0] & p1 || part[1] & p2 || part[2] & p3);
    }

    inline void Set(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
    }

    inline bool operator<(const flag96& right) const
    {
        for (uint8 i = 3; i > 0; --i)
        {
            if (part[i - 1] < right.part[i - 1])
                return true;
            else if (part[i - 1] > right.part[i - 1])
                return false;
        }
        return false;
    }

    inline bool operator==(const flag96& right) const
    {
        return (
            part[0] == right.part[0] && part[1] == right.part[1] && part[2] == right.part[2]);
    }

    inline bool operator!=(const flag96& right) const
    {
        return !this->operator==(right);
    }

    inline flag96& operator=(const flag96& right)
    {
        part[0] = right.part[0];
        part[1] = right.part[1];
        part[2] = right.part[2];
        return *this;
    }

    inline flag96 operator&(const flag96& right) const
    {
        return flag96(part[0] & right.part[0], part[1] & right.part[1],
            part[2] & right.part[2]);
    }

    inline flag96& operator&=(const flag96& right)
    {
        part[0] &= right.part[0];
        part[1] &= right.part[1];
        part[2] &= right.part[2];
        return *this;
    }

    inline flag96 operator|(const flag96& right) const
    {
        return flag96(part[0] | right.part[0], part[1] | right.part[1],
            part[2] | right.part[2]);
    }

    inline flag96& operator|=(const flag96& right)
    {
        part[0] |= right.part[0];
        part[1] |= right.part[1];
        part[2] |= right.part[2];
        return *this;
    }

    inline flag96 operator~() const
    {
        return flag96(~part[0], ~part[1], ~part[2]);
    }

    inline flag96 operator^(const flag96& right) const
    {
        return flag96(part[0] ^ right.part[0], part[1] ^ right.part[1],
            part[2] ^ right.part[2]);
    }

    inline flag96& operator^=(const flag96& right)
    {
        part[0] ^= right.part[0];
        part[1] ^= right.part[1];
        part[2] ^= right.part[2];
        return *this;
    }

    inline operator bool() const
    {
        return (part[0] != 0 || part[1] != 0 || part[2] != 0);
    }

    inline bool operator!() const
    {
        return !this->operator bool();
    }

    inline uint32& operator[](uint8 el)
    {
        return part[el];
    }

    inline const uint32& operator[](uint8 el) const
    {
        return part[el];
    }
};

enum ComparisionType
{
    COMP_TYPE_EQ = 0,
    COMP_TYPE_HIGH,
    COMP_TYPE_LOW,
    COMP_TYPE_HIGH_EQ,
    COMP_TYPE_LOW_EQ,
    COMP_TYPE_MAX
};

template<class T>
bool CompareValues(ComparisionType type, T val1, T val2)
{
    switch (type)
    {
        case COMP_TYPE_EQ:
            return val1 == val2;
        case COMP_TYPE_HIGH:
            return val1 > val2;
        case COMP_TYPE_LOW:
            return val1 < val2;
        case COMP_TYPE_HIGH_EQ:
            return val1 >= val2;
        case COMP_TYPE_LOW_EQ:
            return val1 <= val2;
        default:
            // incorrect parameter
            ASSERT(false);
            return false;
    }
}

template<typename T>
inline bool isFutureReady(const std::future<T>& f)
{
    return f.valid() && f.wait_for(std::chrono::seconds(0)) != std::future_status::timeout;
}

tm tm_from_time_t(time_t t);

namespace Trinity
{
    void CopyTextToClipboard(std::string_view str);
}

namespace Trinity::Impl
{
    std::string ByteArrayToHexStr(uint8 const* bytes, size_t length, bool reverse = false);
    void HexStrToByteArray(std::string_view str, uint8* out, size_t outlen, bool reverse = false);
}

template <typename Container>
std::string ByteArrayToHexStr(Container const& c, bool reverse = false)
{
    return Trinity::Impl::ByteArrayToHexStr(std::data(c), std::size(c), reverse);
}

template <size_t Size>
void HexStrToByteArray(std::string_view str, std::array<uint8, Size>& buf, bool reverse = false)
{
    Trinity::Impl::HexStrToByteArray(str, buf.data(), Size, reverse);
}

template <size_t Size>
std::array<uint8, Size> HexStrToByteArray(std::string_view str, bool reverse = false)
{
    std::array<uint8, Size> arr;
    HexStrToByteArray(str, arr, reverse);
    return arr;
}

inline std::vector<uint8> HexStrToByteVector(std::string_view str, bool reverse = false)
{
    std::vector<uint8> buf;
    size_t const sz = (str.size() / 2);
    buf.resize(sz);
    Trinity::Impl::HexStrToByteArray(str, buf.data(), sz, reverse);
    return buf;
}

template<class T>
void HashCombine(size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<class T>
std::optional<T> char_cast(const char* text)
{
    try
    {
        return boost::lexical_cast<T>(text);
    }
    catch (boost::bad_lexical_cast&)
    {
        return std::nullopt;
    }
}

// implementation of std::to_underlying from c++23
// casts enum value to its underlying type
template<typename E>
constexpr auto to_underlying(E e) -> std::underlying_type_t<E>
{
    return static_cast<std::underlying_type_t<E>>(e);
}

#endif
