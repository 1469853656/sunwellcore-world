#ifndef _LOGDATABASE_H
#define _LOGDATABASE_H

#include "MySQLConnection.h"

enum LogDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    LOG_INS_MISC_LOGS,
    LOG_INS_CHEAT_LOGS,
    LOG_INS_STORE_LOGS,
    LOG_INS_REPORT_LOGS,
    LOG_INS_WHISPER_LOGS,
    LOG_REP_MUTE_LOGS,
    LOG_SEL_MUTE_LOGS,
    LOG_INS_GM_LOGS,
    LOG_INS_ENCOUNTER_STATS,
    LOG_INS_ENCOUNTER_PLAYER_STATS,
    LOG_SEL_ENCOUNTER_RECORD,

    MAX_LOGDATABASE_STATEMENTS
};

class LogDatabaseConnection : public MySQLConnection
{
    public:
        using Statements = LogDatabaseStatements;

        //- Constructors for sync and async connections
        LogDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) { }
        LogDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) { }
        ~LogDatabaseConnection() override { };

        //- Loads database type specific prepared statements
        void DoPrepareStatements() override;
};

#endif
