
/* ScriptData
SDName: Stranglethorn_Vale
SD%Complete: 100
SDComment: Quest support: 592
SDCategory: Stranglethorn Vale
EndScriptData */

/* ContentData
npc_yenniku
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "SpellInfo.h"

/*######
## npc_yenniku
######*/

class npc_yenniku : public CreatureScript
{
public:
    npc_yenniku()
        : CreatureScript("npc_yenniku")
    {
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_yennikuAI(creature);
    }

    struct npc_yennikuAI : public ScriptedAI
    {
        npc_yennikuAI(Creature* creature)
            : ScriptedAI(creature)
        {
            bReset = false;
        }

        uint32 Reset_Timer;
        bool bReset;

        void Reset() override
        {
            Reset_Timer = 0;
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (bReset || spell->Id != 3607)
                return;

            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestStatus(592) == QUEST_STATUS_INCOMPLETE) //Yenniku's Release
                {
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);
                    me->CombatStop();       //stop combat
                    me->DeleteThreatList(); //unsure of this
                    me->setFaction(83);     //horde generic

                    bReset      = true;
                    Reset_Timer = 60000;
                }
            }
        }

        void EnterCombat(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            if (bReset)
            {
                if (Reset_Timer <= diff)
                {
                    EnterEvadeMode();
                    bReset = false;
                    me->setFaction(28); //troll, bloodscalp
                    return;
                }

                Reset_Timer -= diff;

                if (me->IsInCombat() && me->GetVictim())
                {
                    if (Player* player = me->GetVictim()->ToPlayer())
                    {
                        if (player->GetTeam(CF_DENY) == TEAM_HORDE)
                        {
                            me->CombatStop();
                            me->DeleteThreatList();
                        }
                    }
                }
            }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

enum Thysta
{
    NPC_GRYPHON  = 9526,
    NPC_WYVERN   = 9297,
    SAY_THYSTA_0 = 0
};

struct npc_thysta : public ScriptedAI
{
    npc_thysta(Creature* creature)
        : ScriptedAI(creature)
        , summons(me)
    {
    }

    void EnterCombat(Unit* who) override
    {
        if (Player* pl = who->ToPlayer())
        {
            uint32 creatureEntry = pl->GetTeam() == TEAM_ALLIANCE ? NPC_WYVERN : NPC_GRYPHON;
            if (Creature* spawn = me->SummonCreature(creatureEntry, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 30000))
            {
                if (spawn->IsAIEnabled)
                    spawn->AI()->AttackStart(who);
            }
        }
        Talk(SAY_THYSTA_0);
    }

    void Reset() override
    {
        summons.DespawnAll();
        ScriptedAI::Reset();
    }

    void JustSummoned(Creature* creature) override
    {
        summons.Summon(creature);
        ScriptedAI::JustSummoned(creature);
    }

private:
    SummonList summons;
};

/*######
##
######*/

void AddSC_stranglethorn_vale()
{
    new npc_yenniku();
    new CreatureAILoader<npc_thysta>("npc_thysta");
}
