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

#include <locale.h>
#include <stdio.h>
#include <wchar.h>

#include "marcrecord.h"
#include "marcxml_reader.h"
#include "marcxml_writer.h"

/*
 * Main function.
 */
int main(int argc, char *argv[])
{
	FILE *marcFile = NULL;

	// setlocale(LC_CTYPE, "en_US.UTF-8");

	/* Parse arguments. */
	if (argc < 2) {
		fprintf(stdout, "Error: source ISO 2709 file must be specified.\n");
		return 1;
	}
	const char *marcFileName = argv[1];

	try {
		/* Open records file. */
		marcFile = fopen(marcFileName, "rb");
		if (marcFile == NULL) {
			fprintf(stdout, "Error: can't open file '%s'.\n", marcFileName);
			throw 1;
		}

		/* Read record. */
		MarcRecord marcRecord(MarcRecord::UNIMARC);
		if (marcRecord.readIso2709(marcFile) != true) {
			fprintf(stdout, "Error: can't read file '%s'.\n", marcFileName);
			throw 1;
		}

		/* Close records file. */
		fclose(marcFile);

		{
			printf("Testing 'toString()'.\n");

			std::string textRecord = marcRecord.toString();
			printf("%s", textRecord.c_str());

			printf("Done.\n\n");
		}

		{
			printf("Testing 'getLeader()'.\n");

			MarcRecord::Leader recordLeader = marcRecord.getLeader();
			printf("Record status: %c\n", recordLeader.recordStatus);
			printf("Record type: %c\n", recordLeader.recordType);
			printf("Bibliographic level: %c\n", recordLeader.bibliographicLevel);

			printf("Done.\n\n");
		}

		{
			printf("Testing 'setLeader()'.\n");

			MarcRecord::Leader recordLeader = marcRecord.getLeader();
			recordLeader.recordStatus = 'x';
			recordLeader.recordType = 'y';
			recordLeader.bibliographicLevel = 'z';
			marcRecord.setLeader(recordLeader);

			MarcRecord::Leader newRecordLeader = marcRecord.getLeader();
			printf("Record status: %c\n", newRecordLeader.recordStatus);
			printf("Record type: %c\n", newRecordLeader.recordType);
			printf("Bibliographic level: %c\n", newRecordLeader.bibliographicLevel);

			printf("Done.\n\n");
		}

		{
			printf("Testing 'getFields()', 'getSubfields()'.\n");

			MarcRecord::FieldRefList fieldList = marcRecord.getFields("801");
			for (MarcRecord::FieldRefIt fieldIt = fieldList.begin();
				fieldIt != fieldList.end(); fieldIt++)
			{
				MarcRecord::SubfieldRefList subfieldList =
					(*fieldIt)->getSubfields('b');
				if (!subfieldList.empty()) {
					MarcRecord::SubfieldRefIt subfieldIt =
						subfieldList.begin();
					printf("%3s [%c%c] $%c %s\n",
						(*fieldIt)->tag.c_str(),
						(*fieldIt)->ind1, (*fieldIt)->ind2,
						(*subfieldIt)->id, (*subfieldIt)->data.c_str());
					// (*fieldIt)->tag = "999";
					// (*subfieldIt)->data = "456";
				}
			}

			printf("Done.\n\n");
		}

		{
			printf("Testing 'getField()', 'getSubfield()'.\n");

			MarcRecord::FieldIt fieldIt = marcRecord.getField("210");
			if (fieldIt == marcRecord.nullField()) {
				throw "field not found";
			}

			MarcRecord::SubfieldIt subfieldIt = fieldIt->getSubfield('d');
			if (subfieldIt == fieldIt->nullSubfield()) {
				throw "subfield not found";
			}

			printf("Subfield 210d: '%s'\n",
				subfieldIt->data.c_str());

			printf("Done.\n\n");
		}

		{
			printf("Testing 'getEmbeddedFields()'.\n");
			MarcRecord::FieldIt fieldIt = marcRecord.getField("461");
			MarcRecord::EmbeddedFieldList embeddedFieldList =
				fieldIt->getEmbeddedFields("801");
			printf("Found embedded fields: %u\n",
				(unsigned int) embeddedFieldList.size());
			for (MarcRecord::EmbeddedFieldIt subfieldList = embeddedFieldList.begin();
				subfieldList != embeddedFieldList.end();
				subfieldList++)
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
			printf("Done.\n\n");
		}

		{
			printf("Testing 'getEmbeddedField()', 'getEmbeddedData()'.\n");

			MarcRecord::FieldIt fieldIt = marcRecord.getField("461");
			MarcRecord::SubfieldRefList subfieldList =
				fieldIt->getEmbeddedField("001");
			if (subfieldList.empty() == true) {
				throw "embedded field not found";
			}
			for (MarcRecord::SubfieldRefIt subfieldIt = subfieldList.begin();
				subfieldIt != subfieldList.end();
				subfieldIt++)
			{
				printf("Embedded field 461 <001>: '%s'\n",
					(*subfieldIt)->getEmbeddedData().c_str());
			}

			printf("Done.\n\n");
		}

		{
			printf("Testing 'addField()', 'addFieldBefore()', "
				"'addSubfield()', 'addSubfieldBefore()'.\n");

			MarcRecord newMarcRecord(MarcRecord::UNIMARC);

			MarcRecord::FieldIt fieldIt = newMarcRecord.addField(
				MarcRecord::Field("997", '3', '4'));
			MarcRecord::SubfieldIt subfieldIt = fieldIt->addSubfield(
				MarcRecord::Subfield('c', "ccc"));
			fieldIt->addSubfieldBefore(subfieldIt, MarcRecord::Subfield('b', "bbb"));
			fieldIt = newMarcRecord.addFieldBefore(fieldIt,
				MarcRecord::Field("996", '1', '2'));
			fieldIt->addSubfield(MarcRecord::Subfield('a', "aaa"));

			fieldIt = newMarcRecord.addDataField("999", '3', '4');
			subfieldIt = fieldIt->addSubfield('n', "nnn");
			fieldIt->addSubfieldBefore(subfieldIt, 'm', "mmm");
			fieldIt = newMarcRecord.addDataFieldBefore(fieldIt, "998", '1', '2');
			fieldIt->addSubfield(MarcRecord::Subfield('l', "lll"));

			std::string textRecord = newMarcRecord.toString();
			printf("%s", textRecord.c_str());

			printf("Done.\n\n");
		}

		{
			printf("Testing 'removeField()', 'removeSubfield()'.\n");

			MarcRecord newMarcRecord(MarcRecord::UNIMARC);
			MarcRecord::FieldIt fieldIt;
			MarcRecord::SubfieldIt subfieldIt;

			fieldIt = newMarcRecord.addDataField("901", '1', '1');
			fieldIt->addSubfield('a', "aaa");
			subfieldIt = fieldIt->addSubfield('b', "bbb");
			fieldIt->addSubfield('c', "ccc");
			fieldIt->removeSubfield(subfieldIt);

			fieldIt = newMarcRecord.addDataField("902", '2', '2');
			fieldIt->addSubfield('d', "ddd");
			newMarcRecord.removeField(fieldIt);

			std::string textRecord = newMarcRecord.toString();
			printf("%s", textRecord.c_str());

			printf("Done.\n\n");
		}

		{
			printf("Testing 'writeIso2709()'.\n");

			FILE *outputFile = fopen("test.iso", "wb");
			if (outputFile == NULL) {
				throw "can't open destination file";
			}

			MarcRecord newMarcRecord(MarcRecord::UNIMARC);
			MarcRecord::FieldIt fieldIt;

			newMarcRecord.clear();
			fieldIt = newMarcRecord.addControlField("001", "12345");
			fieldIt = newMarcRecord.addDataField("200", '0', '1');
			fieldIt->addSubfield('a', "abc");
			fieldIt->addSubfield('b', "defg");
			fieldIt = newMarcRecord.addDataField("899", '2', '3');
			fieldIt->addSubfield('c', "123");
			fieldIt->addSubfield('d', "12345");
			fieldIt->addSubfield('e', "1234567");
			fieldIt = newMarcRecord.addDataField("899", '2', '3');
			fieldIt->addSubfield('c', "987");
			fieldIt->addSubfield('d', "98765");
			fieldIt->addSubfield('e', "9876543");
			newMarcRecord.writeIso2709(outputFile, "utf-8");

			newMarcRecord.clear();
			fieldIt = newMarcRecord.addControlField("001", "abcde");
			fieldIt = newMarcRecord.addDataField("201", '0', '1');
			fieldIt->addSubfield('a', "123");
			fieldIt->addSubfield('b', "456");
			newMarcRecord.writeIso2709(outputFile, "utf-8");

			fclose(outputFile);

			printf("Done.\n\n");
		}

		{
			printf("Testing 'MarcXmlWriter'.\n");

			marcFile = fopen("test.xml", "wb");
			if (marcFile == NULL) {
				throw "can't open MARCXML file";
			}

			MarcXmlWriter marcXmlWriter(marcFile, "WINDOWS-1251");
			marcXmlWriter.writeHeader();
			marcXmlWriter.write(marcRecord);
			marcXmlWriter.writeFooter();

			fclose(marcFile);

			printf("Done.\n\n");
		}

		{
			printf("Testing 'MarcXmlReader'.\n");

			marcFile = fopen("test.xml", "rb");
			if (marcFile == NULL) {
				throw "can't open MARCXML file";
			}

			MarcXmlReader marcXmlReader(marcFile);
			MarcRecord marcRecord;
			while (marcXmlReader.next(marcRecord)) {
				printf("%s", marcRecord.toString().c_str());
			}

			fclose(marcFile);

			printf("Done.\n\n");
		}
	} catch (const char *errorMessage) {
		if (marcFile != NULL)
			fclose(marcFile);

		fprintf(stdout, "Error: %s.\n", errorMessage);
		return 1;
	} catch (MarcXmlReader::Exception &e) {
		fprintf(stdout, "Error: %s.\n", e.errorMessage.c_str());
		return 1;
	}

	// fwprintf(stderr, L"Done.\n");
	printf("Done.\n");

	return 0;
}
