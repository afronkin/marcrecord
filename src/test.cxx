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

#include <stdio.h>
#include "marcrecord.h"
#include "marc_reader.h"
#include "marc_writer.h"
#include "marcxml_reader.h"
#include "marcxml_writer.h"

/*
 * Create test MARC record 1.
 */
MarcRecord createRecord1(void)
{
	MarcRecord record(MarcRecord::UNIMARC);
	MarcRecord::FieldIt fieldIt;
	fieldIt = record.addControlField("001", "12345");
	fieldIt = record.addDataField("200", '0', '1');
	fieldIt->addSubfield('a', "abc");
	fieldIt->addSubfield('b', "defg");
	fieldIt = record.addDataField("461", '1', '0');
	fieldIt->addSubfield('1', "001abc");
	fieldIt->addSubfield('1', "005999999.9");
	fieldIt->addSubfield('1', "20056");
	fieldIt->addSubfield('a', "abc");
	fieldIt->addSubfield('b', "def");
	fieldIt->addSubfield('c', "ghi");
	fieldIt->addSubfield('1', "80165");
	fieldIt->addSubfield('x', "klm");
	fieldIt->addSubfield('y', "nop");
	fieldIt->addSubfield('1', "80156");
	fieldIt->addSubfield('x', "mlk");
	fieldIt->addSubfield('y', "pon");
	fieldIt = record.addDataField("463", '1', '0');
	fieldIt->addSubfield('1', "001xyz");
	fieldIt->addSubfield('1', "30012");
	fieldIt->addSubfield('a', "ijk");
	fieldIt->addSubfield('b', "lmn");
	fieldIt = record.addDataField("899", '2', '3');
	fieldIt->addSubfield('c', "123");
	fieldIt->addSubfield('d', "12345");
	fieldIt->addSubfield('e', "1234567");
	fieldIt->addSubfield('e', "7654321");
	fieldIt = record.addDataField("899", '2', '3');
	fieldIt->addSubfield('c', "987");
	fieldIt->addSubfield('d', "98765");
	fieldIt->addSubfield('e', "9876543");
	fieldIt->addSubfield('e', "3456789");

	return record;
}

/*
 * Create test MARC record 2.
 */
MarcRecord createRecord2(void)
{
	MarcRecord record(MarcRecord::UNIMARC);
	MarcRecord::FieldIt fieldIt;
	fieldIt = record.addControlField("001", "abcde");
	fieldIt = record.addDataField("201", '0', '1');
	fieldIt->addSubfield('a', "123");
	fieldIt->addSubfield('b', "456");

	return record;
}

bool test1(void)
{
	printf("Testing addField(), addFieldBefore(), addSubfield(), addSubfieldBefore().\n");

	try {
		/* Create new MARC record. */
		MarcRecord record(MarcRecord::UNIMARC);

		/* Add fields and subfields to new record. */
		MarcRecord::FieldIt fieldIt = record.addField(MarcRecord::Field("997", '3', '4'));
		MarcRecord::SubfieldIt subfieldIt = fieldIt->addSubfield(
			MarcRecord::Subfield('c', "ccc"));
		fieldIt->addSubfieldBefore(subfieldIt, MarcRecord::Subfield('b', "bbb"));
		fieldIt = record.addFieldBefore(fieldIt, MarcRecord::Field("996", '1', '2'));
		fieldIt->addSubfield(MarcRecord::Subfield('a', "aaa"));

		fieldIt = record.addDataField("999", '3', '4');
		subfieldIt = fieldIt->addSubfield('n', "nnn");
		fieldIt->addSubfieldBefore(subfieldIt, 'm', "mmm");
		fieldIt = record.addDataFieldBefore(fieldIt, "998", '1', '2');
		fieldIt->addSubfield(MarcRecord::Subfield('l', "lll"));
	
		/* Print content of new record. */
		printf("%s\n", record.toString().c_str());
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test2(void)
{
	printf("Testing toString().\n");

	try {
		MarcRecord record = createRecord1();

		/* Convert MARC record to string. */
		printf("%s\n", record.toString().c_str());
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test3(void)
{
	printf("Testing setLeader(), getLeader().\n");

	try {
		MarcRecord record = createRecord1();

		/* Print original leader fields. */
		MarcRecord::Leader leader = record.getLeader();
		printf("Record status: %c\n", leader.recordStatus);
		printf("Record type: %c\n", leader.recordType);
		printf("Bibliographic level: %c\n---\n", leader.bibliographicLevel);

		/* Modify some fields in leader. */
		leader.recordStatus = 'x';
		leader.recordType = 'y';
		leader.bibliographicLevel = 'z';
		record.setLeader(leader);

		/* Print modified leader fields. */
		leader = record.getLeader();
		printf("Record status: %c\n", leader.recordStatus);
		printf("Record type: %c\n", leader.recordType);
		printf("Bibliographic level: %c\n", leader.bibliographicLevel);
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test4(void)
{
	printf("Testing getField(), getSubfield().\n");

	try {
		MarcRecord record = createRecord1();

		/* Get specified field. */
		MarcRecord::FieldIt fieldIt = record.getField("200");
		if (fieldIt == record.nullField()) {
			throw "field not found";
		}

		/* Get specified subfield from field. */
		MarcRecord::SubfieldIt subfieldIt = fieldIt->getSubfield('b');
		if (subfieldIt == fieldIt->nullSubfield()) {
			throw "subfield not found";
		}

		/* Print subfield value. */
		printf("Subfield 200b: '%s'\n", subfieldIt->data.c_str());
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test5(void)
{
	printf("Testing getFields(), getSubfields().\n");

	try {
		MarcRecord record = createRecord1();

		/* Get list of specified fields. */
		MarcRecord::FieldRefList fieldList = record.getFields("899");
		for (MarcRecord::FieldRefIt fieldIt = fieldList.begin();
			fieldIt != fieldList.end(); fieldIt++)
		{
			/* Print tag and indicators of field. */
			printf("%3s [%c%c]", (*fieldIt)->tag.c_str(),
				(*fieldIt)->ind1, (*fieldIt)->ind2);

			/* Get list of specified subfields from field. */
			MarcRecord::SubfieldRefList subfieldList = (*fieldIt)->getSubfields('e');
			if (!subfieldList.empty()) {
				/* Print values of subfields. */
				for (MarcRecord::SubfieldRefIt subfieldIt = subfieldList.begin();
					subfieldIt != subfieldList.end();  subfieldIt++)
				{
					printf(" $%c %s",
						(*subfieldIt)->id, (*subfieldIt)->data.c_str());
				}
				printf("\n");
			}
		}
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test6(void)
{
	printf("Testing getEmbeddedField(), getEmbeddedData().\n");

	try {
		MarcRecord record = createRecord1();

		/* Get specified field. */
		MarcRecord::FieldIt fieldIt = record.getField("463");
		if (fieldIt == record.nullField()) {
			throw "field not found";
		}

		/* Get specified embedded control field. */
		MarcRecord::SubfieldRefList subfieldList = fieldIt->getEmbeddedField("001");
		if (subfieldList.empty()) {
			throw "embedded field not found";
		}

		/* Print subfields of embedded control field*/
		for (MarcRecord::SubfieldRefIt subfieldIt = subfieldList.begin();
			subfieldIt != subfieldList.end(); subfieldIt++)
		{
			printf("Embedded field 461 <001>: '%s'\n",
				(*subfieldIt)->getEmbeddedData().c_str());
		}
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test7(void)
{
	printf("Testing getEmbeddedFields().\n");

	try {
		MarcRecord record = createRecord1();

		/* Get specified field. */
		MarcRecord::FieldIt fieldIt = record.getField("461");

		/* Get list of specified embedded fields. */
		MarcRecord::EmbeddedFieldList embeddedFieldList = fieldIt->getEmbeddedFields("801");
		printf("Found embedded fields: %u\n", (unsigned int) embeddedFieldList.size());
		for (MarcRecord::EmbeddedFieldIt subfieldList = embeddedFieldList.begin();
			subfieldList != embeddedFieldList.end(); subfieldList++)
		{
			printf("Embedded field 461 <801>:");
			for (MarcRecord::SubfieldRefIt subfieldIt = subfieldList->begin();
				subfieldIt != subfieldList->end(); subfieldIt++)
			{
				printf(" $%c '%s'",
					(*subfieldIt)->id, (*subfieldIt)->data.c_str());
			}
			printf("\n");
		}
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test8(void)
{
	printf("Testing removeField(), removeSubfield().\n");

	try {
		MarcRecord record = createRecord1();

		/* Get specified field. */
		MarcRecord::FieldIt fieldIt = record.getField("461");
		if (fieldIt == record.nullField()) {
			throw "field not found";
		}

		/* Remove field. */
		record.removeField(fieldIt);

		/* Get specified subfield. */
		fieldIt = record.getField("899");
		if (fieldIt == record.nullField()) {
			throw "field not found";
		}
		MarcRecord::SubfieldIt subfieldIt = fieldIt->getSubfield('c');
		if (subfieldIt == fieldIt->nullSubfield()) {
			throw "subfield not found";
		}

		/* Remove subfield. */
		fieldIt->removeSubfield(subfieldIt);

		/* Print content of record. */
		printf("%s\n", record.toString().c_str());
	} catch (const char *errorMessage) {
		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test9(void)
{
	FILE *outputFile = NULL;

	printf("Testing MarcWriter.\n");

	try {
		/* Open ISO 2709 file. */
		outputFile = fopen("test1.iso", "wb");
		if (outputFile == NULL) {
			throw "can't open output file";
		}

		/* Initialize MARC writer. */
		MarcWriter marcWriter(outputFile);

		/* Write MARC records to ISO 2709 file. */
		MarcRecord record1 = createRecord1();
		MarcRecord record2 = createRecord2();
		if (!marcWriter.write(record1) || !marcWriter.write(record2)) {
			throw "can't write MARC record to file";
		}

		/* Close ISO 2709 file. */
		fclose(outputFile);
	} catch (const char *errorMessage) {
		/* Close files. */
		if (outputFile) {
			fclose(outputFile);
		}

		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test10(void)
{
	FILE *inputFile = NULL;

	printf("Testing MarcReader.\n");

	try {
		/* Open ISO 2709 file. */
		inputFile = fopen("test1.iso", "rb");
		if (inputFile == NULL) {
			throw "can't open input file";
		}

		/* Initialize MARC reader. */
		MarcReader marcReader(inputFile);

		/* Read record. */
		MarcRecord record(MarcRecord::UNIMARC);
		if (!marcReader.next(record)) {
			throw "can't read MARC record from file";
		}

		/* Close ISO 2709 file. */
		fclose(inputFile);
	} catch (const char *errorMessage) {
		/* Close files. */
		if (inputFile) {
			fclose(inputFile);
		}

		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test11(void)
{
	FILE *outputFile = NULL;

	printf("Testing MarcXmlWriter.\n");

	try {
		/* Open MARCXML file. */
		outputFile = fopen("test2.xml", "wb");
		if (outputFile == NULL) {
			throw "can't open output file";
		}

		/* Initialize MARC writer. */
		MarcXmlWriter marcXmlWriter(outputFile, "WINDOWS-1251");

		/* Write MARC records to ISO 2709 file. */
		MarcRecord record1 = createRecord1();
		MarcRecord record2 = createRecord2();
		marcXmlWriter.writeHeader();
		if (!marcXmlWriter.write(record1) || !marcXmlWriter.write(record2)) {
			throw "can't write MARCXML record to file";
		}
		marcXmlWriter.writeFooter();

		/* Close ISO 2709 file. */
		fclose(outputFile);
	} catch (const char *errorMessage) {
		/* Close files. */
		if (outputFile) {
			fclose(outputFile);
		}

		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

bool test12(void)
{
	FILE *inputFile = NULL;

	printf("Testing MarcXmlReader.\n");

	try {
		/* Open ISO 2709 file. */
		inputFile = fopen("test2.xml", "rb");
		if (inputFile == NULL) {
			throw "can't open input file";
		}

		/* Initialize MARCXML reader. */
		MarcXmlReader marcXmlReader(inputFile);

		/* Read records. */
		MarcRecord record(MarcRecord::UNIMARC);
		while (marcXmlReader.next(record)) {
			printf("%s\n", record.toString().c_str());
		}

		/* Close ISO 2709 file. */
		fclose(inputFile);
	} catch (const char *errorMessage) {
		/* Close files. */
		if (inputFile) {
			fclose(inputFile);
		}

		/* Print error message. */
		printf("Error: %s.\n", errorMessage);

		return false;
	}

	/* Print status. */
	printf("ok\n\n");

	return true;
}

/*
 * Main function.
 */
int main(void)
{
	/* Execute tests. */
	if (!test1() || !test2() || !test3() || !test4() || !test5() || !test6() || !test7()
		|| !test8() || !test9() || !test10() || !test11() || !test12())
	{
		printf("Tests failed.\n");
		return 1;
	}

	// fwprintf(stderr, L"Done.\n");
	printf("Done.\n");

	return 0;
}
