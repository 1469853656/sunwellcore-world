/*
REWRITTEN BY XINEF
*/

#ifndef DEF_BLOOD_FURNACE_H
#define DEF_BLOOD_FURNACE_H

enum bloodFurnace
{
    DATA_THE_MAKER                          = 0,
    DATA_BROGGOK                            = 1,
    DATA_KELIDAN                            = 2,
    BF_MAX_ENCOUNTER                        = 3,

    DATA_PRISON_CELL1                       = 20,
    DATA_PRISON_CELL2                       = 21,
    DATA_PRISON_CELL3                       = 22,
    DATA_PRISON_CELL4                       = 23,

    ACTION_ACTIVATE_BROGGOK                 = 30,
    ACTION_PREPARE_BROGGOK                  = 31,
    DATA_MAGTHERIDON_BF
};

enum bloodFurnaceNPC
{
    NPC_THE_MAKER                        = 17381,
    NPC_BROGGOK                          = 17380,
    NPC_KELIDAN                          = 17377,
    NPC_NASCENT_FEL_ORC                  = 17398,
    NPC_CHANNELER                        = 17653,
    NPC_MAGTHERIDON_BF                   = 21174,
    NPC_HELLFIRE_CHANNELER               = 17256,

};

enum BloodFuranceGameObjects
{
    GO_THE_MAKER_DOOR                   = 181811,
    GO_THE_MAKER_EXIT_DOOR              = 181812,

    GO_BLOOD_FURANCE_FINAL_EXIT         = 181766,

    GO_BROGOKK_ENTRANCE_DOOR            = 181822,
    GO_BROGOKK_EXIT_DOOR                = 181819,
    GO_BROGOKK_PRISON_CELL_1            = 181821,
    GO_BROGOKK_PRISON_CELL_2            = 181818,
    GO_BROGOKK_PRISON_CELL_3            = 181820,
    GO_BROGOKK_PRISON_CELL_4            = 181817,
    GO_BROGOKK_LEVER                    = 181982,

    GO_KELIDAN_EXIT_DOOR                = 181823,

    GO_MAKER_PRISON_CELL_1              = 181813,
    GO_MAKER_PRISON_CELL_2              = 181814,
    GO_MAKER_PRISON_CELL_3              = 181815,
    GO_MAKER_PRISON_CELL_4              = 181816
};

#endif

