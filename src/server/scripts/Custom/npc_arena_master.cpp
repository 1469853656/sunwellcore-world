#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "CreatureTextMgr.h"
#include "PassiveAI.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "SpellAuras.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Pet.h"
#include "Chat.h"
#include "Group.h"
#include "WaypointManager.h"
#include "SmartAI.h"
#include "DBCStructure.h"
#include "Language.h"
#include "CustomEventMgr.h"
#include "BattlegroundMgr.h"
#include "ArenaTeamMgr.h"
#include "SoloQueue.h"

#define GOSSIP_JOIN_1_VS_1 "Join 1v1 Arena"
#define GOSSIP_JOIN_SOLO_QUEUE "Join Solo Queue"
#define GOSSIP_CUSTOMIZE_SOLO_QUEUE_RETURN "Return to main menu"

struct EmblemStruct
{
    uint32 BorderStyle     = 6;
    uint32 BorderColor     = 4294705149;
    uint32 IconStyle       = 101;
    uint32 IconColor       = 4294242303;
    uint32 BackgroundColor = 0;
};

enum CustomizeMenu
{
    CUSTOMIZE_SOLO_QUEUE_TEAM_BANNER_STYLE = 0,
    //CUSTOMIZE_SOLO_QUEUE_TEAM_ICON_STYLE = 1,
    CUSTOMIZE_SOLO_QUEUE_TEAM_ICON_STYLE_1 = 100,
    CUSTOMIZE_SOLO_QUEUE_TEAM_ICON_STYLE_2 = 200,
    CUSTOMIZE_SOLO_QUEUE_TEAM_ICON_STYLE_3 = 300,
    CUSTOMIZE_SOLO_QUEUE_TEAM_ICON_STYLE_4 = 400
};

enum GossipTextID
{
    GOSSIP_SOLO_QUEUE_TEXT_HELLO_NO_TEAM    = 1200000,
    GOSSIP_SOLO_QUEUE_TEXT_EMBLEM_CUSTOMIZE = 1200001,
    GOSSIP_SOLO_QUEUE_TEXT_EMBLEM_BANNER    = 1200002,
    GOSSIP_SOLO_QUEUE_TEXT_EMBLEM_ICON      = 1200003,
    GOSSIP_SOLO_QUEUE_TEXT_HELLO_TEAM       = 1200004,
    GOSSIP_SOLO_QUEUE_TEXT_HELLO_IN_QUEUE   = 1200005,
    GOSSIP_SOLO_QUEUE_TEXT_80_LVL           = 1200006,
    GOSSIP_SOLO_QUEUE_TEXT_RESI             = 1200007,
};

struct npc_arena_masterAI : public ScriptedAI
{
    npc_arena_masterAI(Creature* creature) : ScriptedAI(creature)
    {
    }

    void sGossipHello(Player* player) override
    {
        if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE))
            return;

        if (player->getLevel() < 80)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Understood", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_SOLO_QUEUE_TEXT_80_LVL, me->GetGUID());
            return;
        }

        // Resilience check
        uint32 reqResilience = sWorld->getIntConfig(CONFIG_SOLOQUEUE_RESILIENCE_REQ);
        if (player->GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1 + CR_CRIT_TAKEN_SPELL) < reqResilience)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Understood", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_SOLO_QUEUE_TEXT_RESI, me->GetGUID());

            ChatHandler(player->GetSession()).PSendSysMessage("You need at least %u resilience to start playing Solo Queue arenas!", reqResilience);
            return;
        }

        if (player->InBattleground())
            return;

        if (player->InBattlegroundQueue())
        {
            player->SEND_GOSSIP_MENU(GOSSIP_SOLO_QUEUE_TEXT_HELLO_IN_QUEUE, me->GetGUID());
            return;
        }

        uint32 textID = 0;
        if (!player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_SOLO_QUEUE))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_JOIN_SOLO_QUEUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            textID = GOSSIP_SOLO_QUEUE_TEXT_HELLO_TEAM;
        }
        else
            textID = GOSSIP_SOLO_QUEUE_TEXT_HELLO_NO_TEAM;

        player->SEND_GOSSIP_MENU(textID, me->GetGUID());
    }

    void sGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        uint32 menuId   = player->PlayerTalkClass->GetGossipOptionAction(action);
        uint32 senderId = player->PlayerTalkClass->GetGossipOptionSender(action);

        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

        uint8 arenatype = 0;
        switch (menuId)
        {
            case GOSSIP_ACTION_INFO_DEF:
                arenatype = ARENA_TYPE_SOLO_QUEUE;
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                return;
            default:
                break;
        }

        WorldPacket data;
        data << uint64(me->GetGUID());
        data << uint8(2);
        data << uint8(0);
        data << uint8(0);
        player->GetSession()->HandleBattlemasterJoinArena(data);
    }
};

void AddSC_npc_arena_master()
{
    new CreatureAILoader<npc_arena_masterAI>("npc_arena_master");
}