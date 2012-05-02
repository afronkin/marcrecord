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

#if defined(MARCXML)

#if !defined(MARCXML_READER_H)
#define MARCXML_READER_H

#include <string>
#include "marcrecord.h"

#include <expat.h>

/*
 * MARCXML records reader.
 */
class MarcXmlReader {
public:
	/* Exception class for events and errors handling. */
	class Exception {
	public:
		enum ErrorCode { ERROR_XML } errorCode;
		std::string errorMessage;

		Exception(enum ErrorCode errorCode, std::string errorMessage)
		{
			this->errorCode = errorCode;
			this->errorMessage = errorMessage;
		}
	};

	/* XML parser state structure definition. */
	struct XmlParserState {
		XML_Parser xmlParser;
		bool done;
		bool paused;
		std::string parentTag;

		MarcRecord *record;
		MarcRecord::FieldIt fieldIt;
		MarcRecord::SubfieldIt subfieldIt;
		std::string characterData;
	};

protected:
	/* Input MARCXML file. */
	FILE *inputFile;
	/* Encoding of input MARCXML file. */
	std::string inputEncoding;

	/* XML parser. */
	XML_Parser xmlParser;
	/* XML parser state. */
	struct XmlParserState parserState;
	/* Record buffer. */
	char buffer[4096];

public:
	/* Constructor. */
	MarcXmlReader(FILE *inputFile = NULL, const char *inputEncoding = NULL);
	/* Destructor. */
	~MarcXmlReader();

	/* Read next record from MARCXML file. */
	bool next(MarcRecord &);
};

#endif /* MARCXML_READER_H */
#endif /* MARCXML */