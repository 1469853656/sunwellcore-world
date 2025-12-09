
#include "Config.h"
#include "Errors.h"
#include <boost/algorithm/string/trim.hpp>
#include <fstream>

bool ConfigMgr::LoadInitial(char const* file)
{
    ASSERT(file);

    std::lock_guard guard(_configLock);

    _mainConfigFileName = file;
    m_config.clear();
    std::fstream fs;
    fs.open(file);
    if (!fs.is_open()) return false;

    std::string input;
    while (!fs.eof())
    {
        std::getline(fs, input);
        if (input.empty() || input[0] == '#') continue;
        auto eq = input.find('=');
        if (eq == std::string::npos) continue;
        std::string key = input.substr(0, eq);
        std::string value  = input.substr(eq+1);
        boost::algorithm::trim(key);
        boost::algorithm::trim(value);
        if (value[0] == '"') value = value.substr(1, value.size()-2);
        m_config[key]=value;
    }
    return true;
}

bool ConfigMgr::LoadMore(char const* file)
{
    ASSERT(file);

    std::lock_guard guard(_configLock);

    _additionalConfigFileNames.emplace(file);
    std::fstream fs;
    fs.open(file);
    if (!fs.is_open()) return false;

    std::string input;
    while (!fs.eof())
    {
        std::getline(fs, input);
        if (input.empty() || input[0] == '#') continue;
        auto eq = input.find('=');
        if (eq == std::string::npos) continue;
        std::string key   = input.substr(0, eq);
        std::string value = input.substr(eq + 1);
        boost::algorithm::trim(key);
        boost::algorithm::trim(value);
        if (value[0] == '"') value = value.substr(1, value.size() - 2);
        m_config[key] = value;
    }
    return true;
}

bool ConfigMgr::Reload()
{
    if (!LoadInitial(_mainConfigFileName.c_str()))
        return false;

    for (std::string const& fileNames : _additionalConfigFileNames)
        if (!LoadMore(fileNames.c_str()))
            return false;

    return true;
}

std::string ConfigMgr::GetStringDefault(const char* name, const std::string& def)
{
    auto val= m_config.find(name);
    return (val == m_config.end() ? def : val->second);
}

bool ConfigMgr::GetBoolDefault(const char* name, bool def)
{
    auto val= m_config.find(name);
    if (val == m_config.end()) return def;
    return (val->second == "true" || val->second == "TRUE" || val->second == "yes" || val->second == "YES" || val->second == "1");
}

int ConfigMgr::GetIntDefault(const char* name, int def)
{
    auto val= m_config.find(name);
    return (val == m_config.end() ? def : std::stoi(val->second));
}

float ConfigMgr::GetFloatDefault(const char* name, float def)
{
    auto val= m_config.find(name);
    return (val == m_config.end() ? def : std::stof(val->second));
}

std::string const& ConfigMgr::GetMainConfigFileName()
{
    std::lock_guard guard(_configLock);
    return _mainConfigFileName;
}

