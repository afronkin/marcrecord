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

#if !defined(MARC_READER_H)
#define MARC_READER_H

#include <string>
#include "marcrecord.h"

/*
 * MARC (ISO 2709) records reader.
 */
class MarcReader {
protected:
	/* Input ISO 2709 file. */
	FILE *inputFile;
	/* Encoding of input ISO 2709 file. */
	std::string inputEncoding;

private:
	/* Parse field from ISO 2709 buffer. */
	inline MarcRecord::Field parseField(const std::string &fieldTag,
		const char *fieldData, unsigned int fieldLength);

public:
	/* Constructor. */
	MarcReader(FILE *inputFile = NULL, const char *inputEncoding = NULL);
	/* Destructor. */
	~MarcReader();

	/* Read next record from MARCXML file. */
	bool next(MarcRecord &record);

	/* Parse record from ISO 2709 buffer. */
	bool parseRecord(const char *recordBuf, MarcRecord &record);
};

#endif /* MARC_READER_H */
