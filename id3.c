/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * id3.c - implementation file for MP3 ID3 Tag handling
 *
 * This file provides the implementation for functions that are needed
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

#include "id3.h"

/**
 * make a real string from the _DEFAULT_STRING preprocessor constant
 */
char default_tag[256] = _DEFAULT_STRING;


ID3Tag* id3_link_file(const char *filename, char id3_v1) {
	ID3Tag *t = ID3Tag_New();		/* allocate a new tag object (id3lib) */
	if ((!filename) || (!t)) return 0;	/* if no filename was given or a 
						   tag object cannot be allocated,
						   stop here */
	if (id3_v1) {
		ID3Tag_LinkWithFlags(t, filename, ID3TT_ID3V1);	/* obtain the tag object (ID3 v1 only) */
	} else {
		ID3Tag_LinkWithFlags(t, filename, ID3TT_ID3);	/* get the tag object (ID3 v2) */
	}
	return t;
}


void id3_unlink_file(ID3Tag *tag) {
	if (!tag) return;
	ID3Tag_Delete(tag);
	return;
}


size_t id3_get_file_size(const char *filename) {
	struct stat buff;
	if (!filename) return 0;
	if (stat(filename, &buff)) return 0;
	return (size_t)buff.st_size;
}


/**
 * id3_get_frame_text() extracts the text information that is contained in an MP3 ID3 frame
 * and returns it as string. ID3 tags have the following structure:
 * MP3 file -> ID3 tag +> ID3 frame (artist) +> ID3 field (text)
 *                     |                     -> ID3 field (integer)
 *                     +> ID3 frame (title)  +> ID3 field (text)
 *                                           -  ID3 field (integer)
 *            ... and so on ...
 */
const char* id3_get_frame_text(ID3Frame *frame) {
	ID3Field *field;
	char *buff = 0;
	size_t size = 0;
	
	if (frame) {						/* if an ID3 frame was given... */
		field = ID3Frame_GetField(frame, ID3FN_TEXT);	/* ... try to extract the text field */
		
		if (field) {					/* if the field is there */
			size = 1 + ID3Field_Size(field);	/* determine the field size */
			buff = (char*)malloc(sizeof(char[size]));	/* allocate a string */
			ID3Field_GetASCII(field, buff, size);	/* and fill it with the field contents */
		}
	}

        if ((!buff) || (strlen(buff) == 0)) buff = default_tag;	/* if the field was empty, return the 
								   default value */
	return buff;
}


const char* id3_get_artist(ID3Tag *tag) {
	ID3Frame *frame; 
	
	/* extract the right frame for the artist */
	if ((frame = ID3Tag_FindFrameWithID(tag, ID3FID_LEADARTIST)) ||
		(frame = ID3Tag_FindFrameWithID(tag, ID3FID_BAND)) ||
		(frame = ID3Tag_FindFrameWithID(tag, ID3FID_CONDUCTOR)) ||
		(frame = ID3Tag_FindFrameWithID(tag, ID3FID_COMPOSER))	) {
		
		return id3_get_frame_text(frame);	/* and return the frame's text field */
	}
	return default_tag;
}


const char* id3_get_title(ID3Tag *tag) {
	ID3Frame *frame;

	/* extract the right frame for the title */
	if ((frame = ID3Tag_FindFrameWithID(tag, ID3FID_TITLE))) {
		return id3_get_frame_text(frame);	/* and return the frame's text field */
	}
	return default_tag;
}


const char* id3_get_album(ID3Tag *tag) {
	ID3Frame *frame;

	/* extract the right frame for the album */
	if ((frame = ID3Tag_FindFrameWithID(tag, ID3FID_ALBUM))) {
		return id3_get_frame_text(frame);	/* and return the frame's text field */
	}
	return default_tag;
}


/**
 * The genre description is not encoded in plain text into an ID3 tag. It is just a number
 * that is looked up in an array (ID3_v1_genre_description provided by id3lib). Unfortunately
 * this number is stored in the text field of that frame in this way: (42). Therefore, after
 * retrieving the text field in the usual way, the string must be parsed to the get the number
 * that is used for looking up the right genre description in the array.
 */
const char* id3_get_genre(ID3Tag *tag) {
	ID3Frame *frame;
	char *buff = 0;
	char ch_tmp[16];
	unsigned int genre_id;
	unsigned short i = 1;

	/* extract the right frame for the genre */
	if ((frame = ID3Tag_FindFrameWithID(tag, ID3FID_CONTENTTYPE))) {
		buff = (char*)id3_get_frame_text(frame);	/* and return the frame's text field */
	}

	if (!buff) return default_tag;

	/* if the first character is a '(' */
	if (buff[0] == '(') {
		/* copy all following number characters into ch_tmp until ')' is found */
		while ((buff[i] != ')') && (buff[i] != '\0') && (is_digit(buff[i]))) {
			ch_tmp[i-1] = buff[i];
			i++;
		}
		ch_tmp[i-1] = '\0';	/* better put a null terminator in there */
	} else {
		return default_tag;
	}

	if (strlen(ch_tmp) == 0) return default_tag;

	genre_id = (unsigned int)strtol(ch_tmp, 0, 10);	/* convert the number to an integer */
	
		/* if the number obtained is bigger than the number of know genre descriptions,
		 * we will return the default string */
	if (genre_id >= ID3_NR_OF_V1_GENRES) return default_tag;
	
	return ID3_v1_genre_description[genre_id];	/* return the right genre description */
}


const char *id3_get_syear(ID3Tag *tag) {
	ID3Frame *frame;
	static char *buff = 0;

	if ((frame = ID3Tag_FindFrameWithID(tag, ID3FID_YEAR))) {
		buff = (char*)id3_get_frame_text(frame);
	}

	/* if the ID3 information for the year was not set, we will return the string "0" */
	if ((!buff) || (strlen(buff) == 0)) {
		buff = (char*)malloc(sizeof(char[2]));
		buff = "0";
	}
	return (const char*)buff;
}


unsigned int id3_get_year(ID3Tag *tag) {
	char *s_year;
	if (!tag) return 0;

	/* get the year string by id3_get_syear() */
	s_year = (char*)id3_get_syear(tag);
	if (!s_year) return 0;

	/* convert the string to an integer and return it */
	return (unsigned int)strtol(s_year, 0, 10);
}


unsigned int id3_get_trackno(ID3Tag *tag) {
	ID3Frame *frame;
	static char *buff = 0;

	/* get the track number in the usual fashion as string */
	if ((frame = ID3Tag_FindFrameWithID(tag, ID3FID_TRACKNUM))) {
		buff = (char*)id3_get_frame_text(frame);
	}

	if ((!buff) || (strlen(buff) == 0)) return 0;
	/* convert the string into an integer and return it */
	return (unsigned int)strtol(buff, 0, 10);
}


s_id3_tag* id3_get_id3_struct(const char *filename, char id3v1) {
	s_id3_tag *tag;
	ID3Tag *t;
	size_t filesize;

	/* if the filename was not given or the filesize cannot be determined (i.e. error
	 * in accessing the file), return 0 */
	if ((!filename) || (!(filesize = id3_get_file_size(filename)))) {
		return 0;
	}
	
	/* try to link the file, if that does not work, return 0 */
	t = id3_link_file(filename, id3v1);
	if (!t) return 0;

	/* allocate a new instance if struct id3_struct, if it fails, we return 0 */
	tag = (s_id3_tag*)malloc(sizeof(s_id3_tag));
	if (!tag) return 0;
	
	/* fill in all the struct's fields by just calling all the functions above */
	tag->filename = filename;
	tag->size     = filesize;
	tag->artist   = id3_get_artist(t);
	tag->title    = id3_get_title(t);
	tag->album    = id3_get_album(t);
	tag->genre    = id3_get_genre(t);
	tag->year     = id3_get_year(t);
	tag->s_year   = id3_get_syear(t);
	tag->trackno  = id3_get_trackno(t);
	tag->time     = id3_get_time(t);
	tag->frequency = id3_get_frequency(t);
	tag->bitrate  = id3_get_bitrate(t);
	tag->trackid  = 0;	/* will be set by the player if a transfer succeeded */
	
	tag->next     = 0;	/* needed for the tracklist and MUST be NULL at init time */
	tag->element  = 0;

	/* unlink and destroy the tag object and return the freshly created id3_struct */
	id3_unlink_file(t);
	return tag;
}


/* Did I already mention that this method is dangerous as it does not work and just produces
 * segfaults? */
void id3_delete_id3_struct(s_id3_tag *tag) {
	if (!tag) return;

	free((char*)tag->filename);
	free((char*)tag->artist);
	free((char*)tag->title);
	free((char*)tag->album);
	free((char*)tag->genre);
	free((char*)tag->s_year);

	free(tag);

	return;
}


void id3_print_tags(s_id3_tag *tag) {
	if (!tag) return;	/* if the tag or any of the string fields is NULL, return without
				   doing anything */
	if ((!tag->filename) ||
		(!tag->size) ||
		(!tag->artist) ||
		(!tag->title) ||
		(!tag->album) ||
		(!tag->genre)) return;

	/* printf all that stuff to the screen */
	printf(" File:     %s\n", tag->filename);
	printf(" Size:     %d kB\n\n", tag->size/1024);
	
	printf("  Trackno: %d\n", tag->trackno);
	printf("  Artist:  %s\n", tag->artist);
	printf("  Title:   %s\n", tag->title);
	printf("  Album:   %s\n", tag->album);
	printf("  Genre:   %s\n", tag->genre);
	printf("  Year:    %d\n\n", tag->year);
	
	printf("  Time:    %d:%02d\n", tag->time/60, tag->time%60);
	printf("  Freq.:   %d Hz\n", tag->frequency);
	printf("  Bitrate: %d kbps\n\n", tag->bitrate/1000);
}
