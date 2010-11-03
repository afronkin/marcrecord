/*
 * Copyright (c) Tver Regional Scientific Library
 * Author: Alexander Fronkin
 *
 * Version 2.0 (1 Jan 2003)
 */

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include "marcrec.h"

// -------------
// Main function
// -------------
int main(int argc, char *argv[])
{
    CMarcRecord MarcRecord(CMarcRecord::UNIMARC);
    FILE *MarcFile = NULL;
    CMarcRecord::TFieldPtrList FieldList;
    CMarcRecord::TFieldPtrRef FieldRef;
    CMarcRecord::TSubfieldPtrList SubfieldList;
    CMarcRecord::TSubfieldPtrRef SubfieldRef;

    setlocale(LC_CTYPE, "en_US.UTF-8");

    try {
        // Open records file
        MarcFile = fopen(argv[1], "rb");
        if (MarcFile == NULL) {
            printf("Error: can't open file '%s'\n", argv[1]);
            throw 1;
        }

        // Read record
        if (MarcRecord.Read(MarcFile) != true) {
            printf("Error: can't open file '%s'\n", argv[1]);
            throw 1;
        }

        // Print some subfields
        FieldList = MarcRecord.GetFieldList(200);
        for (FieldRef = FieldList.begin(); FieldRef != FieldList.end();
            FieldRef++)
        {
            SubfieldList = MarcRecord.GetSubfieldList(*FieldRef, 'a');
            if (!SubfieldList.empty()) {
                SubfieldRef = SubfieldList.begin();
                printf("%03d [%c%c] $%c %s\n",
                    (*FieldRef)->iTag, (*FieldRef)->cInd1, (*FieldRef)->cInd2,
                    (*SubfieldRef)->cId, (*SubfieldRef)->strData.c_str());
(*FieldRef)->iTag = 999;
(*SubfieldRef)->strData = "456";
            }
        }

        // Print record
        std::string strTextRecord = MarcRecord.ToString();
        printf("%s\n", strTextRecord.c_str());

        fclose(MarcFile);
    } catch (int iErrorCode) {
        if (MarcFile != NULL)
            fclose(MarcFile);

        return iErrorCode;
    }

    wprintf(L"Done.\n");

    return 0;
}

