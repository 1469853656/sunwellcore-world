

#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Opcodes.h"
#include "Player.h"

//This send to player windows for invite player to join the war
//Param1:(BattleId) the BattleId of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(time) Time in second that the player have for accept
void WorldSession::SendBfInvitePlayerToWar(uint32 BattleId, uint32 ZoneId, uint32 p_time)
{
    //Send packet
    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 12);
    data << uint32(BattleId);
    data << uint32(ZoneId);
    data << uint32((time(nullptr) + p_time));

    //Sending the packet to player
    SendPacket(&data);
}

//This send invitation to player to join the queue
//Param1:(BattleId) the BattleId of Bf
void WorldSession::SendBfInvitePlayerToQueue(uint32 BattleId)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 5);

    data << uint32(BattleId);
    data << uint8(1); //warmup ? used ?

    //Sending packet to player
    SendPacket(&data);
}

//This send packet for inform player that he join queue
//Param1:(BattleId) the BattleId of Bf
//Param2:(ZoneId) the zone where the battle is (4197 for wg)
//Param3:(CanQueue) if able to queue
//Param4:(Full) on log in is full
void WorldSession::SendBfQueueInviteResponse(uint32 BattleId, uint32 ZoneId, bool CanQueue, bool Full)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, 11);
    data << uint32(BattleId);
    data << uint32(ZoneId);
    data << uint8((CanQueue ? 1 : 0)); //Accepted          //0 you cannot queue wg     //1 you are queued
    data << uint8((Full ? 0 : 1));     //Logging In        //0 wg full                 //1 queue for upcoming
    data << uint8(1);                  //Warmup
    SendPacket(&data);
}

//This is call when player accept to join war
//Param1:(BattleId) the BattleId of Bf
void WorldSession::SendBfEntered(uint32 BattleId)
{
    //    m_PlayerInWar[player->GetTeam()].insert(player->GetGUID());
    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERED, 7);
    data << uint32(BattleId);
    data << uint8(1);                        //unk
    data << uint8(1);                        //unk
    data << uint8(_player->isAFK() ? 1 : 0); //Clear AFK
    SendPacket(&data);
}

void WorldSession::SendBfLeaveMessage(uint32 BattleId, BFLeaveReason reason)
{
    WorldPacket data(SMSG_BATTLEFIELD_MGR_EJECTED, 7);
    data << uint32(BattleId);
    data << uint8(reason); //byte Reason
    data << uint8(2);      //byte BattleStatus
    data << uint8(0);      //bool Relocated
    SendPacket(&data);
}

//Send by client when he click on accept for queue
void WorldSession::HandleBfQueueInviteResponse(WorldPacket& recvData)
{
    uint32 battleId = 0u;
    recvData >> battleId;

    uint8 accepted = 0u;
    recvData >> accepted;

    if (accepted == 0)
        return;

    Battlefield* battlefield = sBattlefieldMgr->GetBattlefieldByBattleId(battleId);
    if (battlefield == nullptr)
        return;

    battlefield->QueueEvent<BattlefieldAcceptInvite>(_player->GetGUID(), BattlefieldAcceptInvite::Queue);
}

//Send by client on clicking in accept or refuse of invitation windows for join game
void WorldSession::HandleBfEntryInviteResponse(WorldPacket& recvData)
{
    uint32 battleId = 0u;
    recvData >> battleId;

    uint8 accepted = 0u;
    recvData >> accepted;

    Battlefield* battlefield = sBattlefieldMgr->GetBattlefieldByBattleId(battleId);
    if (battlefield == nullptr)
        return;

    //If player accept invitation
    if (accepted)
        battlefield->QueueEvent<BattlefieldAcceptInvite>(_player->GetGUID(), BattlefieldAcceptInvite::War);
    else
        battlefield->QueueEvent<BattlefieldRemovePlayer>(_player->GetGUID());
}

void WorldSession::HandleBfExitRequest(WorldPacket& recvData)
{
    uint32 battleId = 0;
    recvData >> battleId;

    //sLog->outError("HandleBfExitRequest: BattleID:%u ", BattleId);
    Battlefield* battlefield = sBattlefieldMgr->GetBattlefieldByBattleId(battleId);
    if (battlefield == nullptr)
        return;

    battlefield->QueueEvent<BattlefieldPlayerLeaveRequest>(_player->GetGUID());
}
