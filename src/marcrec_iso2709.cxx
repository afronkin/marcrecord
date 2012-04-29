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

#include "marcrec.h"

/*
 * Read record from ISO 2709 file.
 */
bool MarcRecord::readIso2709(FILE *marcFile, const char *encoding)
{
	int symbol;
	char recordBuf[100000];
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
	return parseIso2709(recordBuf, encoding);
}

/*
 * Write record to ISO 2709 file.
 */
bool MarcRecord::writeIso2709(FILE *marcFile, const char *encoding)
{
	return true;
}

/*
 * Parse record from ISO 2709 buffer.
 */
bool MarcRecord::parseIso2709(const char *recordBuf, const char *encoding)
{
	unsigned int baseAddress, numFields;
	RecordDirectoryEntry *directoryEntry;
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
			sizeof(RecordDirectoryEntry);

		/* Parse list of fields. */
		directoryEntry = (RecordDirectoryEntry *) (recordBuf + sizeof(RecordLabel));
		recordData = recordBuf + baseAddress;
		for (fieldNo = 0; fieldNo < numFields; fieldNo++, directoryEntry++) {
			/* Parse directory entry. */
			fieldTag.assign(directoryEntry->fieldTag, 0, 3);
			if (sscanf(directoryEntry->fieldLength, "%4d%5d", &fieldLength, &fieldStartPos) != 2) {
				throw ERROR;
			}

			/* Parse field. */
			field = parseField(fieldTag,
				recordData + fieldStartPos, fieldLength, encoding);
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
