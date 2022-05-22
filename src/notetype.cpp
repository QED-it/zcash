// Copyright (c) 2022 QEDIT Systems
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include "notetype.h"

static const unsigned char znt[32] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

NoteType::NoteType() { // default constructor TODO: Want to make the default constructor setting the note type to ZEC?
    type_id = znt;
}

NoteType::NoteType(unsigned char tid[]) {
    type_id = tid;
}

NoteType::NoteType(unsigned char* tid) {
    type_id = *tid;
}

NoteType::NoteType(char tid[]) {
    type_id = reinterpret_cast<unsigned char[]> (tid);
}
