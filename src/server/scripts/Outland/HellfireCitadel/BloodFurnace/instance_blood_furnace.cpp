
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "blood_furnace.h"
#include "CreatureAI.h"

DoorData const BloodFuranceDoorData[] =
{
    { GO_THE_MAKER_DOOR, DATA_THE_MAKER, DOOR_TYPE_ROOM, BOUNDARY_NONE },
    { GO_THE_MAKER_EXIT_DOOR, DATA_THE_MAKER, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_BROGOKK_ENTRANCE_DOOR, DATA_BROGGOK, DOOR_TYPE_ROOM, BOUNDARY_NONE },
    { GO_BROGOKK_EXIT_DOOR, DATA_BROGGOK, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_KELIDAN_EXIT_DOOR, DATA_KELIDAN, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_BLOOD_FURANCE_FINAL_EXIT, DATA_KELIDAN, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { 0,             0,           DOOR_TYPE_ROOM, BOUNDARY_NONE } // END
};

ObjectData const BloodFuranceGameObjectData[] =
{
    { GO_BROGOKK_PRISON_CELL_1, GO_BROGOKK_PRISON_CELL_1 },
    { GO_BROGOKK_PRISON_CELL_2, GO_BROGOKK_PRISON_CELL_2 },
    { GO_BROGOKK_PRISON_CELL_3, GO_BROGOKK_PRISON_CELL_3 },
    { GO_BROGOKK_PRISON_CELL_4, GO_BROGOKK_PRISON_CELL_4 },
    { GO_BROGOKK_LEVER,         GO_BROGOKK_LEVER         },
    { GO_MAKER_PRISON_CELL_1,   GO_MAKER_PRISON_CELL_1   },
    { GO_MAKER_PRISON_CELL_2,   GO_MAKER_PRISON_CELL_2   },
    { GO_MAKER_PRISON_CELL_3,   GO_MAKER_PRISON_CELL_3   },
    { GO_MAKER_PRISON_CELL_4,   GO_MAKER_PRISON_CELL_4   },
    { GO_BROGOKK_EXIT_DOOR,     GO_BROGOKK_EXIT_DOOR     },
    { 0, 0                                               }
};

std::vector<uint64> _creaturesToHideOnMythic
{
    /** Orcs in cages on Brogokk event **/
    138127, 138128, 138129, 138130, 138131, 138132, 138133,
    138134, 138135, 138136, 138137, 138138, 138139, 138140,
    138141, 138142, 138143, 138144
    /** end of orcs **/
};

ObjectData const BloodFurnaceCreatureData[] =
{
    { NPC_BROGGOK, DATA_BROGGOK },
    { NPC_KELIDAN, DATA_KELIDAN },
    { NPC_MAGTHERIDON_BF, DATA_MAGTHERIDON_BF },
    { 0, 0 } // end
};

class instance_blood_furnace : public InstanceMapScript
{
public:
    instance_blood_furnace() : InstanceMapScript("instance_blood_furnace", 542) { }

    struct instance_blood_furnace_InstanceMapScript : public InstanceScript
    {
        instance_blood_furnace_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            SetHeaders("BF");
            SetBossNumber(BF_MAX_ENCOUNTER);
            LoadDoorData(BloodFuranceDoorData);
            LoadObjectData(BloodFurnaceCreatureData, BloodFuranceGameObjectData);
        }

        void Initialize() override
        {
            memset(&_prisonGUIDs, 0, sizeof(_prisonGUIDs));
            memset(&_prisonerCounter, 0, sizeof(_prisonerCounter));

            for (uint8 i = 0; i < 4; ++i)
                _prisonersCell[i].clear();
        }

        bool CheckRequiredBosses( uint32 bossId, Player const* player ) const override
        {
            if ( bossId == DATA_BROGGOK )
                return GetBossState( DATA_THE_MAKER ) == DONE;
            else if ( bossId == DATA_KELIDAN )
                return GetBossState( DATA_THE_MAKER ) == DONE && GetBossState( DATA_BROGGOK ) == DONE;

            return true;
        }

        bool BuildMythicResurrectGossip( Player* player ) override
        {
            if ( !player || player->IsInCombat() || IsEncounterInProgress() )
                return false;

            if ( GetBossState( DATA_THE_MAKER ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to The Maker", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_THE_MAKER,
                    "", 0 );

            if ( GetBossState( DATA_BROGGOK ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Broggok.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_BROGGOK,
                    "", 0 );

            if ( GetBossState( DATA_KELIDAN ) == DONE )
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem( -1, GOSSIP_ICON_CHAT, "Teleport to Keli'dan the Breaker.", Mythic::MYTHIC_GOSSIP_SENDER_TELEPORT_BOSS, DATA_KELIDAN,
                    "", 0 );

            return true;
        }

        void OnPlayerUseMythicShortcut( uint32 id, Player* player ) override
        {
            switch ( id )
            {
                case DATA_THE_MAKER:
                {
                    player->NearTeleportTo( { 327.170013f, 137.815994f, 9.715668f, 4.721210f }, false, false, true );
                    break;
                }
                case DATA_BROGGOK:
                {
                    player->NearTeleportTo( { 456.517914f, 57.654778f, 9.616262f, 4.816797f }, false, false, true );
                    break;
                }
                case DATA_KELIDAN:
                {
                    player->NearTeleportTo( { 325.862000f, -87.208702f, -24.651199f, 5.873140f }, false, false, true );
                    break;
                }
            }
        }

        void OnCreatureCreate(Creature* creature) override
        {
            if ( creature->GetEntry() == NPC_HELLFIRE_CHANNELER )
            {
                if ( creature->m_mythicScaling )
                    creature->m_mythicScaling->m_canBeRespawned = false;
                //creature->DespawnOrUnsummon();
                return;
            }

            InstanceScript::OnCreatureCreate(creature);

            if (creature->GetEntry() == NPC_NASCENT_FEL_ORC)
                StorePrisoner(creature);

            if ( creature->GetEntry() == NPC_BROGGOK && creature->IsAIEnabled )
                creature->AI()->Reset();
        }

        void OnUnitDeath(Unit* unit) override
        {
            if (unit && unit->GetEntry() == NPC_NASCENT_FEL_ORC)
                PrisonerDied(unit->GetGUID());

            switch (unit->GetEntry())
            {
                case NPC_THE_MAKER:
                case NPC_BROGGOK:
                {
                    if (IsMythic() && sMythicMgr.GetCurrentMythicEventId() == Mythic::MythicEventType::MYTHIC_EVENT_CRYSTALS)
                        SummonMythicEventCrystal(unit);
                    break;
                }
            }
        }

        bool SetBossState(uint32 bossId, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(bossId, state))
                return false;

            if (bossId == DATA_BROGGOK)
                UpdateBroggokEvent(static_cast<uint32>(state));
            else if (bossId == DATA_KELIDAN && state == DONE)
            {
                if (IsMythic() && IsMythicTimerRunning())
                    FinishMythicInstance();
            }

            return true;
        }

        uint64 GetData64(uint32 data) const override
        {
            switch (data)
            {
                case DATA_PRISON_CELL1:
                    return GetGameObject( GO_BROGOKK_PRISON_CELL_1 )->GetGUID();
                case DATA_PRISON_CELL2:
                    return GetGameObject( GO_BROGOKK_PRISON_CELL_2 )->GetGUID();
                case DATA_PRISON_CELL3:
                    return GetGameObject( GO_BROGOKK_PRISON_CELL_3 )->GetGUID();
                case DATA_PRISON_CELL4:
                    return GetGameObject( GO_BROGOKK_PRISON_CELL_4 )->GetGUID();
                default:
                    return 0;
            }

            return 0;
        }

        void Update(uint32 diff) override
        {
            InstanceScript::Update(diff);
            if (IsMythicTimerRunning())
                UpdateMythicTimers(diff);
        }

    protected:
        void ResetPrisons()
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                _prisonerCounter[i] = _prisonersCell[i].size();
                ResetPrisoners(_prisonersCell[i]);
            }

            for ( auto const& ent : { DATA_PRISON_CELL1, DATA_PRISON_CELL2, DATA_PRISON_CELL3, DATA_PRISON_CELL4 } )
            {
                HandleGameObject( GetData64( ent ), false );
            }
        }

        void ResetPrisoners(std::set<uint64> prisoners)
        {
            for (std::set<uint64>::iterator i = prisoners.begin(); i != prisoners.end(); ++i)
                if (Creature* prisoner = instance->GetCreature(*i))
                    ResetPrisoner(prisoner);
        }

        void ResetPrisoner(Creature* prisoner)
        {
            if (IsMythic())
            {
                if (prisoner->m_mythicScaling)
                    prisoner->m_mythicScaling->m_canBeRespawned = false;
                //! Never despawn in the same update
                prisoner->DespawnOrUnsummon(5s);
                return;
            }

            if (!prisoner->IsAlive())
                prisoner->Respawn(true);
            prisoner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
        }

        void StorePrisoner(Creature* creature)
        {
            float posX = creature->GetPositionX();
            float posY = creature->GetPositionY();

            if (posX >= 405.0f && posX <= 423.0f)
            {
                if (IsMythic())
                    creature->SetPhaseMask(8, true);

                if (posY >= 106.0f && posY <= 123.0f)
                {
                    _prisonersCell[0].insert(creature->GetGUID());
                    ++_prisonerCounter[0];
                    ResetPrisoner(creature);
                }
                else if (posY >= 76.0f && posY <= 91.0f)
                {
                    _prisonersCell[1].insert(creature->GetGUID());
                    ++_prisonerCounter[1];
                    ResetPrisoner(creature);
                }
            }
            else if (posX >= 490.0f && posX <= 506.0f)
            {
                if (IsMythic())
                    creature->SetPhaseMask(8, true);

                if (posY >= 106.0f && posY <= 123.0f)
                {
                    _prisonersCell[2].insert(creature->GetGUID());
                    ++_prisonerCounter[2];
                    ResetPrisoner(creature);
                }
                else if (posY >= 76.0f && posY <= 91.0f)
                {
                    _prisonersCell[3].insert(creature->GetGUID());
                    ++_prisonerCounter[3];
                    ResetPrisoner(creature);
                }
            }
        }

        void PrisonerDied(uint64 guid)
        {
            if (_prisonersCell[0].find(guid) != _prisonersCell[0].end() && --_prisonerCounter[0] <= 0)
                ActivateCell(DATA_PRISON_CELL2);
            else if (_prisonersCell[1].find(guid) != _prisonersCell[1].end() && --_prisonerCounter[1] <= 0)
                ActivateCell(DATA_PRISON_CELL3);
            else if (_prisonersCell[2].find(guid) != _prisonersCell[2].end() && --_prisonerCounter[2] <= 0)
                ActivateCell(DATA_PRISON_CELL4);
            else if (_prisonersCell[3].find(guid) != _prisonersCell[3].end() && --_prisonerCounter[3] <= 0 && !IsMythic())
                ActivateCell(GO_BROGOKK_EXIT_DOOR);
        }

        void ActivateCell(uint32 id)
        {
            switch (id)
            {
                case DATA_PRISON_CELL1:
                case DATA_PRISON_CELL2:
                case DATA_PRISON_CELL3:
                case DATA_PRISON_CELL4:
                    HandleGameObject(GetData64(id), true);
                    ActivatePrisoners(_prisonersCell[id - DATA_PRISON_CELL1]);
                    break;
                case GO_BROGOKK_EXIT_DOOR:
                    Creature* broggok = GetCreature(DATA_BROGGOK);
                    CreatureAI* ai = broggok ? broggok->AI() : nullptr;
                    GameObject* door = GetGameObject(GO_BROGOKK_EXIT_DOOR);
                    if (ai && door)
                    {
                        ai->DoAction(ACTION_ACTIVATE_BROGGOK);
                        HandleGameObject(door->GetGUID(), true);
                    }
                    break;
            }
        }

        void ActivatePrisoners(std::set<uint64> prisoners)
        {
            for (uint64 guid : prisoners)
            {
                if (Creature* prisoner = instance->GetCreature(guid))
                {
                    prisoner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                    prisoner->SetInCombatWithZone();
                }
            }
        }

        void UpdateBroggokEvent(uint32 data)
        {
            if (data == IN_PROGRESS)
                ActivateCell(DATA_PRISON_CELL1);
            else if (data == NOT_STARTED)
            {
                ResetPrisons();
                if (GameObject* lever = GetGameObject(GO_BROGOKK_LEVER))
                    lever->Respawn();
            }
        }

    private:
        uint64 _prisonGUIDs[4];
        std::set<uint64> _prisonersCell[4];
        uint8 _prisonerCounter[4];
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_blood_furnace_InstanceMapScript(map);
    }
};

void AddSC_instance_blood_furnace()
{
    new instance_blood_furnace();
}

