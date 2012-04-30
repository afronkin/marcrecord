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
MarcRecord::SubfieldRefList MarcRecord::Field::getSubfields(char subfieldId)
{
	SubfieldRefList resultSubfieldList;
	SubfieldIt subfieldIt;

	/* Check subfields in list. */
	for (subfieldIt = subfieldList.begin(); subfieldIt != subfieldList.end(); subfieldIt++) {
		if (subfieldId == ' ' || subfieldIt->id == subfieldId) {
			resultSubfieldList.push_back(subfieldIt);
		}
	}

	return resultSubfieldList;
}

/*
 * Get subfield.
 */
MarcRecord::SubfieldIt MarcRecord::Field::getSubfield(char subfieldId)
{
	SubfieldIt subfieldIt;

	/* Check subfields in list. */
	for (subfieldIt = subfieldList.begin(); subfieldIt != subfieldList.end(); subfieldIt++) {
		if (subfieldId == ' ' || subfieldIt->id == subfieldId) {
			return subfieldIt;
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
	SubfieldRefList embeddedSubfieldList;

	/* Check subfields in list. */
	embeddedSubfieldList.clear();
	for (SubfieldIt subfieldIt = subfieldList.begin();
		subfieldIt != subfieldList.end(); subfieldIt++)
	{
		if (subfieldIt->id == '1') {
			if (embeddedSubfieldList.empty() == false) {
				/* Append embedded field to the result list. */
				resultFieldList.push_back(embeddedSubfieldList);
				embeddedSubfieldList.clear();
			}

			if (fieldTag == "" || fieldTag == subfieldIt->getEmbeddedTag()) {
				/* Append first subfield of embedded field. */
				embeddedSubfieldList.push_back(subfieldIt);
			}
		} else if (embeddedSubfieldList.empty() == false) {
			/* Append subfield to the embedded field. */
			embeddedSubfieldList.push_back(subfieldIt);
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
MarcRecord::SubfieldRefList MarcRecord::Field::getEmbeddedField(std::string fieldTag)
{
	SubfieldRefList embeddedSubfieldList;

	/* Check subfields in list. */
	embeddedSubfieldList.clear();
	for (SubfieldIt subfieldIt = subfieldList.begin();
		subfieldIt != subfieldList.end(); subfieldIt++)
	{
		if (subfieldIt->id == '1') {
			if (embeddedSubfieldList.empty() == false) {
				break;
			}

			if (fieldTag == "" || fieldTag == subfieldIt->getEmbeddedTag()) {
				/* Append first subfield of embedded field. */
				embeddedSubfieldList.push_back(subfieldIt);
			}
		} else if (embeddedSubfieldList.empty() == false) {
			/* Append subfield to the embedded field. */
			embeddedSubfieldList.push_back(subfieldIt);
		}
	}

	return embeddedSubfieldList;
}

/*
 * Add subfield to the end of field.
 */
MarcRecord::SubfieldIt MarcRecord::Field::addSubfield(Subfield subfield)
{
	/* Append subfield to the list. */
	SubfieldIt subfieldIt = subfieldList.insert(subfieldList.end(), subfield);
	return subfieldIt;
}

MarcRecord::SubfieldIt MarcRecord::Field::addSubfield(char subfieldId, std::string subfieldData)
{
	/* Append subfield to the list. */
	SubfieldIt subfieldIt = subfieldList.insert(subfieldList.end(),
		Subfield(subfieldId, subfieldData));
	return subfieldIt;
}

/*
 * Add subfield to the field before specified subfield.
 */
MarcRecord::SubfieldIt MarcRecord::Field::addSubfieldBefore(SubfieldIt nextSubfieldIt,
	Subfield subfield)
{
	/* Append subfield to the list. */
	SubfieldIt subfieldIt = subfieldList.insert(nextSubfieldIt, subfield);
	return subfieldIt;
}

MarcRecord::SubfieldIt MarcRecord::Field::addSubfieldBefore(SubfieldIt nextSubfieldIt,
	char subfieldId, std::string subfieldData)
{
	/* Append subfield to the list. */
	SubfieldIt subfieldIt = subfieldList.insert(nextSubfieldIt,
		Subfield(subfieldId, subfieldData));
	return subfieldIt;
}

/*
 * Remove subfield from the field.
 */
void MarcRecord::Field::removeSubfield(SubfieldIt subfieldIt)
{
	/* Remove subfield from the list. */
	subfieldList.erase(subfieldIt);
}
