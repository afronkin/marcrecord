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
#include <string>
#include <string.h>

#include "marcrecord.h"
#include "marcxml_reader.h"
#include "marcxml_reader_encodings.h"

/* XML start element handler for expat library. */
void XMLCALL marcXmlStartElement(void *userData, const XML_Char *name, const XML_Char **atts);
/* XML end element handler for expat library. */
void XMLCALL marcXmlEndElement(void *userData, const XML_Char *name);
/* XML character data handler for expat library. */
void XMLCALL marcXmlCharacterData(void *userData, const XML_Char *s, int len);

/*
 * XML start element handler for expat library.
 */
void XMLCALL marcXmlStartElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	struct MarcXmlReader::XmlParserState *parserState =
		(struct MarcXmlReader::XmlParserState *) userData;

	/* Select MARCXML element. */
	if (strcmp(name, "record") == 0 && parserState->parentTag == "") {
		/* Set parent tag. */
		parserState->parentTag = name;
	} else if (strcmp(name, "controlfield") == 0 && parserState->parentTag == "record") {
		/* Get attribute 'tag' for control field. */
		char *tag = (char *) "";

		for (int i = 0; atts[i]; i += 2) {
			if (strcmp(atts[i], "tag") == 0) {
				tag = (char *) atts[i + 1];
			}
		}

		/* Add control field to the record. */
		parserState->fieldIt = parserState->record->addControlField(tag);
		/* Set parent tag. */
		parserState->parentTag = name;
	} else if (strcmp(name, "datafield") == 0 && parserState->parentTag == "record") {
		/* Get attributes 'tag', 'ind1, 'ind2' for data field. */
		char *tag = (char *) "";
		char ind1 = ' ', ind2 = ' ';

		for (int i = 0; atts[i]; i += 2) {
			if (strcmp(atts[i], "tag") == 0) {
				tag = (char *) atts[i + 1];
			} else if (strcmp(atts[i], "ind1") == 0) {
				ind1 = atts[i + 1][0];
			} else if (strcmp(atts[i], "ind2") == 0) {
				ind2 = atts[i + 1][0];
			}
		}

		/* Add data field to the record. */
		parserState->fieldIt = parserState->record->addDataField(tag, ind1, ind2);
		/* Set parent tag. */
		parserState->parentTag = name;
	} else if (strcmp(name, "subfield") == 0 && parserState->parentTag == "datafield") {
		/* Get attribute 'code' for subfield. */
		char subfieldId = ' ';

		for (int i = 0; atts[i]; i += 2) {
			if (strcmp(atts[i], "code") == 0) {
				subfieldId = atts[i + 1][0];
			}
		}

		/* Add subfield to the data field. */
		parserState->subfieldIt = parserState->fieldIt->addSubfield(subfieldId);
		/* Set parent tag. */
		parserState->parentTag = name;
	}

	/* Store tag name in array. */
	// parserState->tags.push_back(name);
	/* Clear character data. */
	parserState->characterData.erase();
}

/*
 * XML end element handler for expat library.
 */
void XMLCALL marcXmlEndElement(void *userData, const XML_Char *name)
{
	struct MarcXmlReader::XmlParserState *parserState =
		(struct MarcXmlReader::XmlParserState *) userData;

	/* Check if start and end tags are equal. */
	if (parserState->parentTag != name) {
		return;
	}

	/* Select MARCXML element. */
	if (strcmp(name, "record") == 0) {
		/* Restore parent tag. */
		parserState->parentTag = "";
		/* Pause parser. */
		parserState->paused = true;
		XML_StopParser(parserState->xmlParser, XML_TRUE);
	} else if (strcmp(name, "leader") == 0) {
		/* Restore parent tag. */
		parserState->parentTag = "record";
		/* Set record leader. */
		parserState->record->setLeader(parserState->characterData);
	} else if (strcmp(name, "controlfield") == 0) {
		/* Restore parent tag. */
		parserState->parentTag = "record";
		/* Set data of control field. */
		parserState->fieldIt->setData(parserState->characterData);
	} else if (strcmp(name, "datafield") == 0) {
		/* Restore parent tag. */
		parserState->parentTag = "record";
	} else if (strcmp(name, "subfield") == 0) {
		/* Restore parent tag. */
		parserState->parentTag = "datafield";
		/* Set data of subfield. */
		parserState->subfieldIt->setData(parserState->characterData);
	}
}

/*
 * XML character data handler for expat library.
 */
void XMLCALL marcXmlCharacterData(void *userData, const XML_Char *s, int len)
{
	struct MarcXmlReader::XmlParserState *parserState =
		(struct MarcXmlReader::XmlParserState *) userData;

	parserState->characterData.append(s, len);
}

/*
 * XML unknown encoding handler for expat library.
 */
static int XMLCALL marcXmlUnknownEncoding(void *data, const XML_Char *encoding, XML_Encoding *info)
{
	(void) (data);

	if (strcmp(encoding, "windows-1251") == 0 || strcmp(encoding, "WINDOWS-1251") == 0) {
		/* memcpy(info->map, expatEncodingWindows1251, sizeof(int) * 256); */
		for (int i = 0; i < 256; i++) {
			info->map[i] = i;
		}

		info->data = NULL;
		info->convert = NULL;
		info->release = NULL;

		return XML_STATUS_OK;
	}

	return XML_STATUS_ERROR;
}

/*
 * Constructor.
 */
MarcXmlReader::MarcXmlReader(FILE *inputFile, const char *inputEncoding)
{
	/* Initialize input stream parameters. */
	this->inputFile = inputFile == NULL ? stdin : inputFile;
	this->inputEncoding = inputEncoding == NULL ? "" : inputEncoding;

	/* Create XML parser. */
	xmlParser = XML_ParserCreate(inputEncoding);
	XML_SetUserData(xmlParser, &parserState);
	XML_SetElementHandler(xmlParser, marcXmlStartElement, marcXmlEndElement);
	XML_SetCharacterDataHandler(xmlParser, marcXmlCharacterData);
	XML_SetUnknownEncodingHandler(xmlParser, marcXmlUnknownEncoding, NULL);

	/* Initialize XML parser state. */
	parserState.xmlParser = xmlParser;
	parserState.done = false;
	parserState.paused = false;
	parserState.parentTag = "";
	parserState.record = NULL;
	parserState.characterData.erase();
}

/*
 * Destructor.
 */
MarcXmlReader::~MarcXmlReader()
{
	/* Free XML parser. */
	XML_ParserFree(xmlParser);
}

/*
 * Read next record from MARCXML file.
 */
bool MarcXmlReader::next(MarcRecord &record)
{
	enum XML_Status parserResult;

	/* Clear record and initialize record pointer. */
	record.clear();
	parserState.record = &record;

	/* Parse MARCXML file. */
	do {
		if (parserState.paused) {
			/* Resume stopped parser. */
			parserState.paused = false;
			parserResult = XML_ResumeParser(xmlParser);
		} else {
			/* Read and parse buffer from file. */
			size_t dataLength = (int) fread(buffer, 1, sizeof(buffer), inputFile);
			parserState.done = dataLength < sizeof(buffer);
			parserResult = XML_Parse(xmlParser, buffer, dataLength, parserState.done);
		}

		/* Handle parser errors. */
		if (parserResult == XML_STATUS_ERROR) {
			record.clear();
			parserState.parentTag = "";
			throw Exception(Exception::ERROR_XML,
				XML_ErrorString(XML_GetErrorCode(xmlParser)));
		}
	} while (!parserState.done && !parserState.paused);

	/* Finish if parser wasn't paused (means there is no more tags 'record'). */
	if (!parserState.paused) {
		return false;
	}

	return true;
}
