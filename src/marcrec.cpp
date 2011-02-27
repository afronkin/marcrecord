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
	size_t recordLen;

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
	size_t baseAddress, numFields;
	RecordDirEntry *dirEntry;
	const char *recordData, *fieldData;
	int fieldNo;
	int fieldTag;
	size_t fieldLength, fieldStartPos;
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
			if (sscanf((char *) dirEntry, "%3d%4d%5d",
				&fieldTag, &fieldLength, &fieldStartPos) != 3)
			{
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
MarcRecord::Field MarcRecord::parseField(int fieldTag, const char *fieldData, size_t fieldLength, const char *encoding)
{
	Field field;
	Subfield subfield;
	int symbolPos, subfieldStartPos;
	int embeddedFieldTag;

	/* Clear field. */
	/* field.clear(); */

	/* Adjust field length. */
	if (fieldData[fieldLength - 1] == '\x1E') {
		fieldLength--;
	}

	field.tag = fieldTag;
	if (field.tag < 10 || fieldLength < 2) {
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
				/* Skip subfields of embedded field. */
				if (symbolPos < fieldLength && fieldData[subfieldStartPos + 1] == '1') {
					if (fieldData[symbolPos + 1] != '1') {
						continue;
					}
				}

				/* Clear subfield. */
				subfield.clear();
				/* Get subfield identifier. */
				subfield.id = fieldData[subfieldStartPos + 1];

				/* Check for embedded field. */
				if ( recordType == UNIMARC && subfield.id == '1') {
					/* Get embedded field tag. */
					if (sscanf(fieldData + subfieldStartPos + 2, "%3d", &embeddedFieldTag) != 1) {
						embeddedFieldTag = 0;
					}

					/* Parse embedded field. */
					subfield.embeddedField = parseEmbeddedField(embeddedFieldTag,
						fieldData + subfieldStartPos + 5,
						symbolPos - subfieldStartPos - 5,
						encoding);
				} else {
					/* Parse regular subfield. */
					subfield.data.assign(
						fieldData + subfieldStartPos + 2,
						symbolPos - subfieldStartPos - 2);
				}

				/* Append subfield to list. */
				field.subfieldList.push_back(subfield);
			}

			subfieldStartPos = symbolPos;
		}
	}

	return field;
}

/*
 * Parse embedded field.
 */
MarcRecord::Field MarcRecord::parseEmbeddedField(int fieldTag, const char *fieldData, size_t fieldLength,
	const char *encoding)
{
	Field field;
	Subfield subfield;
	int symbolPos, subfieldStartPos;

	field.tag = fieldTag;
	if (field.tag < 10) {
		field.data.assign(fieldData, fieldLength);
	} else {
		field.ind1 = fieldData[0];
		field.ind2 = fieldData[1];

		/* Parse list of subfields. */
		subfieldStartPos = 2;
		for (symbolPos = 2; symbolPos <= fieldLength; symbolPos++) {
			if (fieldData[symbolPos] != '\x1F' && symbolPos != fieldLength) {
				continue;
			}

			if (symbolPos > 2) {
				/* Clear subfield. */
				subfield.clear();
				/* Get subfield identifier. */
				subfield.id = fieldData[subfieldStartPos + 1];

				/* Parse regular subfield. */
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
MarcRecord::FieldPtrList MarcRecord::getFieldList(int fieldTag)
{
	FieldPtrList resultFieldList;
	FieldRef fieldRef;

	/* Check all fields in list. */
	for (fieldRef = fieldList.begin(); fieldRef != fieldList.end();
		fieldRef++)
	{
		if (fieldTag == 0 || fieldTag == fieldRef->tag) {
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

	for (fieldRef = fieldList.begin(); fieldRef != fieldList.end(); fieldRef++)
	{
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

	snprintf(textField, 3, "%03d", field.tag);

	if (field.tag < 10) {
		textField += " ";
		textField += field.data.c_str();
	} else {
		snprintf(textField, 5, " [%c%c]", field.ind1, field.ind2);

		for (subfieldRef = field.subfieldList.begin();
			subfieldRef != field.subfieldList.end(); subfieldRef++)
		{
			if (recordType == UNIMARC && subfieldRef->id == '1') {
				snprintf(textField, 4, " $%c ", subfieldRef->id);
				textField += toString(subfieldRef->embeddedField);
			} else {
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
	tag = 0;
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
	embeddedField.clear();
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

