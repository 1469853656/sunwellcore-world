
#include "Common.h"
#include "RealmList.h"
#include "Database/DatabaseEnv.h"

RealmList::RealmList()
    : m_UpdateInterval(0)
    , m_NextUpdateTime(time(nullptr))
{
}

// Load the realm list from the database
void RealmList::Initialize(uint32 updateInterval)
{
    m_UpdateInterval = updateInterval;

    // Get the content of the realmlist table in the database
    UpdateRealms(true);
}

void RealmList::UpdateRealm(uint32 id, const std::string& name, const std::string& address, uint16 port, uint8 icon, RealmFlags flag, uint8 timezone, AccountTypes allowedSecurityLevel, float popu, uint32 build)
{
    // Create new if not exist or update existed
    Realm& realm = m_realms[name];

    realm.m_ID                 = id;
    realm.name                 = name;
    realm.icon                 = icon;
    realm.flag                 = flag;
    realm.timezone             = timezone;
    realm.allowedSecurityLevel = allowedSecurityLevel;
    realm.populationLevel      = popu;

    realm.Address = address + ":" + std::to_string(port);
    realm.gamebuild = build;
}

void RealmList::UpdateIfNeed()
{
    // maybe disabled or updated recently
    if (!m_UpdateInterval || m_NextUpdateTime > time(nullptr))
        return;

    m_NextUpdateTime = time(nullptr) + m_UpdateInterval;

    // Clears Realm list
    m_realms.clear();

    // Get the content of the realmlist table in the database
    UpdateRealms();
}

void RealmList::UpdateRealms(bool init)
{
    sLog->outString("Updating Realm List...");

    PreparedStatement* stmt    = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALMLIST);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    // Circle through results and add them to the realm map
    if (result)
    {
        do
        {
            Field* fields               = result->Fetch();
            uint32 realmId              = fields[0].GetUInt32();
            std::string name            = fields[1].GetString();
            std::string externalAddress = fields[2].GetString();
            std::string localAddress    = fields[3].GetString();
            std::string localSubmask    = fields[4].GetString();
            uint16 port                 = fields[5].GetUInt16();
            uint8 icon                  = fields[6].GetUInt8();
            RealmFlags flag             = RealmFlags(fields[7].GetUInt8());
            uint8 timezone              = fields[8].GetUInt8();
            uint8 allowedSecurityLevel  = fields[9].GetUInt8();
            float pop                   = fields[10].GetFloat();
            uint32 build                = fields[11].GetUInt32();


            UpdateRealm(realmId, name, externalAddress, port, icon, flag, timezone, (allowedSecurityLevel <= SEC_ADMINISTRATOR ? AccountTypes(allowedSecurityLevel) : SEC_ADMINISTRATOR), pop, build);

            if (init)
                sLog->outString("Added realm \"%s\" at %s:%u.", name.c_str(), externalAddress.c_str(), port);
        }
        while (result->NextRow());
    }
}
