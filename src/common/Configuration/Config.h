
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <list>
#include "Singleton.h"
#include "Common.h"

class ConfigMgr : public Sunwell::Singleton<ConfigMgr>
{
    friend class Sunwell::Singleton<ConfigMgr>;

    public:
        /// Method used only for loading main configuration files (authserver.conf and worldserver.conf)
        bool LoadInitial(char const* file);
        /// This method loads additional configuration files
        bool LoadMore(char const* file);

        bool Reload();

        std::string GetStringDefault(const char* name, const std::string& def);
        std::string GetStringDefault(std::string const& name, std::string const& def)
        {
            return GetStringDefault(name.c_str(), def);
        }
        bool GetBoolDefault(const char* name, bool def);
        bool GetBoolDefault(std::string const& name, bool def)
        {
            return GetBoolDefault(name.c_str(), def);
        }
        int GetIntDefault(const char* name, int def);
        int GetIntDefault(std::string const& name, int def)
        {
            return GetIntDefault(name.c_str(), def);
        }
        float GetFloatDefault(const char* name, float def);
        float GetFloatDefault(std::string const& name, float def)
        {
            return GetFloatDefault(name.c_str(), def);
        }

        // Returns main filename
        std::string const& GetMainConfigFileName();

    private:
        std::string _mainConfigFileName;
        std::set<std::string> _additionalConfigFileNames;
        std::map<std::string,std::string> m_config;
        std::mutex _configLock;
};

#define sConfigMgr ConfigMgr::instance()

#endif
