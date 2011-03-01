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

#include "marcrec.h"

/*
 * Clear field data.
 */
void MarcRecord::Field::clear()
{
	tag = "";
	ind1 = ' ';
	ind2 = ' ';
	data.erase();
	subfieldList.clear();
}

/*
 * Get list of subfields.
 */
MarcRecord::SubfieldPtrList MarcRecord::Field::getSubfields(char subfieldId)
{
	SubfieldPtrList resultSubfieldList;
	SubfieldRef subfieldRef;

	/* Check subfields in list. */
	for (subfieldRef = subfieldList.begin(); subfieldRef != subfieldList.end(); subfieldRef++) {
		if (subfieldId == ' ' || subfieldRef->id == subfieldId) {
			resultSubfieldList.push_back(subfieldRef);
		}
	}

	return resultSubfieldList;
}

/*
 * Get subfield.
 */
MarcRecord::SubfieldRef MarcRecord::Field::getSubfield(char subfieldId)
{
	SubfieldRef subfieldRef;

	/* Check subfields in list. */
	for (subfieldRef = subfieldList.begin(); subfieldRef != subfieldList.end(); subfieldRef++) {
		if (subfieldId == ' ' || subfieldRef->id == subfieldId) {
			return subfieldRef;
		}
	}

	return subfieldList.end();
}

/*
 * Get list of embedded fields.
 */
MarcRecord::EmbeddedFieldList MarcRecord::Field::getEmbeddedFields(std::string fieldTag)
{
	EmbeddedFieldList resultFieldList;
	SubfieldPtrList embeddedSubfieldList;

	/* Check subfields in list. */
	embeddedSubfieldList.clear();
	for (SubfieldRef subfieldRef = subfieldList.begin();
		subfieldRef != subfieldList.end(); subfieldRef++)
	{
		if (subfieldRef->id == '1') {
			if (embeddedSubfieldList.empty() == false) {
				/* Append embedded field to the result list. */
				resultFieldList.push_back(embeddedSubfieldList);
				embeddedSubfieldList.clear();
			}

			if (fieldTag == "" || fieldTag == subfieldRef->getEmbeddedTag()) {
				/* Append first subfield of embedded field. */
				embeddedSubfieldList.push_back(subfieldRef);
			}
		} else if (embeddedSubfieldList.empty() == false) {
			/* Append subfield to the embedded field. */
			embeddedSubfieldList.push_back(subfieldRef);
		}
	}

	if (embeddedSubfieldList.empty() == false) {
		/* Append embedded field to the result list. */
		resultFieldList.push_back(embeddedSubfieldList);
	}

	return resultFieldList;
}

/*
 * Get embedded field.
 */
MarcRecord::SubfieldPtrList MarcRecord::Field::getEmbeddedField(std::string fieldTag)
{
	SubfieldPtrList embeddedSubfieldList;

	/* Check subfields in list. */
	embeddedSubfieldList.clear();
	for (SubfieldRef subfieldRef = subfieldList.begin();
		subfieldRef != subfieldList.end(); subfieldRef++)
	{
		if (subfieldRef->id == '1') {
			if (embeddedSubfieldList.empty() == false) {
				break;
			}

			if (fieldTag == "" || fieldTag == subfieldRef->getEmbeddedTag()) {
				/* Append first subfield of embedded field. */
				embeddedSubfieldList.push_back(subfieldRef);
			}
		} else if (embeddedSubfieldList.empty() == false) {
			/* Append subfield to the embedded field. */
			embeddedSubfieldList.push_back(subfieldRef);
		}
	}

	return embeddedSubfieldList;
}

