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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "marcrecord.h"

/* Print formatted output to std::string. */
int snprintf(std::string &s, size_t n, const char *format, ...);

/*
 * Constructor.
 */
MarcRecord::MarcRecord()
{
	formatVariant = UNIMARC;
	clear();
}

MarcRecord::MarcRecord(FormatVariant newFormatVariant)
{
	setFormatVariant(newFormatVariant);
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
void MarcRecord::clear(void)
{
	/* Clear null-value record flag. */
	nullFlag = false;

	/* Clear field list. */
	fieldList.clear();

	/* Reset record leader. */
	memset(leader.recordLength, ' ', sizeof(leader.recordLength));
	leader.recordStatus = 'n';
	leader.recordType = 'a';
	leader.bibliographicLevel = 'm';
	leader.hierarchicalLevel = ' ';
	leader.undefined1 = ' ';
	leader.indicatorLength = '2';
	leader.subfieldIdLength = '2';
	memset(leader.baseAddress, ' ', sizeof(leader.baseAddress));
	leader.encodingLevel = ' ';
	leader.cataloguingForm = ' ';
	leader.undefined2 = ' ';
	leader.lengthOfFieldLength = '4';
	leader.startingPositionLength = '5';
	leader.implementationDefinedLength = '0';
	leader.undefined3 = ' ';
}

/*
 * Set flag of null-value record.
 */
void MarcRecord::setNull(bool nullFlag)
{
	/* Clear record when set null flag to true. */
	if (nullFlag) {
		clear();
	}

	this->nullFlag = nullFlag;
}

/*
 * Get flag of null-value record.
 */
bool MarcRecord::isNull()
{
	return nullFlag;
}

/*
 * Get record format variant.
 */
MarcRecord::FormatVariant MarcRecord::getFormatVariant(void)
{
	return formatVariant;
}

/*
 * Set record format variant.
 */
void MarcRecord::setFormatVariant(FormatVariant newFormatVariant)
{
	formatVariant = newFormatVariant;
}

/*
 * Get record leader.
 */
MarcRecord::Leader MarcRecord::getLeader(void)
{
	return leader;
}

/*
 * Set record leader.
 */
void MarcRecord::setLeader(Leader &leader)
{
	this->leader = leader;
}

void MarcRecord::setLeader(std::string leaderData)
{
	memcpy((void *) &leader, leaderData.c_str(),
		std::min(sizeof(struct Leader), leaderData.size()));
}

/*
 * Get list of fields.
 */
MarcRecord::FieldRefList MarcRecord::getFields(std::string fieldTag)
{
	FieldRefList resultFieldList;
	FieldIt fieldIt;

	/* Check fields in list. */
	for (fieldIt = fieldList.begin(); fieldIt != fieldList.end();
		fieldIt++)
	{
		if (fieldTag == "" || fieldTag == fieldIt->tag) {
			resultFieldList.push_back(fieldIt);
		}
	}

	return resultFieldList;
}

/*
 * Get field.
 */
MarcRecord::FieldIt MarcRecord::getField(std::string fieldTag)
{
	FieldIt fieldIt;

	/* Check fields in list. */
	for (fieldIt = fieldList.begin(); fieldIt != fieldList.end();
		fieldIt++)
	{
		if (fieldTag == "" || fieldTag == fieldIt->tag) {
			return fieldIt;
		}
	}

	return fieldList.end();
}

/*
 * Add field to the end of record.
 */
MarcRecord::FieldIt MarcRecord::addField(Field field)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(fieldList.end(), field);
	return fieldIt;
}

MarcRecord::FieldIt MarcRecord::addControlField(std::string fieldTag, std::string fieldData)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(fieldList.end(), Field(fieldTag, fieldData));
	return fieldIt;
}

MarcRecord::FieldIt MarcRecord::addDataField(std::string fieldTag, char fieldInd1, char fieldInd2)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(fieldList.end(), Field(fieldTag, fieldInd1, fieldInd2));
	return fieldIt;
}

/*
 * Add field to the record before specified field.
 */
MarcRecord::FieldIt MarcRecord::addFieldBefore(FieldIt nextFieldIt, Field field)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(nextFieldIt, field);
	return fieldIt;
}

MarcRecord::FieldIt MarcRecord::addControlFieldBefore(FieldIt nextFieldIt,
	std::string fieldTag, std::string fieldData)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(nextFieldIt, Field(fieldTag, fieldData));
	return fieldIt;
}

MarcRecord::FieldIt MarcRecord::addDataFieldBefore(FieldIt nextFieldIt,
	std::string fieldTag, char fieldInd1, char fieldInd2)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(nextFieldIt, Field(fieldTag, fieldInd1, fieldInd2));
	return fieldIt;
}

/*
 * Remove field from the record.
 */
void MarcRecord::removeField(FieldIt fieldIt)
{
	/* Remove field from the list. */
	fieldList.erase(fieldIt);
}

/*
 * Format record to string for printing.
 */
std::string MarcRecord::toString(void)
{
	std::string textRecord = "";
	MarcRecord::FieldIt fieldIt;

	/* Iterate all fields. */
	for (fieldIt = fieldList.begin(); fieldIt != fieldList.end(); fieldIt++) {
		/* Print field. */
		textRecord += fieldIt->toString() + "\n";
	}

	return textRecord;
}
