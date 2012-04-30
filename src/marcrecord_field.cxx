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

/*
 * Constructor.
 */
MarcRecord::Field::Field(std::string newTag, char newInd1, char newInd2)
{
	clear();
	tag = newTag;
	ind1 = newInd1;
	ind2 = newInd2;
}

/*
 * Clear field data.
 */
void MarcRecord::Field::clear(void)
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

/*
 * Add subfield to the end of field.
 */
MarcRecord::SubfieldRef MarcRecord::Field::addSubfield(Subfield subfield)
{
	/* Append subfield to the list. */
	SubfieldRef subfieldRef = subfieldList.insert(subfieldList.end(), subfield);
	return subfieldRef;
}

/*
 * Add subfield to the field before specified subfield.
 */
MarcRecord::SubfieldRef MarcRecord::Field::addSubfieldBefore(Subfield subfield,
	SubfieldRef nextSubfieldRef)
{
	/* Append subfield to the list. */
	SubfieldRef subfieldRef = subfieldList.insert(nextSubfieldRef, subfield);
	return subfieldRef;
}
