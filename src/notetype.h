// Copyright (c) 2022 QEDIT
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_NOTETYPE_H
#define ZCASH_NOTETYPE_H

#include <stdlib.h>
#include <string>

//static const unsigned char znt[32] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

class NoteType
{
private:
    unsigned char type_id[32];

public:
//    NoteType() : type_id(znt) { }
    NoteType();
    NoteType(unsigned char tid[]);
//    NoteType(unsigned char type_id): type_id(type_id) {}

};

#endif //ZCASH_NOTETYPE_H
