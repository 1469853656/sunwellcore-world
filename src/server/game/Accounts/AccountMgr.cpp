
#include "AccountMgr.h"
#include "DatabaseEnv.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Util.h"
#include "SRP6.h"
#include "WorldSession.h"

namespace AccountMgr
{
    AccountOpResult CreateAccount(std::string username, std::string password)
    {
        if (Utf8Length(username) > MAX_ACCOUNT_STR)
            return AOR_NAME_TOO_LONG; // username's too long

        if (Utf8Length(password) > MAX_PASS_STR)
            return AOR_PASS_TOO_LONG; // password's too long

        normalizeString(username);
        normalizeString(password);

        if (GetId(username))
            return AOR_NAME_ALREDY_EXIST; // username does already exist

        auto [salt, verifier] = Trinity::Crypto::SRP6::MakeRegistrationData(username, password);

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT);
        stmt->setString(0, username);
        stmt->setBinary(1, salt);
        stmt->setBinary(2, verifier);
        stmt->setString(3, "");
        stmt->setString(4, "");
        LoginDatabase.Query(stmt);

        return AOR_OK; // everything's fine
    }

    AccountOpResult DeleteAccount(uint32 accountId)
    {
        // Check if accounts exists
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BY_ID);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (!result)
            return AOR_NAME_NOT_EXIST;

        // Obtain accounts characters
        auto stmtChar = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARS_BY_ACCOUNT_ID);

        stmtChar->setUInt32(0, accountId);

        result = CharacterDatabase.Query(stmtChar);

        if (result)
        {
            do
            {
                uint32 guidLow = (*result)[0].GetUInt32();
                uint64 guid    = MAKE_NEW_GUID(guidLow, 0, HIGHGUID_PLAYER);

                // Kick if player is online
                if (Player* p = ObjectAccessor::FindPlayer(guid))
                {
                    WorldSession* s = p->GetSession();
                    s->KickPlayer("Delete account"); // mark session to remove at next session list update
                    s->LogoutPlayer(false);          // logout player without waiting next session list update
                }

                Player::DeleteFromDB(guid, accountId, false, false); // no need to update realm characters
            } while (result->NextRow());
        }

        // table realm specific but common for all characters of account for realm
        stmtChar = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TUTORIALS);
        stmtChar->setUInt32(0, accountId);
        CharacterDatabase.Execute(stmtChar);

        stmtChar = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ACCOUNT_DATA);
        stmtChar->setUInt32(0, accountId);
        CharacterDatabase.Execute(stmtChar);

        stmtChar = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_BAN);
        stmtChar->setUInt32(0, accountId);
        CharacterDatabase.Execute(stmtChar);

        auto trans = LoginDatabase.BeginTransaction();

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT);
        stmt->setUInt32(0, accountId);
        trans->Append(stmt);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_ACCESS);
        stmt->setUInt32(0, accountId);
        trans->Append(stmt);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHARACTERS);
        stmt->setUInt32(0, accountId);
        trans->Append(stmt);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_BANNED);
        stmt->setUInt32(0, accountId);
        trans->Append(stmt);

        LoginDatabase.CommitTransaction(trans);

        return AOR_OK;
    }

    AccountOpResult ChangeUsername(uint32 accountId, std::string newUsername, std::string newPassword)
    {
        // Check if accounts exists
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BY_ID);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (!result)
            return AOR_NAME_NOT_EXIST;

        if (Utf8Length(newUsername) > MAX_ACCOUNT_STR)
            return AOR_NAME_TOO_LONG;

        if (Utf8Length(newPassword) > MAX_PASS_STR)
            return AOR_PASS_TOO_LONG;

        normalizeString(newUsername);
        normalizeString(newPassword);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_USERNAME);
        stmt->setString(0, newUsername);
        stmt->setUInt32(1, accountId);
        LoginDatabase.Execute(stmt);

        auto [salt, verifier] = Trinity::Crypto::SRP6::MakeRegistrationData(newUsername, newPassword);
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LOGON);
        stmt->setBinary(0, salt);
        stmt->setBinary(1, verifier);
        stmt->setUInt32(2, accountId);
        LoginDatabase.Execute(stmt);
        return AOR_OK;
    }

    AccountOpResult ChangePassword(uint32 accountId, std::string newPassword)
    {
        std::string username;

        if (!GetName(accountId, username))
            return AOR_NAME_NOT_EXIST; // account doesn't exist

        if (Utf8Length(newPassword) > MAX_ACCOUNT_STR)
            return AOR_PASS_TOO_LONG;

        normalizeString(username);
        normalizeString(newPassword);

        auto [salt, verifier] = Trinity::Crypto::SRP6::MakeRegistrationData(username, newPassword);

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LOGON);
        stmt->setBinary(0, salt);
        stmt->setBinary(1, verifier);
        stmt->setUInt32(2, accountId);

        LoginDatabase.Execute(stmt);

        return AOR_OK;
    }

    bool CheckPassword(uint32 accountId, std::string password)
    {
        std::string username;

        if (!GetName(accountId, username))
            return false;

        normalizeString(username);
        normalizeString(password);

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CHECK_PASSWORD);
        stmt->setUInt32(0, accountId);
        if (PreparedQueryResult result = LoginDatabase.Query(stmt))
        {
            Trinity::Crypto::SRP6::Salt salt = (*result)[0].GetBinary<Trinity::Crypto::SRP6::SALT_LENGTH>();
            Trinity::Crypto::SRP6::Verifier verifier = (*result)[1].GetBinary<Trinity::Crypto::SRP6::VERIFIER_LENGTH>();
            if (Trinity::Crypto::SRP6::CheckLogin(username, password, salt, verifier))
                return true;
        }

        return false;
    }

    uint32 GetId(std::string const& username)
    {
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME);
        stmt->setString(0, username);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        return (result) ? (*result)[0].GetUInt32() : 0;
    }

    uint32 GetSecurity(uint32 accountId)
    {
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ACCESS_GMLEVEL);
        stmt->setUInt32(0, accountId);
        stmt->setInt32(1, int32(realmID));
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        return (result) ? (*result)[0].GetUInt8() : uint32(SEC_PLAYER);
    }

    uint32 GetSecurity(uint32 accountId, int32 realmId)
    {
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_GMLEVEL_BY_REALMID);
        stmt->setUInt32(0, accountId);
        stmt->setInt32(1, realmId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        return (result) ? (*result)[0].GetUInt8() : uint32(SEC_PLAYER);
    }

    bool GetName(uint32 accountId, std::string& name)
    {
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_USERNAME_BY_ID);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (result)
        {
            name = (*result)[0].GetString();
            return true;
        }

        return false;
    }

    std::string const GetName(uint32 accountId)
    {
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_USERNAME_BY_ID);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (result)
            return (*result)[0].GetString();

        return {};
    }

    /*
    bool CheckCountry(uint32 accountId, std::string country, std::string exceptCountry)
    {
        // No country means international message.
        if (country.empty() && exceptCountry.empty())
            return true;

        normalizeString(country);

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_IP2NATION_PLAYER_IP);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (result)
        {
            std::string lastIp = (*result)[0].GetString();
            uint32 ip          = inet_addr(lastIp.c_str());

            EndianConvertReverse(ip);

            auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IP2NATION);
            stmt->setUInt32(0, ip);
            PreparedQueryResult ip_result = LoginDatabase.Query(stmt);

            if (ip_result)
            {
                std::string playerCountry = (*ip_result)[0].GetString();
                normalizeString(playerCountry);

                return country == playerCountry && exceptCountry != playerCountry ? true : false;
            }
        }
        return false;
    }
    */

    uint32 GetCharactersCount(uint32 accountId)
    {
        // check character count
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_SUM_CHARS);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        return (result) ? (*result)[0].GetUInt64() : 0;
    }

    bool normalizeString(std::string& utf8String)
    {
        std::wstring buffer;
        if (!Utf8toWStr(utf8String, buffer))
            return false;

        for (wchar_t& c : buffer)
            c = wcharToUpperOnlyLatin(c);

        return WStrToUtf8(buffer, utf8String);
    }

    bool IsPlayerAccount(uint32 gmlevel)
    {
        return gmlevel == SEC_PLAYER;
    }

    bool IsModeratorAccount(uint32 gmlevel)
    {
        return gmlevel == SEC_MODERATOR;
    }

    bool IsGMAccount(uint32 gmlevel)
    {
        return gmlevel >= SEC_GAMEMASTER && gmlevel <= SEC_CONSOLE;
    }

    bool IsAdminAccount(uint32 gmlevel)
    {
        return gmlevel >= SEC_ADMINISTRATOR && gmlevel <= SEC_CONSOLE;
    }

    bool IsConsoleAccount(uint32 gmlevel)
    {
        return gmlevel == SEC_CONSOLE;
    }

    std::string GetLastIp(uint32 acc_id)
    {
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_LAST_IP_BY_ACCOUNT);
        stmt->setUInt32(0, acc_id);

        auto result = LoginDatabase.Query(stmt);
        if (result)
            return (*result)[0].GetString();

        return "";
    }
} // Namespace AccountMgr
