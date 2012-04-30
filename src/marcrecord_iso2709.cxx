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

#include "marcrecord.h"

#define ISO2709_RECORD_SEPARATOR	'\x1D'
#define ISO2709_FIELD_SEPARATOR		'\x1E'
#define ISO2709_IDENTIFIER_DELIMITER	'\x1F'

#pragma pack(push)
#pragma pack(1)

/* Structure of record directory entry. */
struct RecordDirectoryEntry {
	// Field tag.
	char fieldTag[3];
	// Field length.
	char fieldLength[4];
	// Field starting position.
	char fieldStartingPosition[5];
};

#pragma pack(pop)

/*
 * Parse record from ISO 2709 buffer.
 */
bool MarcRecord::parseRecordIso2709(const char *recordBuf, const char *encoding)
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
		memcpy(&label, recordBuf, sizeof(struct Label));

		/* Get base address of data. */
		if (sscanf(label.baseAddress, "%05d", &baseAddress) != 1) {
			throw ERROR;
		}

		/* Get number of fields. */
		numFields = (baseAddress - sizeof(struct Label) - 1) /
			sizeof(struct RecordDirectoryEntry);

		/* Parse list of fields. */
		directoryEntry = (RecordDirectoryEntry *) (recordBuf + sizeof(struct Label));
		recordData = recordBuf + baseAddress;
		for (fieldNo = 0; fieldNo < numFields; fieldNo++, directoryEntry++) {
			/* Parse directory entry. */
			fieldTag.assign(directoryEntry->fieldTag, 0, 3);
			if (sscanf(directoryEntry->fieldLength, "%4d%5d",
				&fieldLength, &fieldStartPos) != 2)
			{
				throw ERROR;
			}

			/* Parse field. */
			field = parseFieldIso2709(fieldTag,
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
 * Parse field from ISO 2709 buffer.
 */
MarcRecord::Field MarcRecord::parseFieldIso2709(std::string fieldTag,
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
 * Read record from ISO 2709 file.
 */
bool MarcRecord::readIso2709(FILE *inputFile, const char *encoding)
{
	int symbol;
	char recordBuf[100000];
	unsigned int recordLen;

	/* Skip possible wrong symbols. */
	do {
		symbol = fgetc(inputFile);
	} while (symbol >= 0 && isdigit(symbol) == 0);

	if (symbol < 0) {
		return false;
	}

	/* Read record length. */
	recordBuf[0] = (char) symbol;
	if (fread(recordBuf + 1, 1, 4, inputFile) != 4) {
		return false;
	}

	/* Parse record length. */
	if (sscanf(recordBuf, "%5d", &recordLen) != 1) {
		return false;
	}

	/* Read record. */
	if (fread(recordBuf + 5, 1, recordLen - 5, inputFile) != recordLen - 5) {
		return false;
	}

	/* Parse record. */
	return parseRecordIso2709(recordBuf, encoding);
}

/*
 * Write record to ISO 2709 file.
 */
bool MarcRecord::writeIso2709(FILE *outputFile, const char *encoding)
{
	char recordBuf[100000];

	/* Copy record leader to buffer. */
	memcpy(recordBuf, (char *) &label, sizeof(struct Label));

	/* Calculate base address of data and copy it to record buffer. */
	unsigned int baseAddress = sizeof(struct Label) + fieldList.size()
		* sizeof(struct RecordDirectoryEntry) + 1;
	char baseAddressBuf[6];
	sprintf(baseAddressBuf, "%05d", baseAddress);
	memcpy(recordBuf + 12, baseAddressBuf, 5);

	/* Iterate all fields. */
	char *directoryData = recordBuf + sizeof(struct Label);
	char *fieldData = recordBuf + baseAddress;
	for (MarcRecord::FieldIt fieldIt = fieldList.begin();
		fieldIt != fieldList.end(); fieldIt++)
	{
		int fieldLength = 0;
		if (fieldIt->tag < "010") {
			/* Copy control field to buffer. */
			fieldLength = fieldIt->data.size();
			memcpy(fieldData, fieldIt->data.c_str(), fieldLength);
			fieldData += fieldLength;
		} else {
			/* Copy indicators of data field to buffer. */
			*(fieldData++) = fieldIt->ind1;
			*(fieldData++) = fieldIt->ind2;
			fieldLength += 2;

			/* Iterate all subfields. */
			for (MarcRecord::SubfieldIt subfieldIt = fieldIt->subfieldList.begin();
				subfieldIt != fieldIt->subfieldList.end(); subfieldIt++)
			{
				int subfieldLength = subfieldIt->data.size();

				/* Copy subfield to buffer. */
				*(fieldData++) = ISO2709_IDENTIFIER_DELIMITER;
				*(fieldData++) = subfieldIt->id;
				memcpy(fieldData, subfieldIt->data.c_str(), subfieldLength);
				fieldData += subfieldLength;
				fieldLength += subfieldLength + 2;
			}
		}

		/* Set field separator at the end of field. */
		*(fieldData++) = ISO2709_FIELD_SEPARATOR;
		fieldLength++;

		/* Fill directory entry (it is safe to do this way because null character
		   will be overwritten in next iteration and right after the cycle). */
		sprintf(directoryData, "%.3s%04d%05d", fieldIt->tag.c_str(), fieldLength,
			(int) (fieldData - recordBuf) - baseAddress - fieldLength);
		directoryData += sizeof(struct RecordDirectoryEntry);
	}

	/* Set field separator at the end of directory. */
	directoryData[0] = ISO2709_FIELD_SEPARATOR;
	/* Set record separator at the end of record. */
	*(fieldData++) = ISO2709_RECORD_SEPARATOR;

	/* Calculate directory length and copy it to record buffer (can't use easy way here). */
	int recordLength = (int) (fieldData - recordBuf);
	char recordLengthBuf[6];
	sprintf(recordLengthBuf, "%05d", recordLength);
	memcpy(recordBuf, recordLengthBuf, 5);

	/* Write record buffer to file. */
	fwrite(recordBuf, recordLength, 1, outputFile);

	return true;
}
