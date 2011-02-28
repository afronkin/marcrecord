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
	MarcRecord marcRecord(MarcRecord::UNIMARC);
	const char *marcFileName = NULL;
	FILE *marcFile = NULL;
	MarcRecord::FieldPtrList fieldList;
	MarcRecord::FieldPtrRef fieldRef;
	MarcRecord::SubfieldPtrList subfieldList;
	MarcRecord::SubfieldPtrRef subfieldRef;

	setlocale(LC_CTYPE, "en_US.UTF-8");

	/* Parse arguments. */
	if (argc < 2) {
		fwprintf(stderr, L"Error: MARC file must be specified.\n");
		return 1;
	}

	marcFileName = argv[1];

	try {
		/* Open records file. */
		marcFile = fopen(marcFileName, "rb");
		if (marcFile == NULL) {
			fprintf(stderr, "Error: can't open file '%s'.\n", marcFileName);
			throw 1;
		}

		/* Read record. */
		if (marcRecord.read(marcFile) != true) {
			fwprintf(stderr, L"Error: can't read file '%s'.\n", marcFileName);
			throw 1;
		}

		/* Print some subfields. */
		fieldList = marcRecord.getFieldList(200);
		for (fieldRef = fieldList.begin(); fieldRef != fieldList.end();
			fieldRef++)
		{
			subfieldList = marcRecord.getSubfieldList(*fieldRef, 'a');
			if (!subfieldList.empty()) {
				subfieldRef = subfieldList.begin();
				printf("%03d [%c%c] $%c %s\n",
					(*fieldRef)->tag, (*fieldRef)->ind1, (*fieldRef)->ind2,
					(*subfieldRef)->id, (*subfieldRef)->data.c_str());
(*fieldRef)->tag = 999;
(*subfieldRef)->data = "456";
			}
		}

		/* Print record. */
		std::string textRecord = marcRecord.toString();
		printf("%s\n", textRecord.c_str());

		fclose(marcFile);
	} catch (int errorCode) {
		if (marcFile != NULL)
			fclose(marcFile);

		return errorCode;
	}

	fwprintf(stderr, L"Done.\n");

	return 0;
}
