

#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include "AuctionHouseMgr.h"
#include "Log.h"
#include "Language.h"
#include "Opcodes.h"
#include "UpdateMask.h"
#include "Util.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "AsyncAuctionListing.h"
#include "fmt/format.h"
#include "Optional.h"

enum LogAuctionHouseAction
{
    LOG_AH_CREATE = 1,
    LOG_AH_BID    = 2,
    LOG_AH_BUYOUT = 3,
    LOG_AH_CANCEL = 4,
};

//void called when player click on auctioneer npc
void WorldSession::HandleAuctionHelloOpcode(WorldPacket& recvData)
{
    uint64 guid; //NPC guid
    recvData >> guid;

#ifndef DISABLE_DRESSNPCS_CORESOUNDS
    bool IsAnyTypeCreature = IS_CREATURE_GUID(guid) || IS_PET_GUID(guid) || IS_VEHICLE_GUID(guid);
    if (IsAnyTypeCreature)
        if (Creature* creature = _player->GetMap()->GetCreature(guid))
            creature->SendMirrorSound(_player, 0);
#endif

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!unit)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionHelloOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    _player->InvalidateHardcorePath();
    SendAuctionHello(guid, unit);
}

//this void causes that auction window is opened
void WorldSession::SendAuctionHello(uint64 guid, Creature* unit)
{
    if (GetPlayer()->getLevel() < sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ))
    {
        SendNotification(GetTrinityString(LANG_AUCTION_REQ), sWorld->getIntConfig(CONFIG_AUCTION_LEVEL_REQ));
        return;
    }

    if (GetPlayer()->GetSession() && GetPlayer()->GetSession()->GetSecurity() > SEC_PLAYER && AccountMgr::GetSecurity(GetPlayer()->GetSession()->GetAccountId()) < 10)
        return;

    if (GetPlayer()->IsPathOfWar())
    {
        ChatHandler(GetPlayer()->GetSession()).SendSysMessage("This action is blocked for Path of War characters.");
        return;
    }

    AuctionHouseEntry const* ahEntry = AuctionHouseMgr::GetAuctionHouseEntry(unit);
    if (!ahEntry)
        return;

    bool const isHardcoreAuctionHouse = unit->GetEntry() == CustomConstants::HardcoreAuctionHouseEntry;
    if (GetPlayer()->HasHardcoreEnabled() && !isHardcoreAuctionHouse)
    {
        if (std::string const* message = sCustomMgr->GetCustomString(CustomString::HardcoreModeCantUseThisAHInHardcore))
            ChatHandler(this).SendSysMessage(message->c_str());

        return;
    }

    if (!GetPlayer()->HasHardcoreEnabled() && isHardcoreAuctionHouse)
    {
        if (std::string const* message = sCustomMgr->GetCustomString(CustomString::HardcoreModeCantUseThisAHInNormal))
            ChatHandler(this).SendSysMessage(message->c_str());

        return;
    }

    WorldPacket data(MSG_AUCTION_HELLO, 12);
    data << uint64(guid);
    data << uint32(ahEntry->houseId);
    data << uint8(1); // 3.3.3: 1 - AH enabled, 0 - AH disabled
    SendPacket(&data);
}

//call this method when player bids, creates, or deletes auction
void WorldSession::SendAuctionCommandResult(uint32 auctionId, uint32 Action, uint32 ErrorCode, uint32 bidError)
{
    WorldPacket data(SMSG_AUCTION_COMMAND_RESULT, 16);
    data << auctionId;
    data << Action;
    data << ErrorCode;
    if (!ErrorCode && Action)
        data << bidError; //when bid, then send 0, once...
    SendPacket(&data);
}

//this function sends notification, if bidder is online
void WorldSession::SendAuctionBidderNotification(uint32 location, uint32 auctionId, uint64 bidder, uint32 bidSum, uint32 diff, uint32 item_template)
{
    WorldPacket data(SMSG_AUCTION_BIDDER_NOTIFICATION, (8 * 4));
    data << uint32(location);
    data << uint32(auctionId);
    data << uint64(bidder);
    data << uint32(bidSum);
    data << uint32(diff);
    data << uint32(item_template);
    data << uint32(0);
    SendPacket(&data);
}

//this void causes on client to display: "Your auction sold"
void WorldSession::SendAuctionOwnerNotification(AuctionEntry* auction)
{
    WorldPacket data(SMSG_AUCTION_OWNER_NOTIFICATION, (8 * 4));
    data << uint32(auction->Id);
    data << uint32(auction->bid);
    data << uint32(0); //unk
    data << uint64(0); //unk (bidder guid?)
    data << uint32(auction->item_template);
    data << uint32(0); //unk
    data << float(0);  //unk (time?)
    SendPacket(&data);
}

//this void creates new auction and adds auction to some auctionhouse
void WorldSession::HandleAuctionSellItem(WorldPacket& recvData)
{
    uint64 auctioneer = 0u;
    recvData >> auctioneer;

    uint32 itemsCount = 0u;
    recvData >> itemsCount;

    auto SendDatabaseError = [&recvData, this] {
        SendAuctionCommandResult(0, AUCTION_SELL_ITEM, ERR_AUCTION_DATABASE_ERROR);
        recvData.rfinish();
    };

    if (itemsCount > MAX_AUCTION_ITEMS)
        return SendDatabaseError();

    if (isLogingOut())
    {
        sLog->outError("Player %s (guid: %u, account: %u) is trying to create auction while logging out.", GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow(), GetAccountId());
        return SendDatabaseError();
    }

    struct ItemData
    {
        uint64 guid;
        uint32 count;
        ItemRef item;
    };

    std::vector<ItemData> items;
    items.resize(itemsCount);

    for (uint32 i = 0; i < itemsCount; ++i)
    {
        recvData >> items[i].guid;
        recvData >> items[i].count;
    }

    uint32 bid = 0u;
    recvData >> bid;

    uint32 buyout = 0u;
    recvData >> buyout;

    uint32 etime = 0u;
    recvData >> etime;

    if (!bid || !etime)
        return;

    if (bid > MAX_MONEY_AMOUNT || buyout > MAX_MONEY_AMOUNT)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionSellItem - Player %s (GUID %u) attempted to sell item with higher price than max gold amount.", _player->GetName().c_str(), _player->GetGUIDLow());
        return;
    }

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(auctioneer, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionSellItem - Unit (GUID: %u) not found or you can't interact with him. Player interacting (GUIDLow: %u).", GUID_LOPART(auctioneer), GetPlayer()->GetGUIDLow());
        return;
    }

    //! We will get neutral auction house back if sWorld->getBoolConfig(CONFIG_ALLOW_TWOSIDE_AUCTIONHOUSE) is enabled
    AuctionHouseEntry const* auctionHouseEntry = AuctionHouseMgr::GetAuctionHouseEntry(creature);
    if (!auctionHouseEntry)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionSellItem - Unit (GUID: %u) has wrong faction. Player interacting (GUIDLow: %u).", GUID_LOPART(auctioneer), GetPlayer()->GetGUIDLow());
        return;
    }

    etime *= MINUTE;

    switch (etime)
    {
        case 1 * MIN_AUCTION_TIME:
        case 2 * MIN_AUCTION_TIME:
        case 4 * MIN_AUCTION_TIME:
            break;
        default:
            return;
    }

    std::set<uint64> guids;

    std::map<uint32, size_t> itemEntries;
    for (ItemData& data : items)
    {
        ItemRef item = _player->GetItemByGuid(data.guid);
        if (!item)
            return SendDatabaseError();

        //! Item guid already present!
        if (!guids.insert(data.guid).second)
            return SendDatabaseError();

        if (!item->CanBeAuctioned())
            return SendDatabaseError();

        //! There is already auction with that item
        if (sAuctionMgr->GetAItem(item->GetGUIDLow()))
            return SendDatabaseError();

        if (item->GetCount() < data.count)
            return SendDatabaseError();

        if (!data.count)
            return SendDatabaseError();

        uint32 itemId = item->GetTemplate()->ItemId;
        itemEntries[itemId] += data.count;

        data.item = item;
    }

    if (itemEntries.size() != 1)
        return SendDatabaseError();

    size_t auctionedCount    = itemEntries.begin()->second;
    const bool amountIsValid = std::all_of(items.begin(), items.end(), [=](const ItemData& data) {
        return data.item->GetMaxStackCount() >= auctionedCount;
    });

    if (!amountIsValid)
        return SendDatabaseError();

    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    const ItemRef& sourceItem = items.front().item;

    //! Take deposit
    uint32 deposit = sAuctionMgr->GetAuctionDeposit(auctionHouseEntry, etime, sourceItem, auctionedCount);
    if (!_player->HasEnoughMoney(deposit))
        return SendDatabaseError();

    auto CreateAuctionEntry = [=](const ItemRef& item) {
        auto auctionEntry               = std::make_unique<AuctionEntry>();
        auctionEntry->Id                = sObjectMgr->GenerateAuctionID();
        auctionEntry->auctioneer        = GUID_LOPART(auctioneer);
        auctionEntry->item_guidlow      = item->GetGUIDLow();
        auctionEntry->item_template     = item->GetEntry();
        auctionEntry->itemCount         = item->GetCount();
        auctionEntry->owner             = _player->GetGUIDLow();
        auctionEntry->startbid          = bid;
        auctionEntry->bidder            = 0;
        auctionEntry->bid               = 0;
        auctionEntry->buyout            = buyout;
        auctionEntry->expire_time       = time(nullptr) + uint32(etime * sWorld->getRate(RATE_AUCTION_TIME));
        auctionEntry->deposit           = deposit;
        auctionEntry->auctionHouseEntry = auctionHouseEntry;
        auctionEntry->auctionBirthTime  = time(nullptr);

        return auctionEntry;
    };

    auto GetOrCreateAuctionedItem = [&](CharacterDatabaseTransaction& transaction) -> ItemRef {
        //! We are only auctioned item
        if (items.size() == 1 && sourceItem->GetCount() == auctionedCount)
            return sourceItem;

        auto auctionItem = sourceItem->CloneItem(auctionedCount, _player);
        auctionItem->SaveToDB(transaction);

        return auctionItem;
    };

    auto transaction = CharacterDatabase.BeginTransaction();

    ItemRef auctionItem = GetOrCreateAuctionedItem(transaction);
    if (!auctionItem)
        return SendDatabaseError();

    _player->ModifyMoney(-int32(deposit));
    _player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION, 1);

    for (const ItemData& data : items)
    {
        ItemRef item = data.item;

        if (data.count == item->GetCount())
        {
            //! TODO: possible `auctionItem` memoryu leak if move fails!
            if (!_player->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true, transaction))
                return SendDatabaseError();

            if (auctionItem != item)
                item.Delete();
        }
        else
        {
            //! TODO: possible `auctionItem` memory leak if destroys fails!
            if (!_player->DestroyItemCount(item, data.count, true, transaction))
                return SendDatabaseError();
        }
    }

    //! Its needed here currently, because _SaveInventory internally updates item update queue
    _player->SaveInventoryAndGoldToDB(transaction);

    auto auctionEntry = CreateAuctionEntry(auctionItem);
    auctionEntry->SaveToDB(transaction);

    if (sWorldCache.ShouldBeLoggedInAuctionHouseLog(auctionEntry->item_template))
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOG_AUCTION_HOUSE);
        stmt->setUInt32(0, auctionEntry->Id);
        stmt->setUInt32(1, GetPlayer()->GetGUIDLow());
        stmt->setString(2, GetPlayer()->GetName());
        stmt->setUInt32(3, auctionEntry->item_template);
        stmt->setUInt32(4, auctionEntry->item_guidlow);
        stmt->setUInt32(5, auctionEntry->itemCount);
        stmt->setUInt32(6, LOG_AH_CREATE);
        stmt->setUInt32(7, bid);
        stmt->setUInt32(8, buyout);
        stmt->setString(9, fmt::format("Create auction. Item: Entry: {}, Count {}", auctionEntry->item_template, auctionEntry->itemCount));
        CharacterDatabase.Execute(stmt);
    }

    //! transaction callback must be copyable and can NOT hold unique_ptr, so we wrap it in shared context
    struct Context
    {
        ItemRef auctionItem;
        std::unique_ptr<AuctionEntry> auctionEntry;
    };

    auto context          = std::make_shared<Context>();
    context->auctionEntry = std::move(auctionEntry);
    context->auctionItem  = auctionItem;

    //! TODO: SHOULD CALLBACK BE EXECUTED ON WORLD UPDATE, NOT SESSION?
    AddTransactionCallback(CharacterDatabase.AsyncCommitTransaction(transaction)).AfterComplete([context, faction = creature->getFaction(), this](bool success) mutable
    {
        //! TODO: possible `auctionItem` memory leak if transaction fails!
        if (!success)
            return;

        //! WHAT HAPPENS WHEN PLAYER LOGSOUT BEFORE TRANSACTION IS FINISHED?
        auto auctionEntry   = context->auctionEntry.release();
        auctionEntry->aitem = sAuctionMgr->AddAItem(context->auctionItem);

        AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(faction);
        auctionHouse->AddAuction(auctionEntry);

        SendAuctionCommandResult(auctionEntry->Id, AUCTION_SELL_ITEM, ERR_AUCTION_OK);
    });
}

struct SendOutbiddedMail
{
    uint32 bidder = 0;
    uint32 bid    = 0;
};

struct SendAuctionWonMail
{
    ItemRef item;
};

//this function is called when client bids or buys out auction
void WorldSession::HandleAuctionPlaceBid(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: Received CMSG_AUCTION_PLACE_BID from Player (GUIDLow: %u)", GetPlayer()->GetGUIDLow());

    uint64 auctioneer;
    uint32 auctionId;
    uint32 price;
    recvData >> auctioneer;
    recvData >> auctionId >> price;

    if (!auctionId || !price)
        return; //check for cheaters

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(auctioneer, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionPlaceBid - Unit (GUID: %u) not found or you can't interact with him. Player interacting (GUIDLow: %u).", uint32(GUID_LOPART(auctioneer)), GetPlayer()->GetGUIDLow());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    AuctionEntry* auction = auctionHouse->GetAuction(auctionId);
    Player* player        = GetPlayer();

    if (!auction || auction->owner == player->GetGUIDLow())
    {
        //you cannot bid your own auction:
        SendAuctionCommandResult(0, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
        return;
    }

    // impossible have online own another character (use this for speedup check in case online owner)
    Player* auction_owner = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(auction->owner, 0, HIGHGUID_PLAYER));
    if (!auction_owner && sObjectMgr->GetPlayerAccountIdByGUID(MAKE_NEW_GUID(auction->owner, 0, HIGHGUID_PLAYER)) == GetAccountId())
    {
        //you cannot bid your another character auction:
        SendAuctionCommandResult(0, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
        return;
    }

    // cheating
    if (price <= auction->bid || price < auction->startbid)
        return;

    // price too low for next bid if not buyout
    if ((price < auction->buyout || auction->buyout == 0) && price < auction->bid + auction->GetAuctionOutBid())
        return;

    if (!player->HasEnoughMoney(price))
        return;

    if (sWorldCache.ShouldBeLoggedInAuctionHouseLog(auction->item_template))
    {
        bool bid                = price < auction->buyout || auction->buyout == 0;
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOG_AUCTION_HOUSE);
        stmt->setUInt32(0, auction->Id);
        stmt->setUInt32(1, GetPlayer()->GetGUIDLow());
        stmt->setString(2, GetPlayer()->GetName());
        stmt->setUInt32(3, auction->item_template);
        stmt->setUInt32(4, auction->item_guidlow);
        stmt->setUInt32(5, auction->itemCount);
        stmt->setUInt32(6, bid ? LOG_AH_BID : LOG_AH_BUYOUT);
        stmt->setUInt32(7, bid ? price : 0);
        stmt->setUInt32(8, bid ? 0 : price);
        stmt->setString(9, fmt::format("{} auction. Item: Entry: {}, Count {}", (bid ? "Bid" : "Buyout"), auction->item_template, auction->itemCount));
        CharacterDatabase.Execute(stmt);
    }

    Optional<SendOutbiddedMail> outbidded;
    Optional<SendAuctionWonMail> auctionWon;

    auto transaction = CharacterDatabase.BeginTransaction();

    const uint32 playerBid = price < auction->buyout || auction->buyout == 0 ? price : auction->buyout;
    if (auction->bidder == player->GetGUIDLow())
    {
        player->ModifyMoney(-int32(playerBid - auction->bid));
    }
    else
    {
        if (auction->bidder != 0)
            outbidded = SendOutbiddedMail{ auction->bidder, auction->bid };

        player->ModifyMoney(-int32(playerBid));
    }

    player->SaveGoldToDB(transaction);

    auction->bidder = player->GetGUIDLow();
    auction->bid    = playerBid;

    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID, playerBid);

    AuctionEntry auctionEntry = *auction;

    if (auction->buyout != 0 && playerBid >= auction->buyout)
    {
        auctionWon = SendAuctionWonMail{ auction->aitem->GetItem() };

        auction->DeleteFromDB(transaction);

        sAuctionMgr->RemoveAItem(auction);
        auctionHouse->RemoveAuction(auction);
    }
    else
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_AUCTION_BID);
        stmt->setUInt32(0, auction->bidder);
        stmt->setUInt32(1, auction->bid);
        stmt->setUInt32(2, auction->Id);
        transaction->Append(stmt);
    }

    AddTransactionCallback(CharacterDatabase.AsyncCommitTransaction(transaction)).AfterComplete([auctionHouse, auctionEntry, outbidded, auctionWon, this](bool success) mutable
    {
        if (!success)
            return;

        if (outbidded || auctionWon)
        {
            auto transaction = CharacterDatabase.BeginTransaction();
            if (outbidded)
                sAuctionMgr->SendAuctionOutbiddedMail(&auctionEntry, outbidded->bidder, outbidded->bid, GetPlayer(), transaction);

            if (auctionWon)
            {
                sAuctionMgr->SendAuctionSuccessfulMail(&auctionEntry, transaction);
                sAuctionMgr->SendAuctionWonMail(&auctionEntry, auctionWon->item, transaction);
            }

            CharacterDatabase.CommitTransaction(transaction);
        }

        SendAuctionCommandResult(auctionEntry.Id, AUCTION_PLACE_BID, ERR_AUCTION_OK, 0);
    });
}

//this void is called when auction_owner cancels his auction
void WorldSession::HandleAuctionRemoveItem(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: Received CMSG_AUCTION_REMOVE_ITEM. Player interacting (GUIDLow: %u).", GetPlayer()->GetGUIDLow());

    uint64 auctioneer;
    recvData >> auctioneer;

    uint32 auctionId;
    recvData >> auctionId;

    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "Cancel AUCTION AuctionID: %u. Called by Player (GUIDLow: %u)", auctionId, GetPlayer()->GetGUIDLow());

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(auctioneer, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionRemoveItem - Unit (GUID: %u) not found or you can't interact with him. Player interacting (GUIDLow: %u).", uint32(GUID_LOPART(auctioneer)), GetPlayer()->GetGUIDLow());
        return;
    }

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    AuctionEntry* auction = auctionHouse->GetAuction(auctionId);
    if (!auction || auction->owner != _player->GetGUIDLow())
    {
        if (auction && auction->owner != _player->GetGUIDLow())
        {
            sLog->outError("CHEATER : %u, he tried to cancel auction (id: %u) of another player!", _player->GetGUIDLow(), auctionId);
        }

        return SendAuctionCommandResult(0, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);
    }

    if (!auction->aitem || !auction->aitem->GetItem())
    {
        sLog->outError("Auction id: %u has non-existed item (item guid : %u)!!!", auction->Id, auction->item_guidlow);
        return SendAuctionCommandResult(0, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);
    }

    auto _timeSinceAuctionBirth = time(nullptr) - auction->auctionBirthTime;
    if (_timeSinceAuctionBirth <= 30)
        return SendAuctionCommandResult(0, AUCTION_CANCEL, ERR_AUCTION_DATABASE_ERROR);

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    auto trans = CharacterDatabase.BeginTransaction();
    if (auction->bidder > 0) // If we have a bidder, we have to send him the money he paid
    {
        uint32 auctionCut = auction->GetAuctionCut();
        if (!_player->HasEnoughMoney(auctionCut)) //player doesn't have enough money, maybe message needed
            return;
        //some auctionBidderNotification would be needed, but don't know that parts..
        sAuctionMgr->SendAuctionCancelledToBidderMail(auction, trans);

        _player->ModifyMoney(-int32(auctionCut));
        _player->SaveGoldToDB(trans);
    }

    //! TODO: Send mail only if transaction is successfull?

    // item will deleted or added to received mail list
    MailDraft(auction->BuildAuctionMailSubject(AUCTION_CANCELED), AuctionEntry::BuildAuctionMailBody(0, 0, auction->buyout, auction->deposit, 0))
        .AddItem(const_cast<AuctionItem*>(auction->aitem)->GetItem().Release())
        .SendMailTo(trans, _player, auction, MAIL_CHECK_MASK_COPIED);

    //! Its needed here currently, because _SaveInventory internally updates item update queue
    _player->_SaveInventory(trans);

    auction->DeleteFromDB(trans);
    CharacterDatabase.CommitTransaction(trans);

    if (sWorldCache.ShouldBeLoggedInAuctionHouseLog(auction->item_template))
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOG_AUCTION_HOUSE);
        stmt->setUInt32(0, auction->Id);
        stmt->setUInt32(1, GetPlayer()->GetGUIDLow());
        stmt->setString(2, GetPlayer()->GetName());
        stmt->setUInt32(3, auction->item_template);
        stmt->setUInt32(4, auction->item_guidlow);
        stmt->setUInt32(5, auction->itemCount);
        stmt->setUInt32(6, LOG_AH_CANCEL);
        stmt->setUInt32(7, 0);
        stmt->setUInt32(8, 0);
        stmt->setString(9, fmt::format("Cancel auction. Item: Entry: {}, Count {}", auction->item_template, auction->itemCount));
        CharacterDatabase.Execute(stmt);
    }

    SendAuctionCommandResult(auction->Id, AUCTION_CANCEL, ERR_AUCTION_OK);

    sAuctionMgr->RemoveAItem(auction);
    auctionHouse->RemoveAuction(auction);
}

//called when player lists his bids
void WorldSession::HandleAuctionListBidderItems(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: Received CMSG_AUCTION_LIST_BIDDER_ITEMS. Player interacting (GUIDLow: %u).", GetPlayer()->GetGUIDLow());

    uint64 guid;           //NPC guid
    uint32 listfrom;       //page of auctions
    uint32 outbiddedCount; //count of outbidded auctions

    recvData >> guid;
    recvData >> listfrom; // not used in fact (this list not have page control in client)
    recvData >> outbiddedCount;
    if (recvData.size() != (16 + outbiddedCount * 4))
    {
        sLog->outError("Client sent bad opcode!!! with count: %u and size : %lu (must be: %u)", outbiddedCount, (unsigned long)recvData.size(), (16 + outbiddedCount * 4));
        outbiddedCount = 0;
    }

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionListBidderItems - Unit (GUID: %u) not found or you can't interact with him. Player interacting (GUIDLow: %u).", uint32(GUID_LOPART(guid)), GetPlayer()->GetGUIDLow());
        recvData.rfinish();
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    WorldPacket data(SMSG_AUCTION_BIDDER_LIST_RESULT, (4 + 4 + 4) + 30000); // pussywizard: ensure there is enough memory
    Player* player = GetPlayer();
    data << (uint32)0; //add 0 as count
    uint32 count      = 0;
    uint32 totalcount = 0;
    while (outbiddedCount > 0) //add all data, which client requires
    {
        --outbiddedCount;
        uint32 outbiddedAuctionId;
        recvData >> outbiddedAuctionId;
        AuctionEntry* auction = auctionHouse->GetAuction(outbiddedAuctionId);
        if (auction && auction->BuildAuctionInfo(data))
        {
            ++totalcount;
            ++count;
        }
    }

    auctionHouse->BuildListBidderItems(data, player, count, totalcount);
    data.put<uint32>(0, count); // add count to placeholder
    data << totalcount;
    data << (uint32)300; //unk 2.3.0
    SendPacket(&data);
}

//this void sends player info about his auctions
void WorldSession::HandleAuctionListOwnerItems(WorldPacket& recvData)
{
    // pussywizard:
    const uint32 delay = 4500;
    const uint32 now   = World::GetGameTimeMS();
    if (_lastAuctionListOwnerItemsMSTime > now) // list is pending
        return;
    uint32 diff = getMSTimeDiff(_lastAuctionListOwnerItemsMSTime, now);
    if (diff > delay)
        diff = delay;

    uint64 guid;
    recvData >> guid;

    uint32 listFrom;
    recvData >> listFrom;

    _lastAuctionListOwnerItemsMSTime = now + delay; // set longest possible here, actual exectuing will change this to getMSTime of that moment
    _player->m_Events.AddEvent(new AuctionListOwnerItemsDelayEvent(guid, listFrom, _player->GetGUID(), true), _player->m_Events.CalculateTime(delay - diff));
}

void WorldSession::HandleAuctionListOwnerItemsEvent(uint64 guid, uint32 listfrom)
{
    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: Received CMSG_AUCTION_LIST_OWNER_ITEMS. Player interacting (GUIDLow: %u).", GetPlayer()->GetGUIDLow());

    _lastAuctionListOwnerItemsMSTime = World::GetGameTimeMS(); // pussywizard

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER);
    if (!creature)
    {
        sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: HandleAuctionListOwnerItems - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(creature->getFaction());

    WorldPacket data(SMSG_AUCTION_OWNER_LIST_RESULT, (4 + 4 + 4) + 60000); // pussywizard: ensure there is enough memory
    data << (uint32)0;                                                     // amount place holder

    uint32 count      = 0;
    uint32 totalcount = 0;

    auctionHouse->BuildListOwnerItems(data, _player, count, totalcount);
    data.put<uint32>(0, count);
    data << (uint32)totalcount;
    data << (uint32)0;
    SendPacket(&data);
}

//this void is called when player clicks on search button
void WorldSession::HandleAuctionListItems(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: Received CMSG_AUCTION_LIST_ITEMS. Player interacting (GUIDLow: %u).", GetPlayer()->GetGUIDLow());

    std::string searchedname;
    uint8 levelmin, levelmax, usable;
    uint32 listfrom, auctionSlotID, auctionMainCategory, auctionSubCategory, quality;
    uint64 guid;

    recvData >> guid;
    recvData >> listfrom; // start, used for page control listing by 50 elements
    recvData >> searchedname;

    recvData >> levelmin >> levelmax;
    recvData >> auctionSlotID >> auctionMainCategory >> auctionSubCategory;
    recvData >> quality >> usable;

    //recvData.read_skip<uint8>(); // pussywizard: this is the getAll option
    uint8 getAll;
    recvData >> getAll;

    // this block looks like it uses some lame byte packing or similar...
    uint8 unkCnt;
    recvData >> unkCnt;
    for (uint8 i = 0; i < unkCnt; i++)
    {
        recvData.read_skip<uint8>();
        recvData.read_skip<uint8>();
    }

    // remove fake death
    if (_player->HasUnitState(UNIT_STATE_DIED))
        _player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // pussywizard:
    const uint32 delay = 2000;
    const uint32 now   = World::GetGameTimeMS();
    uint32 diff        = getMSTimeDiff(_lastAuctionListItemsMSTime, now);
    if (diff > delay)
        diff = delay;
    _lastAuctionListItemsMSTime = now + delay - diff;
    std::lock_guard lg(AsyncAuctionListingMgr::GetTempLock());
    AsyncAuctionListingMgr::GetTempList().push_back(AuctionListItemsDelayEvent(delay - diff, _player->GetGUID(), guid, searchedname, listfrom, levelmin, levelmax, usable, auctionSlotID, auctionMainCategory, auctionSubCategory, quality, getAll));
}

void WorldSession::HandleAuctionListPendingSales(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_AUCTIONHOUSE, "WORLD: Received CMSG_AUCTION_LIST_PENDING_SALES. Player interacting (GUIDLow: %u).", GetPlayer()->GetGUIDLow());

    recvData.read_skip<uint64>();

    uint32 count = 0;

    WorldPacket data(SMSG_AUCTION_LIST_PENDING_SALES, 4);
    data << uint32(count); // count
    /*for (uint32 i = 0; i < count; ++i)
    {
        data << "";                                         // string
        data << "";                                         // string
        data << uint32(0);
        data << uint32(0);
        data << float(0);
    }*/
    SendPacket(&data);
}
