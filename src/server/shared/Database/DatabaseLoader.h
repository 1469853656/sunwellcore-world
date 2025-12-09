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

#ifndef DatabaseLoader_h__
#define DatabaseLoader_h__

#include "Define.h"
#include "EnumFlag.h"

#include <functional>
#include <queue>
#include <stack>
#include <string>

template <class T>
class DatabaseWorkerPool;

enum class DatabaseType : uint16
{
    None                = 0x0000,

    Login               = 0x0001,
    Character           = 0x0002,
    World               = 0x0004,
    Log                 = 0x0008,
    Custom              = 0x0010,
    All                 = Login | Character | World | Custom | Log
};
DEFINE_ENUM_FLAG(DatabaseType);

// A helper class to initiate all database worker pools,
// handles updating, delays preparing of statements and cleans up on failure.
class DatabaseLoader
{
    public:
        DatabaseLoader(std::string const& logger, DatabaseType const defaultUpdateMask);

        // Register a database to the loader (lazy implemented)
        template <class T>
        DatabaseLoader& AddDatabase(DatabaseWorkerPool<T>& pool, std::string const& name);

        // Load all databases
        bool Load();

    private:
        bool OpenDatabases();
        bool PopulateDatabases();
        bool UpdateDatabases();
        bool PrepareStatements();

        using Predicate = std::function<bool()>;
        using Closer = std::function<void()>;

        // Invokes all functions in the given queue and closes the databases on errors.
        // Returns false when there was an error.
        bool Process(std::queue<Predicate>& queue);

        std::string const _logger;
        bool const _autoSetup;
        EnumFlag<DatabaseType> const _updateFlags;

        std::queue<Predicate> _open, _populate, _update, _prepare;
        std::stack<Closer> _close;
};

#endif // DatabaseLoader_h__
