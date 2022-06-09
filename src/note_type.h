// Copyright (c) 2022 QEDIT
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_NOTE_TYPE_H
#define ZCASH_NOTE_TYPE_H

#include <stdlib.h>
#include <array>
#include <string>
#include "zcash/Zcash.h"

#include "zcash/Zcash.h"

//static const unsigned char znt[32] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

class NoteType
{
private:
    std::array<uint8_t, ZSA_NOTE_SIZE> type_id;

public:
//    NoteType() : type_id(znt) { }
    NoteType();
    NoteType(unsigned char tid[ZSA_NOTE_SIZE]);
//    NoteType(unsigned char type_id): type_id(type_id) {}
//    void set_type_id(unsigned char tid[32]);
};

#endif //ZCASH_NOTE_TYPE_H
