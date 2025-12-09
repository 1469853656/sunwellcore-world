#include "PremiumStoreMgr.h"
#include "Chat.h"
#include "DatabaseLog.h"

void PremiumStoreMgr::Update()
{
    if (!sCustomMgr->GetBoolConfig(CONFIG_PREMIUM_STORE_ENABLED))
        return;

    auto const now = std::chrono::steady_clock::now();
    if (now.time_since_epoch() >= _nextUpdateTime.time_since_epoch())
    {
        // Process new orders
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_UNPROCESSED_PREMIUM_STORE_REQUESTS);
        stmt->setUInt32(0, realmID);

        _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([this](PreparedQueryResult result)
        {
            if (!result)
                return;

            auto trans = LoginDatabase.BeginTransaction();

            do
            {   //     0           1           2               3                    4
                //  OrderID,   AccountID,  CharacterID,     OrderType,         OrderValue
                Field const* fields      = result->Fetch();
                uint32 const orderID     = fields[0].GetUInt32();
                uint32 const accountID   = fields[1].GetUInt32();
                uint32 const characterID = fields[2].GetUInt32();
                auto const orderType     = static_cast<OrderType>(fields[3].GetUInt32());
                int32 const orderValue   = fields[4].GetInt32();

                if (!sCustomMgr->GetBoolConfig(CONFIG_PREMIUM_STORE_PROCESS_SERVICE_PACK))
                    if (static_cast<OrderType>(orderType) == OrderType::ServicePack)
                        continue;

                OrderData orderData{ orderID, characterID, accountID, orderType, orderValue };
                ProcessOrder(orderData, trans);
            } while (result->NextRow());

            LoginDatabase.CommitTransaction(trans);
        }));

        _nextUpdateTime = now + Seconds(sCustomMgr->GetIntConfig(CONFIG_PREMIUM_STORE_UPDATE_TIME));
    }

    _queryProcessor.ProcessReadyCallbacks();
}

void PremiumStoreMgr::SendServiceTo(uint32 const characterID, uint32 const accountID, ServiceID const id)
{
    switch (id)
    {
        case ServiceID::CharacterRename:
            SetAtLoginFlagsForPlayer(characterID, AT_LOGIN_RENAME);
            break;
        case ServiceID::CharacterCustomize:
            SetAtLoginFlagsForPlayer(characterID, AT_LOGIN_CUSTOMIZE);
            break;
        case ServiceID::CharacterChangeFaction:
            SetAtLoginFlagsForPlayer(characterID, AT_LOGIN_CHANGE_FACTION);
            break;
        case ServiceID::CharacterChangeRace:
            SetAtLoginFlagsForPlayer(characterID, AT_LOGIN_CHANGE_RACE);
            break;
        case ServiceID::AccountRapidExperience1D:
            SetPremiumServiceForPlayer(accountID, PremiumServices::RapidExperience, "1d", characterID);
            break;
        case ServiceID::AccountRapidExperience3D:
            SetPremiumServiceForPlayer(accountID, PremiumServices::RapidExperience, "3d", characterID);
            break;
        case ServiceID::AccountHealthyRessurrection:
            SetPremiumServiceForPlayer(accountID, PremiumServices::HealthyResurrection, "14d", characterID);
            break;
        case ServiceID::AccountIndestructibleArmor:
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "14d", characterID);
            break;
        case ServiceID::AccountUnlimitedHearthstone:
            SetPremiumServiceForPlayer(accountID, PremiumServices::UnlimitedHearthstone, "14d", characterID);
            break;
        case ServiceID::AccountInstantFlight:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "14d", characterID);
            break;
        case ServiceID::CharacterResetRacialShift:
        {
            if (Player const* player = sWorld->GetPlayerByAccountID(accountID))
                if (WorldSession* session = player->GetSession())
                    ChatHandler(session).SendSysMessage("Your Racial Shift has been reset!");

            sCustomMgr->SetCustomCharacterFlag(characterID, CustomPlayerFlags::UsedRacialShift, false);

        } break;
        case ServiceID::CharacterSetMultispec: 
        {
            uint32 currentFlag     = 0;
            CustomData* customData = sCustomMgr->GetCustomData(characterID);
            if (customData)
                currentFlag = customData->MultipleSpecsFlag;
            else
            {
                if (QueryResult const result = CustomDatabase.PQuery("SELECT multipleSpecs_flag FROM characters_data WHERE guid = %u", characterID))
                {
                    Field const* fields = result->Fetch();
                    currentFlag         = fields[0].GetUInt32();
                }
            }

            if (currentFlag & MULTIPLE_SPECS_FLAG_GROUP_3) // do nothing if highest flag set
                break;

            MultipleSpecsFlags flagToSet;
            if (currentFlag & MULTIPLE_SPECS_FLAG_GROUP_2)
                flagToSet = MULTIPLE_SPECS_FLAG_GROUP_3;
            else if (currentFlag & MULTIPLE_SPECS_FLAG_GROUP_1)
                flagToSet = MULTIPLE_SPECS_FLAG_GROUP_2;
            else
            {
                sPremiumStoreMgr->SendMailWithItemTo(characterID, CustomConstants::MultispecItemEntry);
                flagToSet = MULTIPLE_SPECS_FLAG_GROUP_1;
            }

            if (customData)
            {
                customData->MultipleSpecsFlag |= flagToSet;
                sCustomMgr->SaveCharacterCustomData(characterID, false);
            }
            else
                CustomDatabase.PExecute("UPDATE characters_data SET MultipleSpecsFlag = MultipleSpecsFlag|%u WHERE CharacterID = %u", static_cast<uint32>(flagToSet), characterID);

            break;
        }
        case ServiceID::WhitemanePlus30D:
        case ServiceID::WhitemanePlus90D:
        case ServiceID::WhitemanePlus180D:
            SetWhitemanePlusSubscription(accountID, id);
            break;
        default:
            break;
    }

    // fill log
    DBLog::OutStore("[PREMIUM STORE] Service ID {} Activated for Player GUID: {}", static_cast<uint32>(id), characterID);
}

void PremiumStoreMgr::SendServicePackTo(uint32 const accountID, ServicePacks const servicePack)
{
    switch (servicePack)
    {
        case ServicePacks::Murloc:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "3d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "3d");
            break;
        case ServicePacks::Bandit:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "7d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "7d");
            break;
        case ServicePacks::Dragon:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "14d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "14d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::UnlimitedHearthstone, "7d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::HealthyResurrection, "3d");
            break;
        case ServicePacks::Demon:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "28d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "21d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::UnlimitedHearthstone, "14d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::HealthyResurrection, "7d");
            break;
        case ServicePacks::Elemental:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "365d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "365d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::UnlimitedHearthstone, "365d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::HealthyResurrection, "365d");
            break;
        case ServicePacks::TheLichKing:
            SetPremiumServiceForPlayer(accountID, PremiumServices::InstantFlight, "3650d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::IndestructibleArmor, "3650d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::UnlimitedHearthstone, "3650d");
            SetPremiumServiceForPlayer(accountID, PremiumServices::HealthyResurrection, "3650d");
            break;
        default:
            DBLog::OutStore("[PREMIUM STORE] Wrong ServicePack type passed to PremiumStoreMgr::SendServicePackTo (AccountID: {}, ServicePackID: {})", accountID, to_underlying(servicePack));
            break;
    }
}

bool PremiumStoreMgr::SendMailWithItemTo(uint32 characterID, uint32 itemEntry, uint32 itemCount, std::optional<int32> randomEnchantID, uint32 OrderID)
{
    return sCustomMgr->SendMail(SendMailOptions(characterID).AddItem(itemEntry, itemCount, randomEnchantID).SetIsStoreMail(true).SetDraftTitle(CustomString::PremiumStoreMailTitle), false, OrderID);
}

bool PremiumStoreMgr::SendMailWithGoldTo(uint32 characterID, uint32 goldAmount, uint32 OrderID)
{
    return sCustomMgr->SendMail(SendMailOptions(characterID).AddMoney(goldAmount).SetIsStoreMail(true).SetDraftTitle(CustomString::PremiumStoreMarketTitle), false, OrderID);
}

void PremiumStoreMgr::SetAtLoginFlagsForPlayer(uint32 const characterID, AtLoginFlags const flag)
{
    if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(characterID, 0, HIGHGUID_PLAYER)))
        player->SetAtLoginFlag(flag);
    else
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | %u WHERE guid = %u", flag, characterID);
}

void PremiumStoreMgr::SetPremiumServiceForPlayer(uint32 const accountID, PremiumServices const premiumServiceType, std::string const& duration, std::optional<uint32> const /*characterID ={}*/)
{
    // -> Query check for previous premium service. If there is the result, stack the time of premium service.
    auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PREMIUM_ACCOUNT_BY_ID);
    stmt->setUInt32(0, accountID);
    stmt->setUInt32(1, realmID);
    stmt->setUInt8(2, static_cast<uint8>(premiumServiceType));

    _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([=](PreparedQueryResult timeResult)
    {
        time_t const currentTime = time(nullptr);

        // -> duration of new premium service
        uint32 premiumTime = TimeStringToSecs(duration);

        if (!timeResult)
            premiumTime = currentTime + premiumTime;
        else
            // Stack the time if time left on the previous service.
            premiumTime += (*timeResult)[0].GetUInt32();

        bool boughtMessageDisplayed = false;
        if (Player const* player = sWorld->GetPlayerByAccountID(accountID))
        {
            if (WorldSession* session = player->GetSession())
            {
                if (premiumServiceType == PremiumServices::WhitemanePlus)
                {
                    if (auto const expiredMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusBoughtMessage))
                        ChatHandler(session).SendSysMessage(expiredMessage->data());

                    sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow())
                        .SetIsStoreMail(false).SetDraftTitle("Your Whitemane+ is active!")
                        .SetDraftMessage(CustomString::WhitemanePlusBoughtMailMessage)
                        .SetCreatureID(0)); // TODO: add sally whitemane creature here?

                    boughtMessageDisplayed = true;
                }
                else
                {
                    ChatHandler(session).SendFormattedSysMessage("Service {} has been enabled for {}!", sCustomMgr->GetPremiumServiceName(premiumServiceType).data(), duration);
                    session->SetPremiumService(premiumServiceType, static_cast<time_t>(premiumTime));
                }
            }
        }

        auto stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_REP_PREMIUM_ACCOUNT);
        stmt2->setUInt32(0, accountID);
        stmt2->setUInt32(1, realmID);
        stmt2->setUInt8(2, static_cast<uint8>(premiumServiceType));
        stmt2->setUInt64(3, currentTime);
        stmt2->setUInt64(4, premiumTime);
        stmt2->setBool(5, false);
        stmt2->setBool(6, boughtMessageDisplayed);
        LoginDatabase.Execute(stmt2);
    }));
}

void PremiumStoreMgr::SetWhitemanePlusSubscription(uint32 accountID, ServiceID serviceID)
{
    std::string duration;
    switch (serviceID)
    {
        case ServiceID::WhitemanePlus30D:
            duration = "30d";
            break;
        case ServiceID::WhitemanePlus90D:
            duration = "90d";
            break;
        case ServiceID::WhitemanePlus180D:
            duration = "180d";
            break;
        default:
            break;
    }

    if (duration.empty())
        return;

    SetPremiumServiceForPlayer(accountID, PremiumServices::WhitemanePlus, duration);
}

void PremiumStoreMgr::ProcessOrder(OrderData orderData, LoginDatabaseTransaction& trans)
{
    switch (orderData.Type)
    {
        case OrderType::Item:
            sPremiumStoreMgr->SendMailWithItemTo(orderData.CharacterID, orderData.OrderValue, 1, {}, orderData.OrderID);
            break;
        case OrderType::Service:
            sPremiumStoreMgr->SendServiceTo(orderData.CharacterID, orderData.AccountID, static_cast<ServiceID>(orderData.OrderValue));
            break;
        /*case OrderType::ItemSet:
            if (auto const* itemSet = context->GetItemSet(message->OrderValue))
                sPremiumStoreMgr->SendMailTo(message->CharacterID, *itemSet);
            break;*/
        case OrderType::Gold:
            sPremiumStoreMgr->SendMailWithGoldTo(orderData.CharacterID, orderData.OrderValue, orderData.OrderID);
            break;
        case OrderType::ServicePack:
            sPremiumStoreMgr->SendServicePackTo(orderData.AccountID, static_cast<ServicePacks>(orderData.OrderValue));
            break;
        default:
            break;
    }

    auto stmtUpd = LoginDatabase.GetPreparedStatement(LOGIN_UPD_PREMIUM_STORE_REQUEST_TO_PROCESSED);
    stmtUpd->setUInt32(0, orderData.OrderID);
    trans->Append(stmtUpd);
}
