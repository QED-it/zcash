// Copyright (c) 2022 QEDIT Systems
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include "note_type.h"

NoteType::NoteType() { // default constructor
    unsigned char* note_type_ret = new unsigned char[ZC_ORCHARD_NOTE_TYPE_SIZE];
    if (zsa_get_native_note_type(note_type_ret)) {
        for (int i = 0; i < ZC_ORCHARD_NOTE_TYPE_SIZE; i++) {
            type_id[i] = *note_type_ret;
            note_type_ret++;
        }
    }
}

NoteType::NoteType(unsigned char tid[]) {
    for (int i = 0; i < ZC_ORCHARD_NOTE_TYPE_SIZE; i++) {
        type_id[i] = tid[i];
    }
}
