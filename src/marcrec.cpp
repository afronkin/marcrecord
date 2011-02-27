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
bool MarcRecord::read(FILE *file, const char *encoding)
{
	int symbol;
	char recordBuf[10000];
	size_t recordLen;

	/* Skip possible wrong symbols. */
	do {
		symbol = fgetc(file);
	} while (symbol >= 0 && isdigit(symbol) == 0);

	if (symbol < 0) {
		return false;
	}

	/* Read record length. */
	recordBuf[0] = (char) symbol;
	if (fread(recordBuf + 1, 1, 4, file) != 4) {
		return false;
	}

	/* Parse record length. */
	if (sscanf(recordBuf, "%5d", &recordLen) != 1) {
		return false;
	}

	/* Read record. */
	if (fread(recordBuf + 5, 1, recordLen - 5, file) != recordLen - 5) {
		return false;
	}

	/* Parse record. */
	return parse(recordBuf, encoding);
}

/*
 * Write record to file.
 */
bool MarcRecord::write(FILE *file, const char *encoding)
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
	Field field;
	Subfield subfield;
	size_t fieldLength, fieldStartPos;
	int symbolPos, subfieldStartPos;

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
			/* Clear field. */
			field.clear();

			/* Parse directory entry. */
			if (sscanf((char *) dirEntry, "%3d%4d%5d",
				&field.tag, &fieldLength, &fieldStartPos) != 3)
			{
				throw ERROR;
			}

			/* Parse field. */
			fieldData = recordData + fieldStartPos;
			if (fieldData[fieldLength - 1] == '\x1e') {
				fieldLength--;
			}

			if (field.tag < 10 || fieldLength < 2) {
				/* Parse control field. */
				field.data.assign(fieldData, fieldLength);
			} else {
				/* Parse regular field. */
				field.ind1 = fieldData[0];
				field.ind2 = fieldData[1];

				/* Parse list of subfields. */
				for (symbolPos = 2; symbolPos <= fieldLength; symbolPos++) {
					if (fieldData[symbolPos] == '\x1f' ||
						symbolPos == fieldLength)
					{
						if (symbolPos > 2) {
							/* Clear subfield. */
							subfield.clear();
							/* Get subfield identifier. */
							subfield.id = fieldData[subfieldStartPos + 1];

							/* Check for embedded field. */
							if (subfield.id == '1' &&
								recordType == UNIMARC &&
								sscanf(fieldData + subfieldStartPos + 2,
									"%3d", &subfield.embeddedField.tag) == 1)
							{
								/* Parse embedded field. */
								subfield.embeddedField.subfieldList.clear();
								if (subfield.embeddedField.tag < 10) {
									subfield.embeddedField.data.assign(
										fieldData + subfieldStartPos + 5,
										symbolPos - subfieldStartPos - 5);
								} else {
									subfield.embeddedField.ind1 =
										fieldData[subfieldStartPos + 5];
									subfield.embeddedField.ind2 =
										fieldData[subfieldStartPos + 6];

									/* Parse subfields of embedded field. */
									// .............
								}
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
			}

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
	MarcRecord::SubfieldRef subfieldRef;

	for (fieldRef = fieldList.begin(); fieldRef != fieldList.end();
		fieldRef++)
	{
		snprintf(textRecord, 3, "%03d", fieldRef->tag);

		if (fieldRef->tag < 10) {
			textRecord += " ";
			textRecord += fieldRef->data.c_str();
		} else {
			snprintf(textRecord, 5, " [%c%c]",
				fieldRef->ind1, fieldRef->ind2);

			for (subfieldRef = fieldRef->subfieldList.begin();
				subfieldRef != fieldRef->subfieldList.end(); subfieldRef++)
			{
				snprintf(textRecord, 4, " $%c ", subfieldRef->id);
				textRecord += subfieldRef->data;
			}
		}

		textRecord += "\n";
	}

	return textRecord;
}
