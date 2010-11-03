/*
 * Copyright (c) Tver Regional Scientific Library
 * Author: Alexander Fronkin
 *
 * Version 2.0 (1 Jan 2003)
 */

#ifndef MARCREC_H
#define MARCREC_H

#include <string>
#include <list>

// -----------------
// MARC record class
// -----------------
class CMarcRecord {
public:
    // Enum of MARC record types
    typedef enum {
        UNIMARC = 1,
        MARC21 = 2
    } TRecordType;

    // Enum of error codes
    typedef enum {
        SUCCESS = 0,
        ERROR = -1
    } TErrorCode;

    #pragma pack(1)

    // Structure of record label
    struct TRecordLabel {
        char aRecLength[5];         // [ 0] Record length
        char cRecStatus;            // [ 5] Record status
        char cRecType;              // [ 6] Type of record
        char cBibliographicalLevel; // [ 7] Bibliographical level
        char cHierarchicalLevel;    // [ 8] Hierarchical level code
        char cUndefined1;           // [ 9] Undefined = '#'
        char cIndicatorLength;      // [10] Indicator length = '2'
        char cSubfieldIdLength;     // [11] Subfield identifier length = '2'
        char aBaseAddress[5];       // [12] Base address of data
        char cEncodingLevel;        // [17] Encoding level
        char cCataloguingForm;      // [18] Descriptive cataloguing form
        char cUndefined2;           // [19] Undefined = '#'
        char cLenFieldLength;       // [20] Length of 'Length of field' = '4'
        char cStartPosLength;       // [21] Length of 'Starting character position' = '5'
        char cImplDefLength;        // [22] Length of implementationdefined portion = '0'
        char cUndefined3;           // [23] Undefined = '#'
    };

    // Structure of record directory entry
    struct TRecordDirEntry {
        char aTag[3];       // Field tag
        char aLength[4];    // Field length
        char aStartPos[5];  // Field starting position
    };

    #pragma pack()

    struct TSubfield;

    // List of subfields
    typedef std::list<TSubfield> TSubfieldList;
    typedef TSubfieldList::iterator TSubfieldRef;
    // List of subfields references
    typedef std::list<TSubfieldRef> TSubfieldPtrList;
    typedef TSubfieldPtrList::iterator TSubfieldPtrRef;

    // Structure of MARC field
    struct TField {
        int iTag;                   // Field tag
        char cInd1;                 // Indicator 1
        char cInd2;                 // Indicator 2
        std::string strData;        // Data of control field
        TSubfieldList SubfieldList; // List of regular subfields

        void Clear()
        {
            iTag = 0;
            cInd1 = cInd2 = ' ';
            strData.erase();
            SubfieldList.clear();
        }
    };

    // List of fields
    typedef std::list<TField> TFieldList;
    typedef TFieldList::iterator TFieldRef;
    // List of fields references
    typedef std::list<TFieldRef> TFieldPtrList;
    typedef TFieldPtrList::iterator TFieldPtrRef;

    // Structure of MARC subfield
    struct TSubfield {
        char cId;               // Subfield identifier
        std::string strData;    // Subfield data
        TField EmbeddedField;   // Embedded field

        void Clear()
        {
            cId = ' ';
            strData.erase();
            EmbeddedField.Clear();
        }
    };

private:
    // Type of record
    TRecordType iRecordType;

    // Record label
    TRecordLabel Label;
    // List of fields
    TFieldList FieldList;

public:
    TErrorCode iErrorCode;

public:
    // Constructors and destructor
    CMarcRecord();
    CMarcRecord(TRecordType iNewRecordType);
    ~CMarcRecord();

    // Clear record
    void Clear();
    // Set record type
    void SetType(TRecordType iNewRecordType);

    // Read record from file
    bool Read(FILE *File, const char *lpszEncoding = "UTF-8");
    // Write record to file
    bool Write(FILE *File, const char *lpszEncoding = "UTF-8");
    // Parse record from buffer
    bool Parse(const char *pRecordBuf, const char *lpszEncoding = "UTF-8");

    // Get list of fields references from record
    TFieldPtrList GetFieldList(int iFieldTag = 0);
    // Get list of subfields references from field
    TSubfieldPtrList GetSubfieldList(TFieldRef FieldRef, char cSubfieldId = ' ');

    // Format record to string for printing
    std::string ToString();
};

#endif
