

#include "Util.h"
#include "Common.h"
#include "utf8.h"
#include "Errors.h" // for ASSERT
#include "StringFormat.h"

#include <array>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

std::mt19937& GetGenerator()
{
    static thread_local std::mt19937 s_generator;
    return s_generator;
}

int32 irand(int32 min, int32 max)
{
    ASSERT(max >= min);
    return std::uniform_int_distribution<int32>(min, max)(GetGenerator());
}

uint32 urand(uint32 min, uint32 max)
{
    ASSERT(max >= min);
    return std::uniform_int_distribution<uint32>(min, max)(GetGenerator());
}

std::chrono::seconds randseconds(uint32 min, uint32 max)
{
    ASSERT(max >= min);
    return std::chrono::seconds(urand(min, max));
}

float frand(float min, float max)
{
    ASSERT(max >= min);
    return std::uniform_real_distribution<float>(min, max)(GetGenerator());
}

uint32 rand32()
{
    return urand(std::numeric_limits<uint32>::lowest(), std::numeric_limits<uint32>::max());
}

double rand_norm()
{
    return std::generate_canonical<double, 5>(GetGenerator());
}

double rand_chance()
{
    return std::generate_canonical<double, 2>(GetGenerator()) * 100.0f;
}

Tokenizer::Tokenizer(const std::string& src, const char sep, uint32 vectorReserve)
{
    m_str = new char[src.length() + 1];
    memcpy(m_str, src.c_str(), src.length() + 1);

    if (vectorReserve)
        m_storage.reserve(vectorReserve);

    char* posold = m_str;
    char* posnew = m_str;

    for (;;)
    {
        if (*posnew == sep)
        {
            m_storage.push_back(posold);
            posold = posnew + 1;

            *posnew = '\0';
        }
        else if (*posnew == '\0')
        {
            // Hack like, but the old code accepted these kind of broken strings,
            // so changing it would break other things
            if (posold != posnew)
                m_storage.push_back(posold);

            break;
        }

        ++posnew;
    }
}

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
struct tm* localtime_r(time_t const* time, struct tm* result)
{
    localtime_s(result, time);
    return result;
}
#endif

void stripLineInvisibleChars(std::string& str)
{
    static std::string const invChars = " \t\7\n";

    size_t wpos = 0;

    bool space = false;
    for (size_t pos = 0; pos < str.size(); ++pos)
    {
        if (invChars.find(str[pos]) != std::string::npos)
        {
            if (!space)
            {
                str[wpos++] = ' ';
                space       = true;
            }
        }
        else
        {
            if (wpos != pos)
                str[wpos++] = str[pos];
            else
                ++wpos;
            space = false;
        }
    }

    if (wpos < str.size())
        str.erase(wpos, str.size());
    if (str.find("|TInterface") != std::string::npos)
        str.clear();
}

std::string secsToTimeString(uint64 timeInSecs, bool shortText)
{
    uint64 secs    = timeInSecs % MINUTE;
    uint64 minutes = timeInSecs % HOUR / MINUTE;
    uint64 hours   = timeInSecs % DAY / HOUR;
    uint64 days    = timeInSecs / DAY;

    std::ostringstream ss;
    if (days)
        ss << days << (shortText ? "d" : " day(s) ");
    if (hours)
        ss << hours << (shortText ? "h" : " hour(s) ");
    if (minutes)
        ss << minutes << (shortText ? "m" : " minute(s) ");
    if (secs || (!days && !hours && !minutes))
        ss << secs << (shortText ? "s" : " second(s) ");

    std::string str = ss.str();

    if (!shortText && !str.empty() && str[str.size() - 1] == ' ')
        str.resize(str.size() - 1);

    return str;
}

int32 MoneyStringToMoney(const std::string& moneyString)
{
    int32 money = 0;

    if (!(std::count(moneyString.begin(), moneyString.end(), 'g') == 1 || std::count(moneyString.begin(), moneyString.end(), 's') == 1 || std::count(moneyString.begin(), moneyString.end(), 'c') == 1))
        return 0; // Bad format

    Tokenizer tokens(moneyString, ' ');
    for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
    {
        std::string tokenString(*itr);
        size_t gCount = std::count(tokenString.begin(), tokenString.end(), 'g');
        size_t sCount = std::count(tokenString.begin(), tokenString.end(), 's');
        size_t cCount = std::count(tokenString.begin(), tokenString.end(), 'c');
        if (gCount + sCount + cCount != 1)
            return 0;

        uint32 amount = atoi(*itr);
        if (gCount == 1)
            money += amount * 100 * 100;
        else if (sCount == 1)
            money += amount * 100;
        else if (cCount == 1)
            money += amount;
    }

    return money;
}

uint32 TimeStringToSecs(const std::string& timestring)
{
    uint32 secs       = 0;
    uint32 buffer     = 0;
    uint32 multiplier = 0;

    for (std::string::const_iterator itr = timestring.begin(); itr != timestring.end(); ++itr)
    {
        if (isdigit(*itr))
        {
            buffer *= 10;
            buffer += (*itr) - '0';
        }
        else
        {
            switch (*itr)
            {
                case 'd': multiplier = DAY; break;
                case 'h': multiplier = HOUR; break;
                case 'm': multiplier = MINUTE; break;
                case 's': multiplier = 1; break;
                default: return 0; //bad format
            }
            buffer *= multiplier;
            secs += buffer;
            buffer = 0;
        }
    }

    return secs;
}

std::string TimeToTimestampStr(time_t t)
{
    tm aTm = boost::posix_time::to_tm(boost::posix_time::from_time_t(t));
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[20];
    int ret = snprintf(buf, 20, "%04d-%02d-%02d_%02d-%02d-%02d", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
    if (ret != 19) return "timestamp error";
    return std::string(buf);
}

std::string TimeToTimestampStr_Safe(time_t time)
{
    tm aTm = boost::posix_time::to_tm(boost::posix_time::from_time_t(time));
    return fmt::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
}

std::string TimeToHumanReadable(time_t t)
{
    std::string timeStr = to_iso_extended_string(boost::posix_time::from_time_t(t));
    std::replace(timeStr.begin(), timeStr.end(), 'T', ' ');
    return timeStr;
}

/// create PID file
uint32 CreatePIDFile(const std::string& filename)
{
    FILE* pid_file = fopen(filename.c_str(), "w");
    if (pid_file == nullptr)
        return 0;

#ifdef _WIN32
    DWORD pid = GetCurrentProcessId();
#else
    pid_t pid = getpid();
#endif

    fprintf(pid_file, "%u", pid);
    fclose(pid_file);

    return (uint32)pid;
}

size_t Utf8Length(std::string const& utf8str)
{
    try
    {
        return utf8::distance(utf8str.begin(), utf8str.end());
    }
    catch (std::exception)
    {
        return 0;
    }
}

void Utf8Truncate(std::string& utf8str, size_t len)
{
    try
    {
        size_t wlen = Utf8Length(utf8str);
        if (wlen <= len)
            return;

        std::wstring wstr;
        wstr.reserve(len);

        Utf8toWStr(utf8str, wstr);
        wstr.resize(std::min(wstr.length(), len));

        WStrToUtf8(wstr, utf8str);
    }
    catch (std::exception)
    {
        utf8str.clear();
    }
}

bool IsChatAllowedCharacter(wchar_t wchar)
{
    if (wchar >= 0x0021 && wchar <= 0x002F)
        return true;

    if (wchar >= 0x003A && wchar <= 0x0040)
        return true;

    if (wchar >= 0x005B && wchar <= 0x0060)
        return true;

    if (wchar >= 0x007B && wchar <= 0x007E)
        return true;

    return isBasicLatinCharacter(wchar);
}

bool IsChatAllowedString(std::string_view utf8String)
{
    std::wstring wstr;
    if (!Utf8toWStr(utf8String.data(), wstr))
        return false;

    return std::ranges::all_of(wstr, [](wchar_t c) { return IsChatAllowedCharacter(c) || isNumericOrSpace(c); });
}

bool Utf8toWStr(std::string const& utf8str, std::wstring& wstr)
{
    try
    {
        wstr.clear();
        wstr.reserve(utf8str.length());

        utf8::utf8to16(utf8str.begin(), utf8str.end(), std::back_inserter(wstr));
    }
    catch (std::exception)
    {
        wstr.clear();
        return false;
    }

    return true;
}

bool WStrToUtf8(std::wstring const& wstr, std::string& utf8str)
{
    try
    {
        utf8str.clear();
        utf8str.reserve(wstr.length());

        utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(utf8str));
    }
    catch (std::exception)
    {
        utf8str.clear();
        return false;
    }

    return true;
}

typedef wchar_t const* const* wstrlist;

std::wstring GetMainPartOfName(std::wstring const& wname, uint32 declension)
{
    // supported only Cyrillic cases
    if (wname.empty() || !isCyrillicCharacter(wname[0]) || declension > 5)
        return wname;

    // Important: end length must be <= MAX_INTERNAL_PLAYER_NAME-MAX_PLAYER_NAME (3 currently)
    static std::wstring const a_End    = { wchar_t(0x0430), wchar_t(0x0000) };
    static std::wstring const o_End    = { wchar_t(0x043E), wchar_t(0x0000) };
    static std::wstring const ya_End   = { wchar_t(0x044F), wchar_t(0x0000) };
    static std::wstring const ie_End   = { wchar_t(0x0435), wchar_t(0x0000) };
    static std::wstring const i_End    = { wchar_t(0x0438), wchar_t(0x0000) };
    static std::wstring const yeru_End = { wchar_t(0x044B), wchar_t(0x0000) };
    static std::wstring const u_End    = { wchar_t(0x0443), wchar_t(0x0000) };
    static std::wstring const yu_End   = { wchar_t(0x044E), wchar_t(0x0000) };
    static std::wstring const oj_End   = { wchar_t(0x043E), wchar_t(0x0439), wchar_t(0x0000) };
    static std::wstring const ie_j_End = { wchar_t(0x0435), wchar_t(0x0439), wchar_t(0x0000) };
    static std::wstring const io_j_End = { wchar_t(0x0451), wchar_t(0x0439), wchar_t(0x0000) };
    static std::wstring const o_m_End  = { wchar_t(0x043E), wchar_t(0x043C), wchar_t(0x0000) };
    static std::wstring const io_m_End = { wchar_t(0x0451), wchar_t(0x043C), wchar_t(0x0000) };
    static std::wstring const ie_m_End = { wchar_t(0x0435), wchar_t(0x043C), wchar_t(0x0000) };
    static std::wstring const soft_End = { wchar_t(0x044C), wchar_t(0x0000) };
    static std::wstring const j_End    = { wchar_t(0x0439), wchar_t(0x0000) };

    static std::array<std::array<std::wstring const*, 7>, 6> const dropEnds = { { { &a_End, &o_End, &ya_End, &ie_End, &soft_End, &j_End, nullptr },
        { &a_End, &ya_End, &yeru_End, &i_End, nullptr, nullptr, nullptr },
        { &ie_End, &u_End, &yu_End, &i_End, nullptr, nullptr, nullptr },
        { &u_End, &yu_End, &o_End, &ie_End, &soft_End, &ya_End, &a_End },
        { &oj_End, &io_j_End, &ie_j_End, &o_m_End, &io_m_End, &ie_m_End, &yu_End },
        { &ie_End, &i_End, nullptr, nullptr, nullptr, nullptr, nullptr } } };

    std::size_t const thisLen                         = wname.length();
    std::array<std::wstring const*, 7> const& endings = dropEnds[declension];
    for (auto itr = endings.begin(), end = endings.end(); (itr != end) && *itr; ++itr)
    {
        std::wstring const& ending = **itr;
        std::size_t const endLen   = ending.length();
        if (!(endLen <= thisLen))
            continue;

        if (wname.substr(thisLen - endLen, thisLen) == ending)
            return wname.substr(0, thisLen - endLen);
    }

    return wname;
}

bool utf8ToConsole(const std::string& utf8str, std::string& conStr)
{
#if PLATFORM == PLATFORM_WINDOWS
    std::wstring wstr;
    if (!Utf8toWStr(utf8str, wstr))
        return false;

    conStr.resize(wstr.size());
    CharToOemBuffW(&wstr[0], &conStr[0], wstr.size());
#else
    // not implemented yet
    conStr = utf8str;
#endif

    return true;
}

bool consoleToUtf8(const std::string& conStr, std::string& utf8str)
{
#if PLATFORM == PLATFORM_WINDOWS
    std::wstring wstr;
    wstr.resize(conStr.size());

    OemToCharBuffW(&conStr[0], &wstr[0], conStr.size());

    return WStrToUtf8(wstr, utf8str);
#else
    // not implemented yet
    utf8str = conStr;
    return true;
#endif
}

bool Utf8FitTo(const std::string& str, std::wstring const& search)
{
    std::wstring temp;
    if (!Utf8toWStr(str, temp))
        return false;

    // converting to lower case
    wstrToLower(temp);

    if (temp.find(search) == std::wstring::npos)
        return false;

    return true;
}

void utf8printf(FILE* out, const char* str, ...)
{
    va_list ap;
    va_start(ap, str);
    vutf8printf(out, str, &ap);
    va_end(ap);
}

void vutf8printf(FILE* out, const char* str, va_list* ap)
{
#if PLATFORM == PLATFORM_WINDOWS
    const size_t MAX_BUFFER_SIZE = 16 * 1024;

    char temp_buf[MAX_BUFFER_SIZE];

    size_t temp_len = vsnprintf(temp_buf, MAX_BUFFER_SIZE, str, *ap);
    if (temp_len == size_t(-1))
        temp_len = MAX_BUFFER_SIZE;

    std::wstring wtemp;
    Utf8toWStr(std::string(temp_buf, temp_len), wtemp);

    CharToOemBuffW(wtemp.data(), &temp_buf[0], std::min(wtemp.size(), MAX_BUFFER_SIZE));
    fprintf(out, "%s", temp_buf);
#else
    vfprintf(out, str, *ap);
#endif
}

std::string Trinity::Impl::ByteArrayToHexStr(uint8 const* bytes, size_t arrayLen, bool reverse /* = false */)
{
    int32 init = 0;
    int32 end = arrayLen;
    int8 op = 1;

    if (reverse)
    {
        init = arrayLen - 1;
        end = -1;
        op = -1;
    }

    std::string result;
    for (int32 i = init; i != end; i += op)
        result += fmt::format("{:02X}", uint32(bytes[i]));

    return result;
}

std::string GenerateRandomIdentifier(size_t length, std::string const& charset)
{
    auto randomchar = [&charset]() -> char {
        return charset[urand(0, charset.size() - 1)];
    };

    std::string string(length, 0);
    std::generate_n(string.begin(), length, randomchar);

    return string;
}

std::string GenerateRandomIdentifier(size_t length, std::function<char(void)>& randomCharacter)
{
    std::string string(length, 0);
    std::generate_n(string.begin(), length, randomCharacter);

    return string;
}

time_t LocalTimeToUTCTime(time_t time)
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    return time + _timezone;
#else
    return time + timezone;
#endif
}

tm tm_from_time_t(time_t t)
{
    return boost::posix_time::to_tm(boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(boost::posix_time::from_time_t(t)));
}

void Trinity::CopyTextToClipboard(std::string_view str)
{
#ifdef WIN32
    const char* output = str.data();
    const size_t length = strlen(output) + 1;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, length);
    memcpy(GlobalLock(hMem), output, length);
    GlobalUnlock(hMem);
    OpenClipboard(nullptr);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
#else
    static_cast<void>(str);
#endif
}

void Trinity::Impl::HexStrToByteArray(std::string_view str, uint8* out, size_t outlen, bool reverse /*= false*/)
{
    ASSERT(str.size() == (2 * outlen));

    int32 init = 0;
    int32 end = int32(str.length());
    int8 op = 1;

    if (reverse)
    {
        init = int32(str.length() - 2);
        end = -2;
        op = -1;
    }

    uint32 j = 0;
    for (int32 i = init; i != end; i += 2 * op)
    {
        char buffer[3] = { str[i], str[i + 1], '\0' };
        out[j++] = uint8(strtoul(buffer, nullptr, 16));
    }
}
