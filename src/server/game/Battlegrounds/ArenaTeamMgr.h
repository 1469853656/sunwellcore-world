

#ifndef _ARENATEAMMGR_H
#define _ARENATEAMMGR_H

#include "ArenaTeam.h"
#include "Singleton.h"

class ArenaTeamMgr : public Sunwell::Singleton<ArenaTeamMgr>
{
    friend class Sunwell::Singleton<ArenaTeamMgr>;
    ArenaTeamMgr();
    ~ArenaTeamMgr();

public:
    typedef std::unordered_map<uint32, ArenaTeam*> ArenaTeamContainer;

    ArenaTeam* GetArenaTeamById(uint32 arenaTeamId) const;

    void LoadArenaTeams();
    void AddArenaTeam(ArenaTeam* arenaTeam);
    void RemoveArenaTeam(uint32 Id);

    ArenaTeamContainer::iterator GetArenaTeamMapBegin() { return ArenaTeamStore.begin(); }
    ArenaTeamContainer::iterator GetArenaTeamMapEnd() { return ArenaTeamStore.end(); }

    void DistributeArenaPoints();

    uint32 GenerateArenaTeamId();
    void SetNextArenaTeamId(uint32 Id) { NextArenaTeamId = Id; }
    uint32 GetNextArenaTeamId() const { return NextArenaTeamId; }

    uint32 GetNextArenaLogId() { return ++LastArenaLogId; }
    void SetLastArenaLogId(uint32 id) { LastArenaLogId = id; }

protected:
    uint32 NextArenaTeamId;
    ArenaTeamContainer ArenaTeamStore;
    uint32 LastArenaLogId;
};

#define sArenaTeamMgr ArenaTeamMgr::instance()

#endif
