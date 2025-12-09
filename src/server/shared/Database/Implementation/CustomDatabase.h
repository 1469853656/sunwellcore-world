#ifndef _CUSTOMDATABASE_H
#define _CUSTOMDATABASE_H

#include "MySQLConnection.h"

enum CustomDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    // Custom Data
    CUSTOM_SEL_CHARACTER_DATA,
    CUSTOM_INS_CHARACTER_DATA,
    CUSTOM_UPD_CHARACTER_DATA_RACIAL,
    CUSTOM_DEL_CHARACTER_DATA,
    CUSTOM_REP_CHARACTER_DATA,
    CUSTOM_SET_CHARACTER_DATA_FLAG,
    CUSTOM_UNSET_CHARACTER_DATA_FLAG,

    // Custom String
    CUSTOM_SEL_CUSTOM_STRING,

    // Boost Values
    CUSTOM_SEL_INSTANCE_BOOST_VALUES,

    // XP Rates
    CUSTOM_SEL_XP_RATE_BRACKETS,

    // Premade Sets
    CUSTOM_SEL_PREMADE_SETS,
    CUSTOM_SEL_PREMADE_SET_EQUIPMENT_ITEMS,
    CUSTOM_SEL_PREMADE_SET_BAG_ITEMS,
    CUSTOM_SEL_PREMADE_SET_TALENTS,
    CUSTOM_SEL_PREMADE_SET_GLYPHS,
    CUSTOM_SEL_PRESET_GROUPS,

    // BoostMgr
    CUSTOM_SEL_BOOST_STAT_VALUES,
    CUSTOM_SEL_BOOST_SPELL_VALUES,

    // Transmogification
    CUSTOM_SEL_TRANSMOG_ENCHANT_NAMES,
    CUSTOM_SEL_TRANSMOG_SETS,
    CUSTOM_SEL_TRANSMOG_SET_ITEMS,

    MAX_CUSTOMDATABASE_STATEMENTS
};

class CustomDatabaseConnection : public MySQLConnection
{
    public:
        using Statements = CustomDatabaseStatements;

        //- Constructors for sync and async connections
        CustomDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) { }
        CustomDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) { }
        ~CustomDatabaseConnection() override { }

        //- Loads database type specific prepared statements
        void DoPrepareStatements() override;
};


#endif
