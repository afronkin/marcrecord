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

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include "marcrec.h"

/*
 * Main function.
 */
int main(int argc, char *argv[])
{
	FILE *marcFile = NULL;

	// setlocale(LC_CTYPE, "en_US.UTF-8");

	/* Parse arguments. */
	if (argc < 2) {
		fprintf(stdout, "Error: MARC file must be specified.\n");
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
		if (marcRecord.read(marcFile) != true) {
			fprintf(stdout, "Error: can't read file '%s'.\n", marcFileName);
			throw 1;
		}

		/* Testing 'toString()'. */
		{
			printf("Testing 'toString()'.\n");

			std::string textRecord = marcRecord.toString();
			printf("%s", textRecord.c_str());

			printf("Done.\n\n");
		}

		/* Testing getFields(), getSubfields(). */
		{
			printf("Testing 'getFields()', 'getSubfields()'.\n");

			MarcRecord::FieldPtrList fieldList = marcRecord.getFields("801");
			for (MarcRecord::FieldPtrRef fieldRef = fieldList.begin();
				fieldRef != fieldList.end();
				fieldRef++)
			{
				MarcRecord::SubfieldPtrList subfieldList =
					(*fieldRef)->getSubfields('b');
				if (!subfieldList.empty()) {
					MarcRecord::SubfieldPtrRef subfieldRef =
						subfieldList.begin();
					printf("%3s [%c%c] $%c %s\n",
						(*fieldRef)->tag.c_str(),
						(*fieldRef)->ind1, (*fieldRef)->ind2,
						(*subfieldRef)->id, (*subfieldRef)->data.c_str());
// (*fieldRef)->tag = "999";
// (*subfieldRef)->data = "456";
				}
			}

			printf("Done.\n\n");
		}

		/* Testing getField(), getSubfield(). */
		{
			printf("Testing 'getField()', 'getSubfield()'.\n");

			MarcRecord::FieldRef fieldRef = marcRecord.getField("210");
			if (fieldRef == marcRecord.nullField()) {
				throw "field not found";
			}

			MarcRecord::SubfieldRef subfieldRef = fieldRef->getSubfield('d');
			if (subfieldRef == fieldRef->nullSubfield()) {
				throw "subfield not found";
			}

			printf("Subfield 210d: '%s'\n",
				subfieldRef->data.c_str());

			printf("Done.\n\n");
		}

		/* Testing getEmbeddedFields(). */
		{
			printf("Testing 'getEmbeddedFields()'.\n");
			MarcRecord::FieldRef fieldRef = marcRecord.getField("461");
			MarcRecord::EmbeddedFieldList embeddedFieldList =
				fieldRef->getEmbeddedFields("801");
			printf("Found embedded fields: %d\n", embeddedFieldList.size());
			for (MarcRecord::EmbeddedFieldRef subfieldList = embeddedFieldList.begin();
				subfieldList != embeddedFieldList.end();
				subfieldList++)
			{
				printf("Embedded field 461 <801>:");
				for (MarcRecord::SubfieldPtrRef subfieldRef = subfieldList->begin();
					subfieldRef != subfieldList->end();
					subfieldRef++)
				{
					printf(" $%c '%s'",
						(*subfieldRef)->id, (*subfieldRef)->data.c_str());
				}
				printf("\n");
			}
			printf("Done.\n\n");
		}

		/* Testing 'getEmbeddedField()', 'getEmbeddedData()'. */
		{
			printf("Testing 'getEmbeddedField()', 'getEmbeddedData()'.\n");
			MarcRecord::FieldRef fieldRef = marcRecord.getField("461");
			MarcRecord::SubfieldPtrList subfieldList =
				fieldRef->getEmbeddedField("001");
			if (subfieldList.empty() == true) {
				throw "embedded field not found";
			}
			for (MarcRecord::SubfieldPtrRef subfieldRef = subfieldList.begin();
				subfieldRef != subfieldList.end();
				subfieldRef++)
			{
				printf("Embedded field 461 <001>: '%s'\n",
					(*subfieldRef)->getEmbeddedData().c_str());
			}
			printf("Done.\n\n");
		}

		fclose(marcFile);
	} catch (const char *errorMessage) {
		if (marcFile != NULL)
			fclose(marcFile);

		fprintf(stdout, "Error: %s.\n", errorMessage);
		return 1;
	}

	// fwprintf(stderr, L"Done.\n");
	printf("Done.\n");

	return 0;
}

