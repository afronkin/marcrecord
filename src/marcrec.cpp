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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "marcrec.h"

/* Print formatted output to std::string. */
int snprintf(std::string &s, size_t n, const char *format, ...);

/*
 * Constructor.
 */
MarcRecord::MarcRecord()
{
	recordType = UNIMARC;
	clear();
}

MarcRecord::MarcRecord(RecordType newRecordType)
{
	setType(newRecordType);
	clear();
}

/*
 * Destructor.
 */
MarcRecord::~MarcRecord()
{
}

/*
 * Clear record.
 */
void MarcRecord::clear()
{
	/* Clear field list. */
	fieldList.clear();

	/* Reset record label. */
	memset(label.recordLength, ' ', sizeof(label.recordLength));
	label.recordStatus = 'n';
	label.recordType = 'a';
	label.bibliographicalLevel = 'm';
	label.hierarchicalLevel = ' ';
	label.undefined1 = ' ';
	label.indicatorLength = '2';
	label.subfieldIdLength = '2';
	memset(label.baseAddress, ' ', sizeof(label.baseAddress));
	label.encodingLevel = ' ';
	label.cataloguingForm = ' ';
	label.undefined2 = ' ';
	label.lengthFieldLength = '4';
	label.startPosLength = '5';
	label.implDefLength = '0';
	label.undefined3 = ' ';
}

/*
 * Set record type.
 */
void MarcRecord::setType(RecordType newRecordType)
{
	recordType = newRecordType;
}

/*
 * Read record from file.
 */
bool MarcRecord::read(FILE *marcFile, const char *encoding)
{
	int symbol;
	char recordBuf[10000];
	unsigned int recordLen;

	/* Skip possible wrong symbols. */
	do {
		symbol = fgetc(marcFile);
	} while (symbol >= 0 && isdigit(symbol) == 0);

	if (symbol < 0) {
		return false;
	}

	/* Read record length. */
	recordBuf[0] = (char) symbol;
	if (fread(recordBuf + 1, 1, 4, marcFile) != 4) {
		return false;
	}

	/* Parse record length. */
	if (sscanf(recordBuf, "%5d", &recordLen) != 1) {
		return false;
	}

	/* Read record. */
	if (fread(recordBuf + 5, 1, recordLen - 5, marcFile) != recordLen - 5) {
		return false;
	}

	/* Parse record. */
	return parse(recordBuf, encoding);
}

/*
 * Write record to file.
 */
bool MarcRecord::write(FILE *marcFile, const char *encoding)
{
	return true;
}

/*
 * Parse record from buffer.
 */
bool MarcRecord::parse(const char *recordBuf, const char *encoding)
{
	unsigned int baseAddress, numFields;
	RecordDirEntry *dirEntry;
	const char *recordData, *fieldData;
	int fieldNo;
	std::string fieldTag;
	unsigned int fieldLength, fieldStartPos;
	Field field;

	try {
		/* Copy record label. */
		memcpy(&label, recordBuf, sizeof(RecordLabel));

		/* Get base address of data. */
		if (sscanf(label.baseAddress, "%05d", &baseAddress) != 1) {
			throw ERROR;
		}

		/* Get number of fields. */
		numFields = (baseAddress - sizeof(RecordLabel) - 1) /
			sizeof(RecordDirEntry);

		/* Parse list of fields. */
		dirEntry = (RecordDirEntry *) (recordBuf + sizeof(RecordLabel));
		recordData = recordBuf + baseAddress;
		for (fieldNo = 0; fieldNo < numFields; fieldNo++, dirEntry++) {
			/* Parse directory entry. */
			fieldTag.assign(dirEntry->tag, 0, 3);
			if (sscanf(dirEntry->length, "%4d%5d", &fieldLength, &fieldStartPos) != 2) {
				throw ERROR;
			}

			/* Parse field. */
			field = parseField(fieldTag, recordData + fieldStartPos, fieldLength, encoding);
			/* Append field to list. */
			fieldList.push_back(field);
		}
	} catch (int errorCode) {
		if (errorCode != 0) {
			clear();
			return false;
		}
	}

	return true;
}

/*
 * Parse field.
 */
MarcRecord::Field MarcRecord::parseField(std::string fieldTag,
	const char *fieldData, unsigned int fieldLength, const char *encoding)
{
	Field field;
	Subfield subfield;
	int symbolPos, subfieldStartPos;

	/* Clear field. */
	/* field.clear(); */

	/* Adjust field length. */
	if (fieldData[fieldLength - 1] == '\x1E') {
		fieldLength--;
	}

	field.tag = fieldTag;
	if (field.tag < "010" || fieldLength < 2) {
		/* Parse control field. */
		field.data.assign(fieldData, fieldLength);
	} else {
		/* Parse regular field. */
		field.ind1 = fieldData[0];
		field.ind2 = fieldData[1];

		/* Parse list of subfields. */
		subfieldStartPos = 0;
		for (symbolPos = 2; symbolPos <= fieldLength; symbolPos++) {
			/* Skip symbols of subfield data. */
			if (fieldData[symbolPos] != '\x1F' && symbolPos != fieldLength) {
				continue;
			}

			if (symbolPos > 2) {
				/* Parse regular subfield. */
				subfield.clear();
				subfield.id = fieldData[subfieldStartPos + 1];
				subfield.data.assign(
					fieldData + subfieldStartPos + 2,
					symbolPos - subfieldStartPos - 2);

				/* Append subfield to list. */
				field.subfieldList.push_back(subfield);
			}

			subfieldStartPos = symbolPos;
		}
	}

	return field;
}

/*
 * Get list of fields from record.
 */
MarcRecord::FieldPtrList MarcRecord::getFieldList(std::string fieldTag)
{
	FieldPtrList resultFieldList;
	FieldRef fieldRef;

	/* Check all fields in list. */
	for (fieldRef = fieldList.begin(); fieldRef != fieldList.end();
		fieldRef++)
	{
		if (fieldTag == "" || fieldTag == fieldRef->tag) {
			resultFieldList.push_back(fieldRef);
		}
	}

	return resultFieldList;
}

/*
 * Get list of subfields from field.
 */
MarcRecord::SubfieldPtrList MarcRecord::getSubfieldList(FieldRef fieldRef,
	char subfieldId)
{
	SubfieldPtrList resultSubfieldList;
	SubfieldRef subfieldRef;

	for (subfieldRef = fieldRef->subfieldList.begin();
		subfieldRef != fieldRef->subfieldList.end(); subfieldRef++)
	{
		if (subfieldId == ' ' || subfieldRef->id == subfieldId) {
			resultSubfieldList.push_back(subfieldRef);
		}
	}

	return resultSubfieldList;
}

/*
 * Format record to string for printing.
 */
std::string MarcRecord::toString()
{
	std::string textRecord = "";
	MarcRecord::FieldRef fieldRef;

	/* Enumerate all fields. */
	for (fieldRef = fieldList.begin(); fieldRef != fieldList.end(); fieldRef++) {
		/* Print field. */
		textRecord += toString(*fieldRef) + "\n";
	}

	return textRecord;
}

/*
 * Format field to string for printing.
 */
std::string MarcRecord::toString(Field field)
{
	std::string textField = "";
	MarcRecord::SubfieldRef subfieldRef, embeddedSubfieldRef;

	textField += field.tag;

	if (field.tag < "010") {
		/* Print control field. */
		textField += " ";
		textField += field.data.c_str();
	} else {
		/* Print header of regular field. */
		snprintf(textField, 5, " [%c%c]", field.ind1, field.ind2);

		/* Enumerate all subfields. */
		for (subfieldRef = field.subfieldList.begin();
			subfieldRef != field.subfieldList.end(); subfieldRef++)
		{
			if (recordType == UNIMARC && subfieldRef->id == '1') {
				/* Print header of embedded field. */
				snprintf(textField, 4, " $%c ", subfieldRef->id);
				if (subfieldRef->getEmbeddedTag() < "010") {
					textField += "<" + subfieldRef->getEmbeddedTag() + "> "
						+ subfieldRef->getEmbeddedData();
				} else {
					textField += "<" + subfieldRef->getEmbeddedTag() + "> ["
						+ subfieldRef->getEmbeddedInd1()
						+ subfieldRef->getEmbeddedInd2() + "]";
				}
			} else {
				/* Print regular subfield. */
				snprintf(textField, 4, " $%c ", subfieldRef->id);
				textField += subfieldRef->data;
			}
		}
	}

	return textField;
}

/*
 * Clear field data.
 */
void MarcRecord::Field::clear()
{
	tag = "";
	ind1 = ' ';
	ind2 = ' ';
	data.erase();
	subfieldList.clear();
}

/*
 * Clear subfield data.
 */
void MarcRecord::Subfield::clear()
{
	id = ' ';
	data.erase();
}

/*
 * Check presence of embedded field.
 */
bool MarcRecord::Subfield::isEmbedded()
{
	return (id == '1' ? true : false);
}

/*
 * Get tag of embedded field.
 */
std::string MarcRecord::Subfield::getEmbeddedTag()
{
	if (id != '1') {
		return "";
	}

	return data.substr(0, 3);
}

/*
 * Get indicator 1 of embedded field.
 */
char MarcRecord::Subfield::getEmbeddedInd1()
{
	if (id != '1' || data.substr(0, 3) < "010") {
		return '?';
	}

	return data[3];
}

/*
 * Get indicator 2 of embedded field.
 */
char MarcRecord::Subfield::getEmbeddedInd2()
{
	if (id != '1' || data.substr(0, 3) < "010") {
		return '?';
	}

	return data[4];
}

/*
 * Get data of embedded field.
 */
std::string MarcRecord::Subfield::getEmbeddedData()
{
	if (id != '1' || data.substr(0, 3) >= "010") {
		return "";
	}

	return data.substr(3);
}

/*
 * Print formatted output to std::string.
 */
int snprintf(std::string &s, size_t n, const char *format, ...)
{
	va_list ap;
	char *buf;
	int resultCode;

	buf = (char *) malloc(n + 1);
	va_start(ap, format);
	resultCode = vsnprintf(buf, n + 1, format, ap);
	va_end(ap);

	if (resultCode >= 0) {
		s.append(buf);
	}
	free(buf);

	return resultCode;
}

