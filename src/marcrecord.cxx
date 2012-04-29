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
 * Set record type.
 */
void MarcRecord::setType(RecordType newRecordType)
{
	recordType = newRecordType;
}

/*
 * Get list of fields.
 */
MarcRecord::FieldPtrList MarcRecord::getFields(std::string fieldTag)
{
	FieldPtrList resultFieldList;
	FieldRef fieldRef;

	/* Check fields in list. */
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
 * Get field.
 */
MarcRecord::FieldRef MarcRecord::getField(std::string fieldTag)
{
	FieldRef fieldRef;

	/* Check fields in list. */
	for (fieldRef = fieldList.begin(); fieldRef != fieldList.end();
		fieldRef++)
	{
		if (fieldTag == "" || fieldTag == fieldRef->tag) {
			return fieldRef;
		}
	}

	return fieldList.end();
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

