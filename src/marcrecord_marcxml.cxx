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

#if defined(MARCRECORD_MARCXML)

#include <string>
#include "marcrecord.h"

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

	/* Store tag name in array. */
	parserState->tags.push_back(name);
	/* Clear character data. */
	parserState->characterData.erase();

	for (unsigned int i = 0; i < parserState->level; i++) {
		putchar('\t');
	}
	printf("<%s>\n", name);

	parserState->level++;
}

/*
 * XML end element handler for expat library.
 */
void XMLCALL marcXmlEndElement(void *userData, const XML_Char *name)
{
	struct MarcXmlReader::XmlParserState *parserState =
		(struct MarcXmlReader::XmlParserState *) userData;

	/* Retrieve last tag from array. */
	std::string tag = parserState->tags.back();
	parserState->tags.pop_back();

	/* Compare start and end tag names. */
	if (tag != name) {
		return;
	}

	parserState->level--;
	for (unsigned int i = 0; i < parserState->level; i++) {
		putchar('\t');
	}
	printf("</%s>\n", name);

	if (tag == "record") {
		/* Pause parser. */
		parserState->paused = true;
		XML_StopParser(parserState->xmlParser, XML_TRUE);
	} else if (tag == "leader") {
		parserState->record->setLeader(parserState->characterData);
	} else if (tag == "controlfield") {
		// parserState->record->addField();
	} else if (tag == "datafield") {
	} else if (tag == "subfield") {
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
 * Constructor.
 */
MarcXmlReader::MarcXmlReader(FILE *inputFile, const char *inputEncoding)
{
	/* Initialize input stream parameters. */
	this->inputFile = inputFile == NULL ? stdin : inputFile;
	this->inputEncoding = inputEncoding;

	/* Create XML parser. */
	xmlParser = XML_ParserCreate(NULL);
	XML_SetUserData(xmlParser, &parserState);
	XML_SetElementHandler(xmlParser, marcXmlStartElement, marcXmlEndElement);
	XML_SetCharacterDataHandler(xmlParser, marcXmlCharacterData);

	/* Initialize XML parser state. */
	parserState.xmlParser = xmlParser;
	parserState.done = false;
	parserState.paused = false;
	parserState.level = 0;
	parserState.tags.clear();
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
MarcRecord MarcXmlReader::next(void)
{
	MarcRecord record;
	enum XML_Status parserResult;

	/* Initialize record pointer. */
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
			throw Exception(Exception::ERROR_XML, "can't parse MARCXML file");
		}
	} while (!parserState.done && !parserState.paused);

	/* Return null-value record when reached end of file. */
	if (!parserState.paused) {
		record.setNull();
	}

	return record;
}

#endif /* MARCRECORD_MARCXML */
