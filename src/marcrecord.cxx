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
	/* Clear field list. */
	fieldList.clear();

	/* Reset record label. */
	memset(label.recordLength, ' ', sizeof(label.recordLength));
	label.recordStatus = 'n';
	label.recordType = 'a';
	label.bibliographicLevel = 'm';
	label.hierarchicalLevel = ' ';
	label.undefined1 = ' ';
	label.indicatorLength = '2';
	label.subfieldIdLength = '2';
	memset(label.baseAddress, ' ', sizeof(label.baseAddress));
	label.encodingLevel = ' ';
	label.cataloguingForm = ' ';
	label.undefined2 = ' ';
	label.lengthOfFieldLength = '4';
	label.startingPositionLength = '5';
	label.implementationDefinedLength = '0';
	label.undefined3 = ' ';
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
 * Get record label.
 */
MarcRecord::Label MarcRecord::getLabel(void)
{
	return label;
}

/*
 * Set record label.
 */
void MarcRecord::setLabel(Label &newLabel)
{
	label = newLabel;
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

MarcRecord::FieldIt MarcRecord::addField(std::string fieldTag, char fieldInd1, char fieldInd2)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(fieldList.end(),
		Field(fieldTag, fieldInd1, fieldInd2));
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

MarcRecord::FieldIt MarcRecord::addFieldBefore(FieldIt nextFieldIt,
	std::string fieldTag, char fieldInd1, char fieldInd2)
{
	/* Append field to the list. */
	FieldIt fieldIt = fieldList.insert(nextFieldIt,
		Field(fieldTag, fieldInd1, fieldInd2));
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

	/* Enumerate all fields. */
	for (fieldIt = fieldList.begin(); fieldIt != fieldList.end(); fieldIt++) {
		/* Print field. */
		textRecord += toString(*fieldIt) + "\n";
	}

	return textRecord;
}

/*
 * Format field to string for printing.
 */
std::string MarcRecord::toString(Field field)
{
	std::string textField = "";
	MarcRecord::SubfieldIt subfieldIt, embeddedSubfieldIt;

	textField += field.tag;

	if (field.tag < "010") {
		/* Print control field. */
		textField += " ";
		textField += field.data.c_str();
	} else {
		/* Print header of regular field. */
		snprintf(textField, 5, " [%c%c]", field.ind1, field.ind2);

		/* Enumerate all subfields. */
		for (subfieldIt = field.subfieldList.begin();
			subfieldIt != field.subfieldList.end(); subfieldIt++)
		{
			if (formatVariant == UNIMARC && subfieldIt->id == '1') {
				/* Print header of embedded field. */
				snprintf(textField, 4, " $%c ", subfieldIt->id);
				if (subfieldIt->getEmbeddedTag() < "010") {
					textField += "<" + subfieldIt->getEmbeddedTag() + "> "
						+ subfieldIt->getEmbeddedData();
				} else {
					textField += "<" + subfieldIt->getEmbeddedTag() + "> ["
						+ subfieldIt->getEmbeddedInd1()
						+ subfieldIt->getEmbeddedInd2() + "]";
				}
			} else {
				/* Print regular subfield. */
				snprintf(textField, 4, " $%c ", subfieldIt->id);
				textField += subfieldIt->data;
			}
		}
	}

	return textField;
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
