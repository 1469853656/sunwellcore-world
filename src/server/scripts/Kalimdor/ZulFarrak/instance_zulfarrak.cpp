#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "zulfarrak.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "GridNotifiers.h"
#include "CellImpl.h"

ObjectData const ZfCreatureData[] =
{
    { NPC_THEKA_MARTYR,             BOSS_THEKA_MARTYR           },
    { NPC_ZUM_RAH,                  BOSS_WITCH_DOCTOR_ZUMRAH    },
    { NPC_ZF_RUUZLU,                BOSS_RUUZLU                 },
    { NPC_ZF_UKORZ,                 BOSS_UKORZ_SANDSCALP        },
    { NPC_ANTUSUL,                  BOSS_ANTUSUL                },
    { NPC_GAHZRILLA,                BOSS_GAHZRILLA              },
    { 0, 0 }
};

DoorData const ZfDoorData[] =
{
   { GO_FINAL_BOSS_ROOM_DOOR,     BOSS_GOBLIN_STAIRS,  DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
   { 0,                   0,             DOOR_TYPE_ROOM,    BOUNDARY_NONE } // END
};

class instance_zulfarrak : public InstanceMapScript
{
public:
    instance_zulfarrak() : InstanceMapScript("instance_zulfarrak", 209) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_zulfarrak_InstanceMapScript(map);
    }

    struct instance_zulfarrak_InstanceMapScript : public InstanceScript
    {
        instance_zulfarrak_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            SetBossNumber(ZULFARRAK_BOSS_MAX);
            LoadObjectData(ZfCreatureData, nullptr);
            //LoadDoorData(ZfDoorData);
        }

        void Initialize() override
        {
            GahzRillaEncounter = NOT_STARTED;
            antusulGUID = 0;
            StairCreatureGUIDs.clear();
            StairEventCagesGUIDs.clear();
            task.CancelAll();
        }

        bool BuildMythicResurrectGossip( Player* player ) override
        {
            if ( !player || player->IsInCombat() || IsEncounterInProgress() )
                return false;

            if ( GetBossState( BOSS_ANTUSUL ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Antu'sul", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, BOSS_ANTUSUL,
                    "", 0 );

            if ( GetBossState( BOSS_THEKA_MARTYR ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Theka the martyr.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, BOSS_THEKA_MARTYR,
                    "", 0 );

            if ( GetBossState( BOSS_WITCH_DOCTOR_ZUMRAH ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Witch doctor Zum'rah", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, BOSS_WITCH_DOCTOR_ZUMRAH,
                    "", 0 );

            if ( GetBossState( BOSS_HYDROMANCER_VELRATHA ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Hydromancer Velratha", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, BOSS_HYDROMANCER_VELRATHA,
                    "", 0 );

            if ( GetBossState( BOSS_SANDSCLAP_RUUZLU ) == DONE)
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Ukorz & Ruuzlu.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, BOSS_SANDSCLAP_RUUZLU,
                    "", 0 );
            return true;
        }

        void OnPlayerUseMythicShortcut( uint32 id, Player* player ) override
        {
            switch ( id )
            {
                case BOSS_ANTUSUL:
                {
                    player->NearTeleportTo( { 1815.781128f, 670.431274f, 14.927388f, 4.892269f }, false, false, true );
                    break;
                }
                case BOSS_THEKA_MARTYR:
                {
                    player->NearTeleportTo( { 1781.403687f, 860.132324f, 8.877005f, 4.341694f }, false, false, true );
                    break;
                }
                case BOSS_WITCH_DOCTOR_ZUMRAH:
                {
                    player->NearTeleportTo( { 1872.067871f, 1027.031006f, 8.876945f, 2.576119f }, false, false, true );
                    break;
                }
                case BOSS_HYDROMANCER_VELRATHA:
                {
                    player->NearTeleportTo( { 1702.326172f, 1195.995239f, 9.752222f, -2.302264f }, false, false, true );
                    break;
                }
                case BOSS_SANDSCLAP_RUUZLU:
                {
                    player->NearTeleportTo( { 1727.479980f, 1017.349976f, 54.910198f, 1.006140f }, false, false, true );
                    break;
                }
            }
        }

        void SetData(uint32 type, uint32 value) override
        {
            if (type == INSTANCE_DATA_RESET_CAGES)
            {
                for (uint64 const guid : StairEventCagesGUIDs)
                {
                    if (GameObject* cage = instance->GetGameObject(guid))
                    {
                        cage->SetGoState(GO_STATE_READY);
                        //! reset flags to default
                        cage->SetUInt32Value(GAMEOBJECT_FLAGS, 32);
                    }
                }
            } else if (type == INSTANCE_DATA_BLY_DEATH)
            {
                RewardMythicWithTime(15 * MINUTE * IN_MILLISECONDS);
                Creature* bly = instance->GetCreature(GetData64(NPC_SERGEANT_BLY));
                if (bly)
                    bly->MonsterTextEmote("Congratulations! Your total mythic timer has been reduced by 15 minutes!", nullptr, true);
            }
        }

        void HandleStairEvent(EncounterState state)
        {
            if (state != IN_PROGRESS)
                return;

            for (uint64 const guid : StairEventCagesGUIDs)
            {
                if (GameObject* cage = instance->GetGameObject(guid))
                {
                    cage->UseDoorOrButton();
                    cage->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                }
            }

            task.Schedule(1500ms, [&](TaskContext /*func*/)
            {
                for (auto const& it : StairCreatureGUIDs)
                {
                    Creature* boss = instance->GetCreature(it.second);
                    if (boss && boss->IsAIEnabled)
                        boss->AI()->DoAction(ACTION_LEAVE_CAGE);
                }
            });
        }

        bool SetBossState(uint32 bossId, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(bossId, state))
                return false;

            switch (bossId)
            {
                case BOSS_STAIRS_EVENT:
                {
                    HandleStairEvent(state);
                    break;
                }
                case BOSS_RUUZLU:
                case BOSS_UKORZ_SANDSCALP:
                {
                    if (!IsMythic())
                        return true;

                    Creature* ruuzlu = GetCreature(BOSS_RUUZLU);
                    Creature* ukorz = GetCreature(BOSS_UKORZ_SANDSCALP);

                    if (state == FAIL)
                    {
                        DoRemoveAurasDueToSpellOnPlayers(SPELL_CREATURE_OF_NIGHTMARE_ZF);
                        DoRemoveAurasDueToSpellOnPlayers(SPELL_ZOMBIE_UKORZ_BRAIN_HACKER);
                        if (ruuzlu && ukorz)
                        {
                            if (ruuzlu->isDead())
                                ruuzlu->Respawn();

                            if (ukorz->isDead())
                                ukorz->Respawn();

                            DoRemoveAurasDueToSpellOnPlayers(SPELL_RUUZLU_KNEEL);
                            DoRemoveAurasDueToSpellOnPlayers(SPELL_RUUZLU_TORNADO);
                            DoRemoveAurasDueToSpellOnPlayers(SPELL_CREATURE_OF_NIGHTMARE_ZF);
                            ruuzlu->AI()->DoAction(ACTION_DESPAWN_SUMMONS);
                            ukorz->AI()->DoAction(ACTION_DESPAWN_SUMMONS);
                            ruuzlu->DespawnOrUnsummon(0U, 30U);
                            ukorz->DespawnOrUnsummon(0U, 30U);
                        }
                        return false;
                    }

                    if (GetBossState(BOSS_RUUZLU) == DONE && GetBossState(BOSS_UKORZ_SANDSCALP) == DONE)
                    {
                        SetBossState(BOSS_SANDSCLAP_RUUZLU, DONE);
                        if (ruuzlu && ukorz)
                        {
                            ruuzlu->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            ukorz->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
                    }
                    break;
                }
                case BOSS_SANDSCLAP_RUUZLU:
                {
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_CREATURE_OF_NIGHTMARE_ZF);
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_ZOMBIE_UKORZ_BRAIN_HACKER);
                    if (IsMythic())
                        FinishMythicInstance();
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        bool CheckRequiredBosses( uint32 bossId, Player const* player ) const override
        {
            if ( bossId == BOSS_UKORZ_SANDSCALP )
                return GetBossState( BOSS_STAIRS_EVENT ) == DONE;
            else if ( bossId == BOSS_SANDFURY_EXECUTIONER && IsMythic() )
                return GetBossState( BOSS_HYDROMANCER_VELRATHA ) == DONE &&
                GetBossState( BOSS_THEKA_MARTYR ) == DONE && GetBossState( BOSS_ANTUSUL ) == DONE;

            return true;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            InstanceScript::OnCreatureCreate(creature);

            switch (creature->GetEntry())
            {
                case NPC_GAHZRILLA:
                    if (GahzRillaEncounter >= IN_PROGRESS)
                        creature->DisappearAndDie();
                    else
                        GahzRillaEncounter = IN_PROGRESS;
                    break;
                case NPC_ANTUSUL:
                    antusulGUID = creature->GetGUID();
                    break;
                case NPC_SERGEANT_BLY:
                case NPC_RAVEN:
                case NPC_ORO:
                case NPC_WEEGLI:
                case NPC_MURTA:
                {
                    if (GetBossState(BOSS_STAIRS_EVENT) == DONE)
                    {
                        creature->DespawnOrUnsummon(1s);
                        break;
                    }

                    creature->SetPassive();
                    StairCreatureGUIDs[creature->GetEntry()] = creature->GetGUID();
                    break;
                }
                default:
                    break;
            }
        }

        uint64 GetData64(uint32 type) const override
        {
            switch (type)
            {
                case NPC_SERGEANT_BLY:
                case NPC_RAVEN:
                case NPC_ORO:
                case NPC_WEEGLI:
                case NPC_MURTA:
                {
                    auto it = StairCreatureGUIDs.find(type);
                    if (it != StairCreatureGUIDs.end())
                        return it->second;
                    return 0;
                }
                default:
                    break;
            }

            return 0;
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            InstanceScript::OnGameObjectCreate(go);

            switch (go->GetEntry())
            {
                case GO_STAIR_EVENT_CAGES_1:
                case GO_STAIR_EVENT_CAGES_2:
                case GO_STAIR_EVENT_CAGES_3:
                case GO_STAIR_EVENT_CAGES_4:
                case GO_STAIR_EVENT_CAGES_5:
                    StairEventCagesGUIDs.push_back(go->GetGUID());
                    break;
                case GO_FINAL_BOSS_ROOM_DOOR:
                    if (GetBossState(BOSS_GOBLIN_STAIRS) == DONE)
                        HandleGameObject(0, true, go);
                    break;
                default:
                    break;
            }
        }

        void Update(uint32 diff) override
        {
            InstanceScript::Update(diff);
            if (IsMythicTimerRunning())
                UpdateMythicTimers(diff);

            task.Update(diff);
        }

        bool CanReportChest(uint32 chestId) const override
        {
            switch (chestId)
            {
                case GO_SHALLOW_GRAVE:
                case GO_SHALLOW_GRAVE_2:
                    return false;
                default:
                    return true;
            }
            return true;
        }

        void OnUnitDeath(Unit* unit) override
        {
            switch (unit->GetEntry())
            {
                case NPC_ANTUSUL:
                case NPC_THEKA_MARTYR:
                case NPC_ZUM_RAH:
                {
                    if (IsMythic() && sMythicMgr.GetCurrentMythicEventId() == Mythic::MythicEventType::MYTHIC_EVENT_CRYSTALS)
                        SummonMythicEventCrystal(unit);
                    break;
                }
            }
        }
    private:
        uint32 GahzRillaEncounter;

        uint64 antusulGUID;
        //! Holds GUIDs for bosses at the stair event
        std::unordered_map<uint32 /*entry*/, uint64 /*guid*/> StairCreatureGUIDs;
        std::vector<uint64> StairEventCagesGUIDs;
        TaskScheduler task;
    };
};

void AddSC_instance_zulfarrak()
{
    new instance_zulfarrak();
}
