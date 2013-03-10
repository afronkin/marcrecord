/*
 * Copyright (c) 2013, Alexander Fronkin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MARCRECORD_SRC_MARC_READER_H
#define MARCRECORD_SRC_MARC_READER_H

#include <iconv.h>
#include <string>
#include "marcrecord.h"

namespace marcrecord {

/*
 * MARC (ISO 2709) records reader.
 */
class MarcReader {
public:
	// Error codes.
	enum ErrorCode {
		OK = 0,
		END_OF_FILE = 1,
		ERROR_INVALID_RECORD = -1,
		ERROR_ICONV = -2
	};

protected:
	// Code of last error.
	ErrorCode m_errorCode;
	// Message of last error.
	std::string m_errorMessage;

	// Input ISO 2709 file.
	FILE *m_inputFile;
	// Encoding of input ISO 2709 file.
	std::string m_inputEncoding;
	// Iconv descriptor for input encoding.
	iconv_t m_iconvDesc;

	// Automatic error correction mode.
	bool m_autoCorrectionMode;

private:
	// Parse field from ISO 2709 buffer.
	inline MarcRecord::Field parseField(const std::string &fieldTag,
		const char *fieldData, unsigned int fieldLength);
	// Parse subfield.
	MarcRecord::Subfield parseSubfield(const char *fieldData,
		unsigned int subfieldStartPos, unsigned int subfieldEndPos);

public:
	// Constructor.
	MarcReader(FILE *inputFile = NULL, const char *inputEncoding = NULL);
	// Destructor.
	~MarcReader();

	// Get last error code.
	ErrorCode getErrorCode(void);
	// Get last error message.
	std::string & getErrorMessage(void);

	// Set automatic error correction mode.
	void setAutoCorrectionMode(bool autoCorrectionMode = true);

	// Open input file.
	bool open(FILE *inputFile, const char *inputEncoding = NULL);
	// Close input file.
	void close(void);

	// Read next record from MARCXML file.
	bool next(MarcRecord &record);
	// Parse record from ISO 2709 buffer.
	bool parse(const char *recordBuf, unsigned int recordBufLen,
		MarcRecord &record);
};

} // namespace marcrecord

#endif // MARCRECORD_SRC_MARC_READER_H