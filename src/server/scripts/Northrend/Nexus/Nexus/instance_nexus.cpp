
#include "ScriptMgr.h"

#include "nexus.h"
#include "MapReference.h"
#include "Player.h"

DoorData const doorData[] =
{
    { GO_TELESTRA_SPHERE,    DATA_TELESTRA_ORB,    DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_ANOMALUS_SPHERE,    DATA_ANOMALUS_ORB,    DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_ORMOROK_SPHERE,     DATA_ORMOROK_ORB,     DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { 0,                    0,                     DOOR_TYPE_ROOM,       BOUNDARY_NONE } // end
};

ObjectData const CreatureData[] =
{
    { NPC_KERISTRASZA, DATA_KERISTRASZA_EVENT },
    { NPC_GRAND_MAGUS_TELESTRA, DATA_MAGUS_TELESTRA_EVENT },
    { NPC_ANOMALUS, DATA_ANOMALUS_EVENT },
    { 0, 0 }
};

struct MythicGauntletAdditionalAdds
{
    uint32 entry = 0;
    Position pos{ };
};

std::vector<MythicGauntletAdditionalAdds> AdditionalGauntletTrash =
{
    { NPC_ALLIANCE_CLERIC, { 431.622f, 192.007f, -35.0196f, 4.82431f } },
    //{ NPC_ALLIANCE_RANGER, { 436.209f, 180.406f, -35.0196f, 4.82431f } },
    //{ NPC_ALLIANCE_RANGER, { 445.760f, 179.445f, -35.0196f, 5.23900f } },
    { NPC_ALLIANCE_BERSERKER, { 456.408f, 153.209f, -35.0203f, 2.09663f } },
    { NPC_ALLIANCE_BERSERKER, { 467.814f, 161.708f, -35.0203f, 2.41001f } }
};

class instance_nexus : public InstanceMapScript
{
public:
    instance_nexus() : InstanceMapScript("instance_nexus", 576) { }

    struct instance_nexus_InstanceMapScript : public InstanceScript
    {
        instance_nexus_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            TeamIdInInstance = TEAM_NEUTRAL;
        }

        void Initialize() override
        {
            SetHeaders("NEXUS");
            SetBossNumber(MAX_ENCOUNTERS);
            LoadDoorData(doorData);
            LoadObjectData(CreatureData, nullptr);
            gauntletTrigger = 0;
        }

        bool BuildMythicResurrectGossip( Player* player ) override
        {
            if ( !player || player->IsInCombat() || IsEncounterInProgress() )
                return false;

            if ( GetBossState( DATA_MAGUS_TELESTRA_EVENT ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Grand Magus Telestra.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_MAGUS_TELESTRA_EVENT,
                    "", 0 );

            if ( GetBossState( DATA_ANOMALUS_EVENT ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Anomalus.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_ANOMALUS_EVENT,
                    "", 0 );

            if ( GetBossState( DATA_ORMOROK_EVENT ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Ormorok.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_ORMOROK_EVENT,
                    "", 0 );

            if ( GetBossState( DATA_KERISTRASZA_EVENT) == DONE)
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Keristrasza.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_ORMOROK_EVENT,
                    "", 0 );
            return true;
        }

        void OnPlayerUseMythicShortcut( uint32 id, Player* player ) override
        {
            switch ( id )
            {
                case DATA_MAGUS_TELESTRA_EVENT:
                {
                    player->NearTeleportTo( { 494.726990f, 89.128799f, -16.041300f, 6.021390f }, false, false, true );
                    break;
                }
                case DATA_ANOMALUS_EVENT:
                {
                    player->NearTeleportTo( { 637.669983f, -289.121002f, -9.132520f, 4.747290f }, false, false, true );
                    break;
                }
                case DATA_ORMOROK_EVENT:
                {
                    player->NearTeleportTo( { 264.959015f, -225.414993f, -9.017650f, 5.899210f }, false, false, true );
                    break;
                }
                case DATA_KERISTRASZA_EVENT:
                {
                    player->NearTeleportTo( { 301.454010f, -5.458310f, -15.480800f, 3.124140f }, false, false, true );
                    break;
                }
            }
        }

        void OnPlayerEnterMythic(Player* player) override
        {
            InstanceScript::OnPlayerEnterMythic(player);
            if (!gauntletTrigger)
            {
                Position spawnPos{ 370.666f, 152.633f, -34.6224f, 0.158269f };
                Creature* gauntlet = instance->SummonCreature(NPC_MYTHIC_GAUNTLET_TRIGGER, spawnPos);
                gauntletTrigger = gauntlet ? gauntlet->GetGUID() : 0;

                for (MythicGauntletAdditionalAdds const& data : AdditionalGauntletTrash)
                    instance->SummonCreature(data.entry, data.pos);
            }
        }

        void OnCreatureCreate(Creature* creature) override
        {
            InstanceScript::OnCreatureCreate(creature);
            if (TeamIdInInstance == TEAM_NEUTRAL)
            {
                Map::PlayerList const& players = instance->GetPlayers();
                if (!players.isEmpty())
                    if (Player* pPlayer = players.begin()->GetSource())
                        TeamIdInInstance = pPlayer->GetTeam();
            }

            switch (creature->GetEntry())
            {
                case NPC_ALLIANCE_RANGER:
                    creature->setFaction(16);
                    if (TeamIdInInstance == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_RANGER);
                    break;
                case NPC_ALLIANCE_BERSERKER:
                    creature->setFaction(16);
                    if (TeamIdInInstance == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_BERSERKER);
                    break;
                case NPC_ALLIANCE_COMMANDER:
                    creature->setFaction(16);
                    if (TeamIdInInstance == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_COMMANDER);
                    if (IsMythic())
                    {
                        creature->DespawnOrUnsummon(3s);
                        creature->m_mythicScaling->m_canBeRespawned = false;
                        creature->SetRespawnDelay(86400);
                    }
                    break;
                case NPC_ALLIANCE_CLERIC:
                    creature->setFaction(16);
                    if (TeamIdInInstance == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_CLERIC);
                    break;
                case NPC_COMMANDER_STOUTBEARD:
                    creature->setFaction(16);
                    if (TeamIdInInstance == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_COMMANDER_KOLURG);
                    break;
                default:
                    break;
            }
        }

        void OnUnitDeath(Unit* unit) override
        {
            switch (unit->GetEntry())
            {
                case NPC_GRAND_MAGUS_TELESTRA:
                case NPC_ANOMALUS:
                case NPC_ORMOROK:
                {
                    if (IsMythic() && sMythicMgr.GetCurrentMythicEventId() == Mythic::MythicEventType::MYTHIC_EVENT_CRYSTALS)
                        SummonMythicEventCrystal(unit);
                }
            }
        }

        void Update(uint32 diff) override
        {
            InstanceScript::Update(diff);
            if (IsMythicTimerRunning())
                UpdateMythicTimers(diff);
        }

        void OnGameObjectCreate(GameObject* gameObject) override
        {
            InstanceScript::OnGameObjectCreate(gameObject);

            switch (gameObject->GetEntry())
            {
                case GO_TELESTRA_SPHERE:
                    if (GetBossState(DATA_TELESTRA_ORB) != DONE && GetBossState(DATA_MAGUS_TELESTRA_EVENT) == DONE)
                        gameObject->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    AddDoor(gameObject, true);
                    break;
                case GO_ANOMALUS_SPHERE:
                    if (GetBossState(DATA_ANOMALUS_ORB) != DONE && GetBossState(DATA_ANOMALUS_EVENT) == DONE)
                        gameObject->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    AddDoor(gameObject, true);
                    break;
                case GO_ORMOROK_SPHERE:
                    if (GetBossState(DATA_ORMOROK_ORB) != DONE && GetBossState(DATA_ORMOROK_EVENT) == DONE)
                        gameObject->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    AddDoor(gameObject, true);
                    break;
            }
        }

        void OnGameObjectRemove(GameObject* gameObject) override
        {
            InstanceScript::OnGameObjectRemove(gameObject);

            switch (gameObject->GetEntry())
            {
                case GO_TELESTRA_SPHERE:
                case GO_ANOMALUS_SPHERE:
                case GO_ORMOROK_SPHERE:
                    AddDoor(gameObject, false);
                    break;
                default:
                    break;
            }
        }

        void SetData(uint32 type, uint32 /*value*/) override
        {
            switch (type)
            {
                case GO_TELESTRA_SPHERE:
                    SetBossState(DATA_TELESTRA_ORB, NOT_STARTED);
                    SetBossState(DATA_TELESTRA_ORB, DONE);
                    break;
                case GO_ANOMALUS_SPHERE:
                    SetBossState(DATA_ANOMALUS_ORB, NOT_STARTED);
                    SetBossState(DATA_ANOMALUS_ORB, DONE);
                    break;
                case GO_ORMOROK_SPHERE:
                    SetBossState(DATA_ORMOROK_ORB, NOT_STARTED);
                    SetBossState(DATA_ORMOROK_ORB, DONE);
                    break;
            }
        }

        bool SetBossState(uint32 id, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(id, state))
                return false;

            if (state == DONE && id == DATA_KERISTRASZA_EVENT)
            {
                if (IsMythic())
                    FinishMythicInstance();
            }

            if (state != DONE || id > DATA_ORMOROK_EVENT)
                return true;

            BossInfo const* bossInfo = GetBossInfo(id + DATA_TELESTRA_ORB);
            for (DoorSet::const_iterator i = bossInfo->door[DOOR_TYPE_PASSAGE].begin(); i != bossInfo->door[DOOR_TYPE_PASSAGE].end(); ++i)
                (*i)->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            return true;
        }

    private:
        TeamId TeamIdInInstance;
        uint64 gauntletTrigger;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_nexus_InstanceMapScript(map);
    }
};

void AddSC_instance_nexus()
{
    new instance_nexus();
}
