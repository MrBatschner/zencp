/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * id3_header.cpp - implementation file for MP3 Fileheader handling
 *
 * This file provides the implementation for functions that are needed
 * to read and handle the information that is contained in the header of
 * an MP3 file. These functions have not been wrapped from C++ to C by the
 * wrapper that is included in id3lib, therefore, it is done here.
 *
 * This file depends on a recent version of libid3. Get it from
 * <http://id3lib.sf.net>.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#include "id3_header.h"

unsigned int id3_get_time(ID3Tag *t) {
	unsigned int time = 0;
	ID3_Tag *tag = (ID3_Tag*)t;	/* cast the C ID3Tag object to a C++ ID3_Tag object - their structure is identical */
	const Mp3_Headerinfo *header = tag->GetMp3HeaderInfo();	/* obtain the header object */

	if (header) time = header->time;  /* if the header could be obtained, read the time field */
	tag = 0;
	return time;			/* and return the time */
}


/* the following functions are the same as above */
unsigned int id3_get_frequency(ID3Tag *t) {
	unsigned int f = 0;
	ID3_Tag *tag = (ID3_Tag*)t;
	const Mp3_Headerinfo *header = tag->GetMp3HeaderInfo();

	if (header) f = header->frequency;
	tag = 0;
	return f;
}


unsigned int id3_get_bitrate(ID3Tag *t) {
	unsigned int b = 0;
	ID3_Tag *tag = (ID3_Tag*)t;
	const Mp3_Headerinfo *header = tag->GetMp3HeaderInfo();

	if (header) b = header->bitrate;
	tag = 0;
	return b;
}
