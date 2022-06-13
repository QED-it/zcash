// Copyright (c) 2022 QEDIT
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_NOTE_TYPE_H
#define ZCASH_NOTE_TYPE_H

#include <stdlib.h>
#include <array>
#include <string>

#include "zcash/Zcash.h"
#include "rust/include/rust/orchard/wallet.h"

class NoteType
{
private:
    std::array<uint8_t, ZSA_NOTE_SIZE> type_id;

public:
    NoteType();
    NoteType(unsigned char tid[ZSA_NOTE_SIZE]);
};

#endif //ZCASH_NOTE_TYPE_H
