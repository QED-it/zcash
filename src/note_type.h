// Copyright (c) 2022 QEDIT
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_NOTE_TYPE_H
#define ZCASH_NOTE_TYPE_H

#include <stdlib.h>
#include <array>
#include <string>

#include "zcash/Zcash.h"
#include "rust/orchard/wallet.h"

class NoteType
{
private:
//    std::array<uint8_t, ZC_ORCHARD_NOTE_TYPE_SIZE> type_id;
    unsigned char type_id[ZC_ORCHARD_NOTE_TYPE_SIZE];

public:
    NoteType();
    NoteType(unsigned char tid[ZC_ORCHARD_NOTE_TYPE_SIZE]);

    const unsigned char* get_type();
    void getNoteType(unsigned char noteType[ZC_ORCHARD_NOTE_TYPE_SIZE]);
    std::string get_type_id_str();
};

#endif //ZCASH_NOTE_TYPE_H
