

#ifndef _GUILDMGR_H
#define _GUILDMGR_H

#include "Guild.h"

class GuildMgr : public Sunwell::Singleton<GuildMgr>
{
    friend class Sunwell::Singleton<GuildMgr>;

private:
    GuildMgr();
    ~GuildMgr();

public:
    Guild* GetGuildByLeader(uint64 guid) const;
    Guild* GetGuildById(uint32 guildId) const;
    Guild* GetGuildByName(std::string const& guildName) const;
    std::string GetGuildNameById(uint32 guildId) const;

    void LoadGuilds();
    void AddGuild(Guild* guild);
    void RemoveGuild(uint32 guildId);

    uint32 GenerateGuildId();
    void SetNextGuildId(uint32 Id) { NextGuildId = Id; }

    void ResetTimes();

protected:
    typedef std::unordered_map<uint32, Guild*> GuildContainer;
    uint32 NextGuildId;
    GuildContainer GuildStore;
};

#define sGuildMgr GuildMgr::instance()

#endif
