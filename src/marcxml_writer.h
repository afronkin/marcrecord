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

#if !defined(MARCXML_WRITER_H)
#define MARCXML_WRITER_H

#include <string>
#include "marcrecord.h"

/*
 * MARCXML records writer.
 */
class MarcXmlWriter {
protected:
	/* Output MARCXML file. */
	FILE *outputFile;
	/* Encoding of output MARCXML file. */
	std::string outputEncoding;

public:
	/* Constructor. */
	MarcXmlWriter(FILE *outputFile = NULL, const char *outputEncoding = NULL);
	/* Destructor. */
	~MarcXmlWriter();

	/* Write header to MARCXML file. */
	void writeHeader(void);
	/* Write footer to MARCXML file. */
	void writeFooter(void);
	/* Write record to MARCXML file. */
	void write(MarcRecord &record);
};

#endif /* MARCXML_WRITER_H */
#endif /* MARCXML */