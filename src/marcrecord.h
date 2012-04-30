/*
 * Copyright (c) 2012, Alexander Fronkin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

/* Version: 2.0 (27 Feb 2011) */

#if !defined(MARCRECORD_H)
#define MARCRECORD_H

#include <list>
#include <string>

/*
 * MARC record class.
 */
class MarcRecord {
public:
	/* Enum of MARC record format variants. */
	typedef enum {
		UNIMARC = 1,
		MARC21 = 2
	} FormatVariant;

	/* Enum of error codes. */
	typedef enum {
		SUCCESS = 0,
		ERROR = -1
	} ErrorCode;

	#pragma pack(push)
	#pragma pack(1)

	/* Structure of MARC record label. */
	struct Label {
		// [ 0] Record length.
		char recordLength[5];
		// [ 5] Record status.
		char recordStatus;
		// [ 6] Type of record.
		char recordType;
		// [ 7] Bibliographic level.
		char bibliographicLevel;
		// [ 8] Hierarchical level code.
		char hierarchicalLevel;
		// [ 9] Undefined = '#'.
		char undefined1;
		// [10] Indicator length = '2'.
		char indicatorLength;
		// [11] Subfield identifier length = '2'.
		char subfieldIdLength;
		// [12] Base address of data.
		char baseAddress[5];
		// [17] Encoding level.
		char encodingLevel;
		// [18] Descriptive cataloguing form.
		char cataloguingForm;
		// [19] Undefined = '#'.
		char undefined2;
		// [20] Length of 'Length of field' = '4'.
		char lengthOfFieldLength;
		// [21] Length of 'Starting character position' = '5'.
		char startingPositionLength;
		// [22] Length of implementationdefined portion = '0'.
		char implementationDefinedLength;
		// [23] Undefined = '#'.
		char undefined3;
	};

	#pragma pack(pop)

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
	/* Variant of record format. */
	FormatVariant formatVariant;

	/* Record label. */
	Label label;
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
	MarcRecord(FormatVariant newFormatVariant);
	~MarcRecord();

	/* Clear record. */
	void clear(void);

	/* Get record format variant. */
	FormatVariant getFormatVariant(void);
	/* Set record format variant. */
	void setFormatVariant(FormatVariant newFormatVariant);

	/* Read record from ISO 2709 file. */
	bool readIso2709(FILE *marcFile, const char *encoding = "UTF-8");
	/* Write record to ISO 2709 file. */
	bool writeIso2709(FILE *marcFile, const char *encoding = "UTF-8");
	/* Parse record from ISO 2709 buffer. */
	bool parseIso2709(const char *recordBuf, const char *encoding = "UTF-8");

	/* Get record label. */
	Label getLabel(void);
	/* Set record label. */
	void setLabel(Label &newLabel);

	/* Get list of fields. */
	FieldPtrList getFields(std::string fieldTag = "");
	/* Get field. */
	FieldRef getField(std::string fieldTag);

	/* Add field to the end of record. */
	FieldRef addField(Field field);
	/* Add field to the record before specified field. */
	FieldRef addFieldBefore(Field field, FieldRef nextFieldRef);

	/* Format record to string for printing. */
	std::string toString(void);
	/* Format field to string for printing. */
	std::string toString(Field field);

	/* Return null field value. */
	inline FieldRef nullField(void)
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
	/* Constructor. */
	Field(std::string newTag = "", char newInd1 = ' ', char newInd2 = ' ');

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

	/* Add subfield to the end of field. */
	SubfieldRef addSubfield(Subfield subfield);
	/* Add subfield to the field before specified subfield. */
	SubfieldRef addSubfieldBefore(Subfield subfield, SubfieldRef nextSubfieldRef);

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
	/* Constructor. */
	Subfield(char newId = ' ', std::string newData = "");

	/* Clear subfield data. */
	void clear(void);

	/* Check presence of embedded field. */
	bool isEmbedded(void);
	/* Get tag of embedded field. */
	std::string getEmbeddedTag(void);
	/* Get indicator 1 of embedded field. */
	char getEmbeddedInd1(void);
	/* Get indicator 2 of embedded field. */
	char getEmbeddedInd2(void);
	/* Get data of embedded field. */
	std::string getEmbeddedData(void);
};

#endif /* MARCRECORD_H */
