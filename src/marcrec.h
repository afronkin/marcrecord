/*
 * Copyright (C) 2011  Alexander Fronkin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* Version: 2.0 (27 Feb 2011) */

#if !defined(MARCREC_H)
#define MARCREC_H

#include <string>
#include <list>

/*
 * MARC record class.
 */
class MarcRecord {
public:
	/* Enum of MARC record types. */
	typedef enum {
		UNIMARC = 1,
		MARC21 = 2
	} RecordType;

	/* Enum of error codes. */
	typedef enum {
		SUCCESS = 0,
		ERROR = -1
	} ErrorCode;

	#pragma pack(1)

	/* Structure of record label. */
	struct RecordLabel {
		char recordLength[5];		// [ 0] Record length.
		char recordStatus;		// [ 5] Record status.
		char recordType;		// [ 6] Type of record.
		char bibliographicalLevel;	// [ 7] Bibliographical level.
		char hierarchicalLevel;		// [ 8] Hierarchical level code.
		char undefined1;		// [ 9] Undefined = '#'.
		char indicatorLength;		// [10] Indicator length = '2'.
		char subfieldIdLength;		// [11] Subfield identifier length = '2'.
		char baseAddress[5];		// [12] Base address of data.
		char encodingLevel;		// [17] Encoding level.
		char cataloguingForm;		// [18] Descriptive cataloguing form.
		char undefined2;		// [19] Undefined = '#'.
		char lengthFieldLength;		// [20] Length of 'Length of field' = '4'.
		char startPosLength;		// [21] Length of 'Starting character position' = '5'.
		char implDefLength;		// [22] Length of implementationdefined portion = '0'.
		char undefined3;		// [23] Undefined = '#'.
	};

	/* Structure of record directory entry. */
	struct RecordDirEntry {
		char tag[3];			// Field tag.
		char length[4];			// Field length.
		char startPos[5];		// Field starting position.
	};

	#pragma pack()

	/* MARC field class. */
	class Field;
	/* MARC subfield class. */
	class Subfield;

	/* List of fields. */
	typedef std::list<Field> FieldList;
	typedef FieldList::iterator FieldRef;
	/* List of fields iterators. */
	typedef std::list<FieldRef> FieldPtrList;
	typedef FieldPtrList::iterator FieldPtrRef;

	/* List of subfields. */
	typedef std::list<Subfield> SubfieldList;
	typedef SubfieldList::iterator SubfieldRef;
	/* List of subfields iterators. */
	typedef std::list<SubfieldRef> SubfieldPtrList;
	typedef SubfieldPtrList::iterator SubfieldPtrRef;

	/* List of embedded fields. */
	typedef std::list<SubfieldPtrList> EmbeddedFieldList;
	typedef EmbeddedFieldList::iterator EmbeddedFieldRef;

private:
	/* Type of record. */
	RecordType recordType;

	/* Record label. */
	RecordLabel label;
	/* List of fields. */
	FieldList fieldList;

public:
	ErrorCode errorCode;

private:
	/* Parse field. */
	inline Field parseField(std::string fieldTag,
		const char *fieldData, unsigned int fieldLength, const char *encoding);

public:
	/* Constructors and destructor. */
	MarcRecord();
	MarcRecord(RecordType newRecordType);
	~MarcRecord();

	/* Clear record. */
	void clear();
	/* Set record type. */
	void setType(RecordType newRecordType);

	/* Read record from file. */
	bool read(FILE *marcFile, const char *encoding = "UTF-8");
	/* Write record to file. */
	bool write(FILE *marcFile, const char *encoding = "UTF-8");
	/* Parse record from buffer. */
	bool parse(const char *recordBuf, const char *encoding = "UTF-8");

	/* Get list of fields. */
	FieldPtrList getFields(std::string fieldTag = "");
	/* Get field. */
	FieldRef getField(std::string fieldTag);

	/* Format record to string for printing. */
	std::string toString();
	/* Format field to string for printing. */
	std::string toString(Field field);

	/* Return null field value. */
	inline FieldRef nullField()
	{
		return fieldList.end();
	}
};

/*
 * MARC field class.
 */
class MarcRecord::Field {
public:
	/* Field tag. */
	std::string tag;
	/* Indicator 1. */
	char ind1;
	/* Indicator 2. */
	char ind2;
	/* Data of control field. */
	std::string data;
	/* List of regular subfields. */
	SubfieldList subfieldList;

public:
	/* Clear field data. */
	void clear();

	/* Get list of subfields. */
	SubfieldPtrList getSubfields(char subfieldId = ' ');
	/* Get subfield. */
	SubfieldRef getSubfield(char subfieldId);

	/* Get list of embedded fields. */
	EmbeddedFieldList getEmbeddedFields(std::string fieldTag = "");
	/* Get embedded field. */
	SubfieldPtrList getEmbeddedField(std::string fieldTag);

	/* Return null subfield value. */
	inline SubfieldRef nullSubfield()
	{
		return subfieldList.end();
	}
};

/*
 * MARC subfield class.
 */
class MarcRecord::Subfield {
public:
	/* Subfield identifier. */
	char id;
	/* Subfield data. */
	std::string data;

public:
	/* Clear subfield data. */
	void clear();

	/* Check presence of embedded field. */
	bool isEmbedded();
	/* Get tag of embedded field. */
	std::string getEmbeddedTag();
	/* Get indicator 1 of embedded field. */
	char getEmbeddedInd1();
	/* Get indicator 2 of embedded field. */
	char getEmbeddedInd2();
	/* Get data of embedded field. */
	std::string getEmbeddedData();
};

#endif // MARCREC_H

