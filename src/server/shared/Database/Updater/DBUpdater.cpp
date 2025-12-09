/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DBUpdater.h"
#include "BuiltInConfig.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DatabaseLoader.h"
#include "GitRevision.h"
#include "Log.h"
#include "QueryResult.h"
#include "StartProcess.h"
#include "UpdateFetcher.h"

#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/stream.hpp>

#include "Configuration/Config.h"

std::string DBUpdaterUtil::GetCorrectedMySQLExecutable()
{
    if (!corrected_path().empty())
        return corrected_path();
    else
        return BuiltInConfig::GetMySQLExecutable();
}

bool DBUpdaterUtil::CheckExecutable()
{
    std::filesystem::path exe(GetCorrectedMySQLExecutable());
    if (!is_regular_file(exe))
    {
        exe = Trinity::SearchExecutableInPath("mysql");
        if (!exe.empty() && is_regular_file(exe))
        {
            // Correct the path to the cli
            corrected_path() = absolute(exe).generic_string();
            return true;
        }

        sLog->outCrash("Didn't find any executable MySQL binary at \'%s\' or in path, correct the path in the *.conf (\"MySQLExecutable\").",
            absolute(exe).generic_string().c_str());

        return false;
    }
    return true;
}

std::string& DBUpdaterUtil::corrected_path()
{
    static std::string path;
    return path;
}

// Auth Database
template<>
std::string DBUpdater<LoginDatabaseConnection>::GetConfigEntry()
{
    return "Updates.Auth";
}

template<>
std::string DBUpdater<LoginDatabaseConnection>::GetTableName()
{
    return "Auth";
}

template<>
std::string DBUpdater<LoginDatabaseConnection>::GetBaseFile()
{
    return BuiltInConfig::GetSourceDirectory() +
        "/sql/base/auth_database.sql";
}

template<>
bool DBUpdater<LoginDatabaseConnection>::IsEnabled(EnumFlag<DatabaseType> const updateMask)
{
    return updateMask.HasFlag(DatabaseType::Login);
}

// World Database
template<>
std::string DBUpdater<WorldDatabaseConnection>::GetConfigEntry()
{
    return "Updates.World";
}

template<>
std::string DBUpdater<WorldDatabaseConnection>::GetTableName()
{
    return "World";
}

template<>
std::string DBUpdater<WorldDatabaseConnection>::GetBaseFile()
{
    return (
        std::filesystem::path(BuiltInConfig::GetSourceDirectory()) /
        std::filesystem::path(GitRevision::GetFullDatabase())).string();
}

template<>
bool DBUpdater<WorldDatabaseConnection>::IsEnabled(EnumFlag<DatabaseType> const updateMask)
{
    return updateMask.HasFlag(DatabaseType::World);
}

template<>
BaseLocation DBUpdater<WorldDatabaseConnection>::GetBaseLocationType()
{
    return LOCATION_REPOSITORY_ARCHIVE;
}

// Character Database
template<>
std::string DBUpdater<CharacterDatabaseConnection>::GetConfigEntry()
{
    return "Updates.Character";
}

template<>
std::string DBUpdater<CharacterDatabaseConnection>::GetTableName()
{
    return "Character";
}

template<>
std::string DBUpdater<CharacterDatabaseConnection>::GetBaseFile()
{
    return BuiltInConfig::GetSourceDirectory() +
        "/sql/base/characters_database.sql";
}

template<>
bool DBUpdater<CharacterDatabaseConnection>::IsEnabled(EnumFlag<DatabaseType> const updateMask)
{
    return updateMask.HasFlag(DatabaseType::Character);
}

// Log Database
template<>
std::string DBUpdater<LogDatabaseConnection>::GetConfigEntry()
{
    return "Updates.Log";
}

template<>
std::string DBUpdater<LogDatabaseConnection>::GetTableName()
{
    return "Log";
}

template<>
std::string DBUpdater<LogDatabaseConnection>::GetBaseFile()
{
    return BuiltInConfig::GetSourceDirectory() +
        "/sql/base/log_database.sql";
}

template<>
bool DBUpdater<LogDatabaseConnection>::IsEnabled(EnumFlag<DatabaseType> const updateMask)
{
    return updateMask.HasFlag(DatabaseType::Log);
}

template<>
BaseLocation DBUpdater<LogDatabaseConnection>::GetBaseLocationType()
{
    return LOCATION_REPOSITORY;
}

// Custom Database
template<>
std::string DBUpdater<CustomDatabaseConnection>::GetConfigEntry()
{
    return "Updates.Custom";
}

template<>
std::string DBUpdater<CustomDatabaseConnection>::GetTableName()
{
    return "Custom";
}

template<>
std::string DBUpdater<CustomDatabaseConnection>::GetBaseFile()
{
    return BuiltInConfig::GetSourceDirectory() +
        "/sql/base/custom_database.sql";
}

template<>
bool DBUpdater<CustomDatabaseConnection>::IsEnabled(EnumFlag<DatabaseType> const updateMask)
{
    return updateMask.HasFlag(DatabaseType::Custom);
}

template<>
BaseLocation DBUpdater<CustomDatabaseConnection>::GetBaseLocationType()
{
    return LOCATION_REPOSITORY;
}

// All
template<class T>
BaseLocation DBUpdater<T>::GetBaseLocationType()
{
    return LOCATION_REPOSITORY;
}

template<class T>
bool DBUpdater<T>::Create(DatabaseWorkerPool<T>& pool)
{
    sLog->outMisc("Database \"%s\" does not exist, do you want to create it? [yes (default) / no]: ",
        pool.GetConnectionInfo()->database.c_str());

    std::string answer;
    std::getline(std::cin, answer);
    if (!answer.empty() && !(answer.substr(0, 1) == "y"))
        return false;

    sLog->outMisc("Creating database \"%s\"...", pool.GetConnectionInfo()->database.c_str());

    // Path of temp file
    static Path const temp("create_table.sql");

    // Create temporary query to use external MySQL CLi
    std::ofstream file(temp.generic_string());
    if (!file.is_open())
    {
        sLog->outCrash("Failed to create temporary query file \"%s\"!", temp.generic_string().c_str());
        return false;
    }

    file << "CREATE DATABASE `" << pool.GetConnectionInfo()->database << "` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci\n\n";

    file.close();

    try
    {
        DBUpdater<T>::ApplyFile(pool, pool.GetConnectionInfo()->host, pool.GetConnectionInfo()->user, pool.GetConnectionInfo()->password,
            pool.GetConnectionInfo()->port_or_socket, "", temp, pool.GetConnectionInfo()->login_path);
    }
    catch (UpdateException&)
    {
        sLog->outCrash("Failed to create database %s! Does the user (named in *.conf) have `CREATE` privileges on the MySQL server?", pool.GetConnectionInfo()->database.c_str());
        std::filesystem::remove(temp);
        return false;
    }

    sLog->outMisc("Done.");
    std::filesystem::remove(temp);
    return true;
}

template<class T>
bool DBUpdater<T>::Update(DatabaseWorkerPool<T>& pool)
{
    if (!DBUpdaterUtil::CheckExecutable())
        return false;

    std::string const name = DBUpdater<T>::GetTableName();
    sLog->outMisc("Updating %s database...", name.c_str());

    Path const sourceDirectory(BuiltInConfig::GetSourceDirectory());

    if (!is_directory(sourceDirectory))
    {
        sLog->outError("DBUpdater: Given source directory %s does not exist, skipped!", sourceDirectory.generic_string().c_str());
        return false;
    }

    UpdateFetcher updateFetcher(sourceDirectory, [&](std::string const& query) { DBUpdater<T>::Apply(pool, query); },
        [&](Path const& file) { DBUpdater<T>::ApplyFile(pool, file); },
            [&](std::string const& query) -> QueryResult { return DBUpdater<T>::Retrieve(pool, query); });

    UpdateResult result;
    try
    {
        result = updateFetcher.Update(
            sConfigMgr->GetBoolDefault("Updates.Redundancy", true),
            sConfigMgr->GetBoolDefault("Updates.AllowRehash", true),
            sConfigMgr->GetBoolDefault("Updates.ArchivedRedundancy", false),
            sConfigMgr->GetIntDefault("Updates.CleanDeadRefMaxCount", 3));
    }
    catch (UpdateException&)
    {
        return false;
    }

    std::string const info = fmt::format("Containing {} new and {} archived updates.",
        result.recent, result.archived);

    if (!result.updated)
        sLog->outMisc(">> %s database is up-to-date! %s", name.c_str(), info.c_str());
    else
        sLog->outMisc("%s", fmt::format(">> Applied {} {}. {}", result.updated, result.updated == 1 ? "query" : "queries", info.c_str()).c_str());

    return true;
}

template<class T>
bool DBUpdater<T>::Populate(DatabaseWorkerPool<T>& pool)
{
    {
        QueryResult const result = Retrieve(pool, "SHOW TABLES");
        if (result && (result->GetRowCount() > 0))
            return true;
    }

    if (!DBUpdaterUtil::CheckExecutable())
        return false;

    sLog->outMisc("Database %s is empty, auto populating it...", DBUpdater<T>::GetTableName().c_str());

    std::string const p = DBUpdater<T>::GetBaseFile();
    if (p.empty())
    {
        sLog->outMisc(">> No base file provided, skipped!");
        return true;
    }

    Path const base(p);
    if (!exists(base))
    {
        switch (DBUpdater<T>::GetBaseLocationType())
        {
            case LOCATION_REPOSITORY:
            case LOCATION_REPOSITORY_ARCHIVE:
            {
                sLog->outError(">> Base file \"%s\" is missing, try to clone the source again.",
                    base.generic_string().c_str());

                break;
            }
            case LOCATION_DOWNLOAD:
            {
                std::string const filename = base.filename().generic_string();
                std::string const workdir = std::filesystem::current_path().generic_string();
                sLog->outError(">> File \"%s\" is missing, download it from %placeholder% uncompress it and place the file \"%s\" in the directory \"%s\".", filename.c_str(), filename.c_str(), workdir.c_str());
                break;
            }
        }
        return false;
    }

    Path outputfile;

    if (GetBaseLocationType() == BaseLocation::LOCATION_REPOSITORY_ARCHIVE)
    {
        sLog->outMisc(">> Unpacking \'%s\'...", base.generic_string().c_str());

        std::string baseDeflated = Trinity::StringFormat("%s/unarchived.sql", base.parent_path().generic_string()).c_str();

        std::ifstream dataFile(base.generic_string(), std::ios::binary);

        boost::iostreams::filtering_istream in;
        in.push(boost::iostreams::bzip2_decompressor());
        in.push(dataFile);

        std::ofstream out(baseDeflated);

        std::string line;
        while (std::getline(in, line))
        {
            out << line;
            out << "\n";
        }

        out.close();

        outputfile = Path(baseDeflated);
    }
    else
    {
        outputfile = Path(base);
    }

    // Update database
    sLog->outMisc(">> Applying \'%s\'...", outputfile.generic_string().c_str());

    try
    {
        ApplyFile(pool, outputfile);
    }
    catch (UpdateException&)
    {
        return false;
    }

    if (GetBaseLocationType() == BaseLocation::LOCATION_REPOSITORY_ARCHIVE)
    {
        if (exists(outputfile))
        {
            sLog->outMisc(">> Removing unpacked file \'%s\'...", outputfile.generic_string().c_str());

            std::filesystem::remove(outputfile);
        }
    }

    sLog->outMisc(">> Done!");
    return true;
}

template<class T>
QueryResult DBUpdater<T>::Retrieve(DatabaseWorkerPool<T>& pool, std::string const& query)
{
    return pool.Query(query.c_str());
}

template<class T>
void DBUpdater<T>::Apply(DatabaseWorkerPool<T>& pool, std::string const& query)
{
    pool.DirectExecute(query.c_str());
}

template<class T>
void DBUpdater<T>::ApplyFile(DatabaseWorkerPool<T>& pool, Path const& path)
{
    DBUpdater<T>::ApplyFile(pool, pool.GetConnectionInfo()->host, pool.GetConnectionInfo()->user, pool.GetConnectionInfo()->password,
        pool.GetConnectionInfo()->port_or_socket, pool.GetConnectionInfo()->database, path, pool.GetConnectionInfo()->login_path);
}

template<class T>
void DBUpdater<T>::ApplyFile(DatabaseWorkerPool<T>& pool, std::string const& host, std::string const& user,
    std::string const& password, std::string const& port_or_socket, std::string const& database, Path const& path, std::string const& login_path)
{
    std::vector<std::string> args;
    args.reserve(7);

    // CLI Client connection info
    if (login_path.empty())
    {
        args.emplace_back("-h" + host);
        args.emplace_back("-u" + user);

        if (!password.empty())
            args.emplace_back("-p" + password);
    }
    else
    {
        args.emplace_back("--login-path=" + login_path);
    }

    // Check if we want to connect through ip or socket (Unix only)
#ifdef _WIN32

    if (host == ".")
        args.emplace_back("--protocol=PIPE");
    else
        args.emplace_back("-P" + port_or_socket);

#else

    if (!std::isdigit(port_or_socket[0]))
    {
        // We can't check if host == "." here, because it is named localhost if socket option is enabled
        args.emplace_back("-P0");
        args.emplace_back("--protocol=SOCKET");
        args.emplace_back("-S" + port_or_socket);
    }
    else
        // generic case
        args.emplace_back("-P" + port_or_socket);

#endif

    // Set the default charset to utf8
    args.emplace_back("--default-character-set=utf8");

    // Set max allowed packet to 1 GB
    args.emplace_back("--max-allowed-packet=1GB");

    // Database
    if (!database.empty())
        args.emplace_back(database);

    // Invokes a mysql process which doesn't leak credentials to logs
    int const ret = Trinity::StartProcess(DBUpdaterUtil::GetCorrectedMySQLExecutable(), args,
                                 "sql.updates", path.generic_string(), true);

    if (ret != EXIT_SUCCESS)
    {
        sLog->outCrash("Applying of file \'%s\' to database \'%s\' failed!" \
           " If you are a user, pull the latest revision from the repository. If you are a developer, fix your sql query.",
            path.generic_string().c_str(), pool.GetConnectionInfo()->database.c_str());

        throw UpdateException("update failed");
    }
}

template class DBUpdater<LoginDatabaseConnection>;
template class DBUpdater<WorldDatabaseConnection>;
template class DBUpdater<CharacterDatabaseConnection>;
template class DBUpdater<LogDatabaseConnection>;
template class DBUpdater<CustomDatabaseConnection>;
