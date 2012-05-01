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
#include <vector>

#if defined(MARCRECORD_MARCXML)
#include <expat.h>
#endif /* MARCRECORD_MARCXML */

/*
 * MARC record class.
 */
class MarcRecord {
public:
	/* Enum of MARC record format variants. */
	enum FormatVariant {
		UNIMARC = 1,
		MARC21 = 2
	};

	/* Enum of error codes. */
	enum ErrorCode {
		SUCCESS = 0,
		ERROR = -1
	};

	#pragma pack(push)
	#pragma pack(1)

	/* Structure of MARC record leader. */
	struct Leader {
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
	typedef FieldList::iterator FieldIt;
	/* List of fields iterators. */
	typedef std::list<FieldIt> FieldRefList;
	typedef FieldRefList::iterator FieldRefIt;

	/* List of subfields. */
	typedef std::list<Subfield> SubfieldList;
	typedef SubfieldList::iterator SubfieldIt;
	/* List of subfields iterators. */
	typedef std::list<SubfieldIt> SubfieldRefList;
	typedef SubfieldRefList::iterator SubfieldRefIt;

	/* List of embedded fields. */
	typedef std::list<SubfieldRefList> EmbeddedFieldList;
	typedef EmbeddedFieldList::iterator EmbeddedFieldIt;

private:
	/* Variant of record format. */
	FormatVariant formatVariant;
	/* Flag of null-value record (for handling errors and events). */
	bool nullFlag;

	/* Record leader. */
	Leader leader;
	/* List of fields. */
	FieldList fieldList;

private:
	/* Parse field from ISO 2709 buffer. */
	inline Field parseFieldIso2709(std::string fieldTag,
		const char *fieldData, unsigned int fieldLength, const char *encoding);

public:
	/* Constructors and destructor. */
	MarcRecord();
	MarcRecord(FormatVariant newFormatVariant);
	~MarcRecord();

	/* Clear record. */
	void clear(void);

	/* Set flag of null-value record. */
	void setNull(bool nullFlag = true);
	/* Get flag of null-value record. */
	bool isNull();

	/* Get record format variant. */
	FormatVariant getFormatVariant(void);
	/* Set record format variant. */
	void setFormatVariant(FormatVariant newFormatVariant);

	/* Parse record from ISO 2709 buffer. */
	bool parseRecordIso2709(const char *recordBuf, const char *encoding = "UTF-8");
	/* Read record from ISO 2709 file. */
	bool readIso2709(FILE *inputFile, const char *encoding = "UTF-8");
	/* Write record to ISO 2709 file. */
	bool writeIso2709(FILE *outputFile, const char *encoding = "UTF-8");

	/* Get record leader. */
	Leader getLeader(void);
	/* Set record leader. */
	void setLeader(Leader &leader);
	void setLeader(std::string leaderData = "");

	/* Get list of fields. */
	FieldRefList getFields(std::string fieldTag = "");
	/* Get field. */
	FieldIt getField(std::string fieldTag);

	/* Add field to the end of record. */
	FieldIt addField(Field field);
	FieldIt addControlField(std::string fieldTag = "", std::string fieldData = "");
	FieldIt addDataField(std::string fieldTag = "", char fieldInd1 = ' ', char fieldInd2 = ' ');
	/* Add field to the record before specified field. */
	FieldIt addFieldBefore(FieldIt nextFieldIt, Field field);
	FieldIt addControlFieldBefore(FieldIt nextFieldIt,
		std::string fieldTag = "", std::string fieldData = "");
	FieldIt addDataFieldBefore(FieldIt nextFieldIt,
		std::string fieldTag = "", char fieldInd1 = ' ', char fieldInd2 = ' ');
	/* Remove field from the record. */
	void removeField(FieldIt fieldIt);

	/* Format record to string for printing. */
	std::string toString(void);

	/* Return null field value. */
	inline FieldIt nullField(void)
	{
		return fieldList.end();
	}
};

/*
 * MARC field class.
 */
class MarcRecord::Field {
public:
	/* Types of field. */
	enum Type {
		CONTROLFIELD,
		DATAFIELD
	};

public:
	/* Type of field. */
	enum Type type;
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
	/* Constructors. */
	Field(std::string tag = "", std::string data = "");
	Field(std::string tag, char ind1, char ind2);

	/* Clear field data. */
	void clear();

	/* Get list of subfields. */
	SubfieldRefList getSubfields(char subfieldId = ' ');
	/* Get subfield. */
	SubfieldIt getSubfield(char subfieldId);

	/* Get list of embedded fields. */
	EmbeddedFieldList getEmbeddedFields(std::string fieldTag = "");
	/* Get embedded field. */
	SubfieldRefList getEmbeddedField(std::string fieldTag);

	/* Add subfield to the end of field. */
	SubfieldIt addSubfield(Subfield subfield);
	SubfieldIt addSubfield(char subfieldId = ' ', std::string subfieldData = "");
	/* Add subfield to the field before specified subfield. */
	SubfieldIt addSubfieldBefore(SubfieldIt nextSubfieldIt, Subfield subfield);
	SubfieldIt addSubfieldBefore(SubfieldIt nextSubfieldIt,
		char subfieldId = ' ', std::string subfieldData = "");
	/* Remove subfield from the field. */
	void removeSubfield(SubfieldIt subfieldIt);

	/* Return null subfield value. */
	inline SubfieldIt nullSubfield()
	{
		return subfieldList.end();
	}

	/* Format field to string for printing. */
	std::string toString();
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

#if defined(MARCRECORD_MARCXML)

/*
 * MARCXML records reader.
 */
class MarcXmlReader {
public:
	/* Exception class for events and errors handling. */
	class Exception {
	public:
		enum ErrorCode { ERROR_XML } errorCode;
		std::string errorMessage;

		Exception(enum ErrorCode errorCode, std::string errorMessage)
		{
			this->errorCode = errorCode;
			this->errorMessage = errorMessage;
		}
	};

	/* XML parser state structure definition. */
	struct XmlParserState {
		XML_Parser xmlParser;
		bool done;
		bool paused;
		unsigned int level;
		std::vector<std::string> tags;
		MarcRecord *record;
		MarcRecord::Field *field;
		std::string characterData;
	};

protected:
	/* Input MARCXML file. */
	FILE *inputFile;
	/* Encoding of input MARCXML file. */
	std::string inputEncoding;

	/* XML parser. */
	XML_Parser xmlParser;
	/* XML parser state. */
	struct XmlParserState parserState;
	/* Record buffer. */
	char buffer[4096];

public:
	/* Constructor. */
	MarcXmlReader(FILE *inputFile = NULL, const char *inputEncoding = "UTF-8");
	/* Destructor. */
	~MarcXmlReader();

	/* Read next record from MARCXML file. */
	MarcRecord next(void);
};

#endif /* MARCRECORD_MARCXML */

#endif /* MARCRECORD_H */
