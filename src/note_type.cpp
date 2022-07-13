// Copyright (c) 2022 QEDIT Systems
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include "note_type.h"
#include <sstream>

NoteType::NoteType() { // default constructor
    unsigned char* note_type_ret = new unsigned char[ZC_ORCHARD_NOTE_TYPE_SIZE];
    zsa_get_native_note_type(note_type_ret);
    for (int i = 0; i < ZC_ORCHARD_NOTE_TYPE_SIZE; i++) {
        type_id[i] = *note_type_ret;
        note_type_ret++;
    }
}

NoteType::NoteType(unsigned char tid[]) {
    for (int i = 0; i < ZC_ORCHARD_NOTE_TYPE_SIZE; i++) {
        type_id[i] = tid[i];
    }
}

void NoteType::getNoteType(unsigned char noteType[ZC_ORCHARD_NOTE_TYPE_SIZE]) {
    for (int i = 0; i < ZC_ORCHARD_NOTE_TYPE_SIZE; i++) {
        noteType[i] = type_id[i];
    }
}

const unsigned char *NoteType::get_type() {
    return &type_id[0];
}

std::string NoteType::get_type_id_str() {
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < ZC_ORCHARD_NOTE_TYPE_SIZE; i++) {
        if ((int)type_id[i] < 16)
            ss << "0";
        ss << (int)type_id[i];
    }
    return ss.str();
}
