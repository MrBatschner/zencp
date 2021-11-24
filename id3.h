/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * id3.h - header file for MP3 ID3 Tag handling
 *
 * This file provides function prototypes and structures that are needed
 * to read and handle ID3 tags of MP3 files as they are required for 
 * Creative's MP3 players.
 *
 * This file depends on a recent version of libid3. Get it from
 * <http://id3lib.sf.net>.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#ifndef __ZENCP_ID3_H
#define __ZENCP_ID3_H

#include <id3.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "list.h"
#include "misc.h"

/** 
 * I don't know if I used this constant anywhere in the program
 */
#define _ID3_MAX_LEN 1024

/**
 * The default tag: this string is used if any ID3 tag of an MP3
 * file is empty. Will be converted into a character array when needed.
 */
#ifndef _DEFAULT_STRING
	#define _DEFAULT_STRING "<Unbekannt>"
#endif

/**
 * The basic structure for the MP3 files in this program: it stores
 * everything that is needed for file transfer to the Creative Audio
 * Player.
 */
struct id3_struct {
	const char *filename;	/* the filename as string */
	unsigned int size;	/* the filesize */
	const char *artist;	/* the ID3 artist of the song as string */
	const char *title;	/* the ID3 title of the song as string */
	const char *album;	/* the ID3 album of the song string */
	const char *genre;	/* the ID3 genre of the song string */
	unsigned int year;	/* the ID3 year of the song as integer */
	const char *s_year;	/* the ID3 year of the song as string */
	unsigned int time;	/* the length of the song as integer (IMPORTANT) */
	unsigned int trackno;	/* the ID3 tracknumber of a song */
	unsigned int trackid;	/* the trackid of a song on the Creative player */
	unsigned int frequency;	/* the sample frequency of an MP3 file (may be ununsed) */
	unsigned int bitrate;	/* the bitrate on an MP3 file (may be unused) */

	struct id3_struct *next;	/* pointer for the tracklist */
	struct id3_struct *element;	/* pointer for the tracklist */
};

typedef struct id3_struct s_id3_tag;

/**
 * id3_link_file() opens an MP3 file with the given filename and will return
 * an ID3 tag object that is used for further MP3 tag processing. This function
 * must be the very first method to be carried out on an MP3 file. If the file
 * cannot be opened for some reason, NULL is returned. If the second parameter is
 * set to a non-NULL value, only ID3 version 1 tags are used, even if version 2 tags
 * are present.
 */
ID3Tag*		id3_link_file(const char *filename, char id3_v1);

/**
 * id3_unlink_file() will unlink an MP3 file from its tag object, close the file
 * descriptor and destroy, i.e. free() the tag object. This method is the last one
 * in ID3 tag handling.
 */
void		id3_unlink_file(ID3Tag *tag);

/**
 * id3_get_file_size() will return the filesize of the given file. If the size cannot
 * be determined for some reason, 0 is returned and errno is set.
 */
size_t 		id3_get_file_size(const char *filename);

/**
 * The following functions will retrieve the ID3 information from a tag object that has
 * been obtained by id3_link_file(). They will return NULL, if the specific tag cannot be
 * read for some reason and they will return _DEFAULT_STRING if the tag is not set in
 * the MP3 file (i.e. it is an empty string).
 */
const char*	id3_get_artist(ID3Tag *tag);
const char*	id3_get_title(ID3Tag *tag);
const char*	id3_get_album(ID3Tag *tag);
const char*	id3_get_genre(ID3Tag *tag);

/**
 * id3_get_syear() returns the year of the MP3 file as string, 
 * id3_get_year() returns the year as integer
 */
const char*	id3_get_syear(ID3Tag *tag);
unsigned int	id3_get_year(ID3Tag *tag);
unsigned int	id3_get_trackno(ID3Tag *tag);

/**
 * These are C++ functions that are very important to the program and are described in
 * detail in id3_header.h .
 */
extern unsigned int id3_get_time(ID3Tag *t);
extern unsigned int id3_get_frequency(ID3Tag *t);
extern unsigned int id3_get_bitrate(ID3Tag *t);

/**
 * id3_get_id3_struct() will take the filename of an MP3 file as an argument and will
 * return a complete struct id3_struct with all pieces of information already filled in.
 * It automatically does all the stuff like memory allocation, linking files to tag objects,
 * retrieving the necessary information using the functions defined above and finally
 * unlinking the MP3 file from its tag object. It will return NULL if anything in that
 * whole processing line went wrong.
 */
s_id3_tag*      id3_get_id3_struct(const char *filename, char id3v1);

/**
 * id3_delete_id3_struct() tries to free all the memory that is occupied by an instance of
 * struct id3_struct. This method is dangerous, as it causes SEGFAULTS as well as memory
 * leaks. This is still a big TODO!
 */
void		id3_delete_id3_struct(s_id3_tag *tag);

/**
 * id3_print_tags() prints the ID3 tags in a fancy manner to the screen.
 */
void		id3_print_tags(s_id3_tag *tag);

#endif
