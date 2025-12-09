#ifndef _PREMIUMSTOREMGR_H
#define _PREMIUMSTOREMGR_H

#include "Threading/SyncQueue.hpp"
#include "SharedDefines.h"
#include "CustomMgr.h"
#include "Player.h"

#include <optional>

enum class OrderType
{
    None        = 0,
    Item        = 1,
    Service     = 2,
    ItemSet     = 3,
    Gold        = 4,
    ServicePack = 5
};

enum class ServiceID : uint32
{
    CharacterRename                 = 1,
    CharacterCustomize              = 2,
    CharacterChangeFaction          = 3,
    CharacterChangeRace             = 4,
    AccountRapidExperience1D        = 5,
    AccountRapidExperience3D        = 6,
    AccountHealthyRessurrection     = 7,
    AccountIndestructibleArmor      = 8,
    AccountUnlimitedHearthstone     = 9,
    AccountInstantFlight            = 10,
    CharacterResetRacialShift       = 11,
    CharacterSetMultispec           = 12,
    WhitemanePlus30D                = 13,
    WhitemanePlus90D                = 14,
    WhitemanePlus180D               = 15,

    Max
};

enum class ServicePacks
{
    Murloc,
    Bandit,
    Dragon,
    Demon,
    Elemental,
    TheLichKing
};

using ItemSetsContainer = std::unordered_map<uint32, std::vector<uint32>>;

struct OrderData
{
    uint32 OrderID = 0;
    uint32 CharacterID = 0;
    uint32 AccountID = 0;
    OrderType Type = OrderType::None;
    int32 OrderValue = 0;
};

struct PremiumStoreMgr : public Sunwell::Singleton<PremiumStoreMgr>
{
    friend class Singleton<PremiumStoreMgr>;

    void Update();
    void SendServiceTo(uint32 characterID, uint32 accountID, ServiceID id);
    void SendServicePackTo(uint32 accountID, ServicePacks servicePack);

    static bool SendMailWithItemTo(uint32 characterID, uint32 itemEntry, uint32 itemCount = 1, std::optional<int32> randomEnchantID = std::nullopt, uint32 OrderID = 0);
    static bool SendMailWithGoldTo(uint32 characterID, uint32 goldAmount, uint32 OrderID = 0);

    static void SetAtLoginFlagsForPlayer(uint32 characterID, AtLoginFlags flag);
    void SetPremiumServiceForPlayer(uint32 accountID, PremiumServices premiumServiceType, std::string const& duration, std::optional<uint32> characterID = {});
    void SetWhitemanePlusSubscription(uint32 accountID, ServiceID serviceID);

    static void ProcessOrder(OrderData orderData, LoginDatabaseTransaction& trans);

    private:
        std::chrono::steady_clock::time_point _nextUpdateTime = {};
        QueryCallbackProcessor _queryProcessor;
};

#define sPremiumStoreMgr PremiumStoreMgr::instance()


#endif
