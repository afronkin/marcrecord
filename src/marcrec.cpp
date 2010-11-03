/*
 * Copyright (c) Tver Regional Scientific Library
 * Author: Alexander Fronkin
 *
 * Version 2.0 (1 Jan 2003)
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "marcrec.h"

// --------------------------------------
// Print formatted output for std::string
// --------------------------------------
int snprintf(std::string &s, size_t n, const char *format, ...)
{
    va_list ap;
    char *pBuf;
    int iResultCode;

    pBuf = (char *)malloc(n + 1);
    va_start(ap, format);
    iResultCode = vsnprintf(pBuf, n + 1, format, ap);
    va_end(ap);

    if (iResultCode >= 0)
        s.append(pBuf);
    free(pBuf);

    return iResultCode;
}

// -----------
// Constructor
// -----------
CMarcRecord::CMarcRecord()
{
    iRecordType = UNIMARC;
    Clear();
}

CMarcRecord::CMarcRecord(TRecordType iNewRecordType)
{
    SetType(iNewRecordType);
    Clear();
}

// ----------
// Destructor
// ----------
CMarcRecord::~CMarcRecord()
{
}

// ------------
// Clear record
// ------------
void CMarcRecord::Clear()
{
    // Clear field list
    FieldList.clear();

    // Reset record label
    memset(Label.aRecLength, ' ', sizeof(Label.aRecLength));
    Label.cRecStatus = 'n';
    Label.cRecType = 'a';
    Label.cBibliographicalLevel = 'm';
    Label.cHierarchicalLevel = ' ';
    Label.cUndefined1 = ' ';
    Label.cIndicatorLength = '2';
    Label.cSubfieldIdLength = '2';
    memset(Label.aBaseAddress, ' ', sizeof(Label.aBaseAddress));
    Label.cEncodingLevel = ' ';
    Label.cCataloguingForm = ' ';
    Label.cUndefined2 = ' ';
    Label.cLenFieldLength = '4';
    Label.cStartPosLength = '5';
    Label.cImplDefLength = '0';
    Label.cUndefined3 = ' ';
}

// ---------------
// Set record type
// ---------------
void CMarcRecord::SetType(TRecordType iNewRecordType)
{
    iRecordType = iNewRecordType;
}

// ---------------------
// Read record from file
// ---------------------
bool CMarcRecord::Read(FILE *File, const char *lpszEncoding)
{
    int iSymbol;
    char RecordBuf[10000];
    size_t iRecordLen;

    // Skip possible wrong symbols
    do {
        iSymbol = fgetc(File);
    } while (iSymbol >= 0 && isdigit(iSymbol) == 0);

    if (iSymbol < 0)
        return false;

    // Read record length
    RecordBuf[0] = (char)iSymbol;
    if (fread(RecordBuf + 1, 1, 4, File) != 4)
        return false;

    // Parse record length
    if (sscanf(RecordBuf, "%5d", &iRecordLen) != 1)
        return false;

    // Read record
    if (fread(RecordBuf + 5, 1, iRecordLen - 5, File) != iRecordLen - 5)
        return false;

    // Parse record
    return Parse(RecordBuf, lpszEncoding);
}

// --------------------
// Write record to file
// --------------------
bool CMarcRecord::Write(FILE *File, const char *lpszEncoding)
{
    return true;
}

// ------------------------
// Parse record from buffer
// ------------------------
bool CMarcRecord::Parse(const char *pRecordBuf, const char *lpszEncoding)
{
    size_t iBaseAddress, iNumFields;
    TRecordDirEntry *pDirEntry;
    const char *pRecordData, *pFieldData;
    int iFieldNo;
    TField Field;
    TSubfield Subfield;
    size_t iFieldLength, iFieldStartPos;
	int iSymbolPos, iSubfieldStartPos;

    try {
        // Copy record label
        memcpy(&Label, pRecordBuf, sizeof(TRecordLabel));

        // Get base address of data
        if (sscanf(Label.aBaseAddress, "%05d", &iBaseAddress) != 1)
            throw ERROR;

        // Get number of fields
        iNumFields = (iBaseAddress - sizeof(TRecordLabel) - 1) /
            sizeof(TRecordDirEntry);

        // Parse list of fields
        pDirEntry = (TRecordDirEntry *)(pRecordBuf + sizeof(TRecordLabel));
        pRecordData = pRecordBuf + iBaseAddress;
        for (iFieldNo = 0; iFieldNo < iNumFields; iFieldNo++, pDirEntry++) {
            // Clear field
            Field.Clear();

            // Parse directory entry
            if (sscanf((char *)pDirEntry, "%3d%4d%5d",
                &Field.iTag, &iFieldLength, &iFieldStartPos) != 3)
            {
                throw ERROR;
            }

            // Parse field
            pFieldData = pRecordData + iFieldStartPos;
            if(pFieldData[iFieldLength - 1] == '\x1e')
                iFieldLength--;

            if (Field.iTag < 10 || iFieldLength < 2) {
                // Parse control field
                Field.strData.assign(pFieldData, iFieldLength);
            } else {
                // Parse regular field
                Field.cInd1 = pFieldData[0];
                Field.cInd2 = pFieldData[1];

                // Parse list of subfields
                for (iSymbolPos = 2; iSymbolPos <= iFieldLength; iSymbolPos++) {
                    if (pFieldData[iSymbolPos] == '\x1f' ||
                        iSymbolPos == iFieldLength)
                    {
                        if (iSymbolPos > 2) {
                            // Clear subfield
                            Subfield.Clear();
                            // Get subfield identifier
                            Subfield.cId = pFieldData[iSubfieldStartPos + 1];

                            // Check for embedded field
                            if (Subfield.cId == '1' &&
                                iRecordType == UNIMARC &&
                                sscanf(pFieldData + iSubfieldStartPos + 2,
                                    "%3d", &Subfield.EmbeddedField.iTag) == 1)
                            {
                                // Parse embedded field
                                Subfield.EmbeddedField.SubfieldList.clear();
                                if (Subfield.EmbeddedField.iTag < 10) {
                                    Subfield.EmbeddedField.strData.assign(
                                        pFieldData + iSubfieldStartPos + 5,
                                        iSymbolPos - iSubfieldStartPos - 5);
                                } else {
                                    Subfield.EmbeddedField.cInd1 =
                                        pFieldData[iSubfieldStartPos + 5];
                                    Subfield.EmbeddedField.cInd2 =
                                        pFieldData[iSubfieldStartPos + 6];

                                    // Parse subfields of embedded field
                                    // .............
                                }
                            } else {
                                // Parse regular subfield
                                Subfield.strData.assign(
                                    pFieldData + iSubfieldStartPos + 2,
                                    iSymbolPos - iSubfieldStartPos - 2);
                            }

                            // Append subfield to list
                            Field.SubfieldList.push_back(Subfield);
                        }

                        iSubfieldStartPos = iSymbolPos;
                    }
                }
            }

            // Append field to list
            FieldList.push_back(Field);
        }
    } catch (int iErrorCode) {
        if (iErrorCode != 0) {
            Clear();
            return false;
        }
    }

    return true;
}

// ------------------------------
// Get list of fields from record
// ------------------------------
CMarcRecord::TFieldPtrList CMarcRecord::GetFieldList(int iFieldTag)
{
    TFieldPtrList ResultFieldList;
    TFieldRef FieldRef;

    // Check all fields in list
    for (FieldRef = FieldList.begin(); FieldRef != FieldList.end();
        FieldRef++)
    {
        if (iFieldTag == 0 || iFieldTag == FieldRef->iTag)
            ResultFieldList.push_back(FieldRef);
    }

    return ResultFieldList;
}

// --------------------------------
// Get list of subfields from field
// --------------------------------
CMarcRecord::TSubfieldPtrList CMarcRecord::GetSubfieldList(TFieldRef FieldRef,
    char cSubfieldId)
{
    TSubfieldPtrList ResultSubfieldList;
    TSubfieldRef SubfieldRef;

    for (SubfieldRef = FieldRef->SubfieldList.begin();
        SubfieldRef != FieldRef->SubfieldList.end(); SubfieldRef++)
    {
        if (cSubfieldId == ' ' || SubfieldRef->cId == cSubfieldId)
            ResultSubfieldList.push_back(SubfieldRef);
    }

    return ResultSubfieldList;
}

// ------------------------------------
// Format record to string for printing
// ------------------------------------
std::string CMarcRecord::ToString()
{
    std::string strTextRecord = "";
    CMarcRecord::TFieldRef FieldRef;
    CMarcRecord::TSubfieldRef SubfieldRef;

    for (FieldRef = FieldList.begin(); FieldRef != FieldList.end();
        FieldRef++)
    {
        snprintf(strTextRecord, 3, "%03d", FieldRef->iTag);

        if (FieldRef->iTag < 10) {
            strTextRecord += " ";
            strTextRecord += FieldRef->strData.c_str();
        } else {
            snprintf(strTextRecord, 5, " [%c%c]",
                FieldRef->cInd1, FieldRef->cInd2);

            for (SubfieldRef = FieldRef->SubfieldList.begin();
                SubfieldRef != FieldRef->SubfieldList.end(); SubfieldRef++)
            {
                snprintf(strTextRecord, 4, " $%c ", SubfieldRef->cId);
                strTextRecord += SubfieldRef->strData;
            }
        }

        strTextRecord += "\n";
    }

    return strTextRecord;
}
