/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * id3_header.h - header file for MP3 Fileheader handling
 *
 * This file provides the prototypes for functions that are needed
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

#ifndef __ZENCP_ID3_HEADER_H
#define __ZENCP_ID3_HEADER_H

#include <id3.h>
#include <id3/tag.h>

extern "C" {

	/**
	 * id3_get_time() returns the length of an MP3 file, i.e. the playtime.
	 * This information is very important for the Creative MP3 player. It will
	 * return 0 if the time cannot be determined. Is there a better way to get
	 * this information?
	 */
	unsigned int id3_get_time(ID3Tag *t);

	/**
	 * id3_get_frequency() returns the sample frequency of the MP3 file.
	 * This information is not crucial but since we are here...
	 */
	unsigned int id3_get_frequency(ID3Tag *t);

	/**
	 * id3_get_bitrate() return the bitrate that has been used to encode the
	 * MP3 file. Again, not very important and even returns useless results for
	 * VBR files. But since we are here...
	 */
	unsigned int id3_get_bitrate(ID3Tag *t);
}

#endif
