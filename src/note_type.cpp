// Copyright (c) 2022 QEDIT Systems
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include "note_type.h"

#include <string.h>

//static const unsigned
//unsigned char znt[32] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
unsigned char zz = '\0';

NoteType::NoteType() { // default constructor TODO: Want to make the default constructor setting the note type to ZEC?
    for (int i = 0; i < ZSA_NOTE_SIZE; i++) {
        type_id[i] = zz;
    }
//    std::move(std::begin(znt), std::end(znt), type_id.begin());
//    strcpy(type_id, znt);
}

NoteType::NoteType(unsigned char tid[]) {
    for (int i = 0; i < ZSA_NOTE_SIZE; i++) {
        type_id[i] = tid[i];
    }
}

//NoteType::NoteType(unsigned char* tid) {
//    type_id = *tid;
//}

//NoteType::NoteType(char tid[]) {
//    strcpy(type_id, reinterpret_cast<unsigned char[]> (tid));

//void NoteType::set_type_id(unsigned char *tid) {
//    std::move(std::begin(tid), std::end(tid), type_id.begin());
//}

//void NoteType::set_type_id(unsigned char *tid) {
//    std::move(std::begin(tid), std::end(tid), type_id.begin());//}
