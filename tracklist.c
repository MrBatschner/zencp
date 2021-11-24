/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * tracklist.c - implementation file for a tracklist
 *
 * This file provides the implementation of a tracklist. It is used to store
 * the list of tracks that are stored on the player and is made up of
 * elements of struct id3_tag.
 * 
 * The list works as follows (example: 3 tracks from ABBA, 2 tracks from
 * AC/DC, 1 track from Blue Brothers, no tracks of artists beginning with 
 * letter C):
 *
 * Array [0.._MAX_INDEX]
 * +--------+            +---------------+         +---------------+
 * | A  ----+----------->| ABBA          |   +---->| AC/DC         |
 * +--------+            | Dancing Queen |   |     | Damned        |
 * | B  ----+-------+    +---------------+   |     +---------------+
 * +--------+       |    | Next: --------+---+     | Next: NULL    +
 * | C  ----+->NULL |    +---------------+         +---------------+
 * +--------+       |    | Element: |    |         | Element: |    |
 * | ...    |       |    +----------+----+         +----------+----+
 *                  |               |                         |
 *                  |               V                         V
 *                  |    +---------------+         +---------------+
 *                  |    | ABBA          |         | AC/DC         |
 *                  |    | Waterloo      |         | Meltdown      |
 *                  |    +---------------+         +---------------+
 *                  |    | Next: NULL    |         | Next: NULL    |
 *                  |    +---------------+         +---------------+
 *                  |    | Element: |    |         | Element: NULL |
 *                  |    +----------+----+         +---------------+
 *                  |               |
 *                  |               V
 *                  |    +---------------+
 *                  |    | ABBA          |
 *                  |    | Voulez-vous   |
 *                  |    +---------------+
 *                  |    | Next: NULL    |
 *                  |    +---------------+
 *                  |    | Element: NULL |
 *                  |    +---------------+
 *                  |
 *                  |
 *                  |    +---------------+
 *                  +--->| Blues Bros.   |
 *                       | Peter Gun     |
 *                       +---------------+
 *                       | Next: NULL    |
 *                       +---------------+
 *                       | Element: NULL |
 *                       +---------------+
 *
 * The index-value is just the first letter of the artist field. Artists with
 * the same first letter are simply linked via the next field, all tracks from
 * the same artist are linked via the element field.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#include "tracklist.h"

s_id3_tag* trackarray[_MAX_INDEX];

void tracklist_setup_tracklist(s_id3_tag* array[]) {
	int i;
	if (!array) return;
	
	for (i = 0; i < _MAX_INDEX; i++) {
		array[i] = 0;
	}
}


int tracklist_get_index(s_id3_tag *tag) {
	int index;
	if ((!tag) || (!tag->artist)) return -1;

	index = (tag->artist)[0];	/* get the ASCII value of the first letter */

	if ((index >= 65) && (index <= 90)) {	/* if it is a capital letter, subtract 65 */
		index -= 65;
	} else if ((index >= 97) && (index <= 122)) { /* if it is a small letter, subtract 97 */
		index -= 97;
	} else if ((index >= 48) && (index <= 57)) {  /* if it is a number, the index will be 26 */
		index = 26;
	} else {			/* and special chars have index 27 */
		index = 27;
	}

	/* this way we got an index number between 0 and 27 which we will mod by 
	 * the number of entries in the index array */
	return index % _MAX_INDEX;
}


/**
 * This function may be confusing and even I have problems with understanding it right now,
 * so the comments may seem weird. Note that this function was written at 23:36 inside of a very
 * crowded train of the Light City Rail transport in Stuttgart (service line U6).
 */
s_id3_tag* tracklist_insert(s_id3_tag* array[], s_id3_tag *new_tag) {
	int index = 0;
	s_id3_tag* root = 0;
	s_id3_tag* artist = 0;
	s_id3_tag* title = 0;
	s_id3_tag* tag = 0;

	/* return if arguments were empty */
	if ((!array) || (!new_tag)) return 0;
	
	/* TODO: possible memory leak in here... */
	/* allocate a new instance of struct id3_struct as I want to
	 * have a copy, and set its field to the same values as in
	 * new_tag */
	tag = (s_id3_tag*)malloc(sizeof(s_id3_tag));
	tag->artist = new_string(new_tag->artist);
	tag->title = new_string(new_tag->title);
	tag->album = new_string(new_tag->album);
	tag->next = 0;	/* IMPORTANT to set these two to 0 */
	tag->element = 0;
	tag->trackid = new_tag->trackid;
	
	/* get the index value */
	index = tracklist_get_index(tag);

	/* if the array[index] already points to an element, assign it to
	 * root, otherwise, let array[index] point to the new element tag and
	 * exit happily */
	if (!(root = array[index])) {
		array[index] = tag;
		return tag;
	} 

	/* ok, array[index] already pointed to an element, so we will look
	 * if our artist is already in the list (note that the pointer to
	 * root is called by reference: if the artist was not found, NULL is
	 * returned and root will point to the last element of the list)
	 */
	artist = tracklist_find_artist(&root, tag);

	/* the artist was found so we will look for the title */	
	if (artist) {
		/* Starting from the first track by our looked-for artist we will
		 * search through the list of titles (element pointers) and look
		 * if the title is contained. If not, we get a NULL here and artist
		 * will point to the last element of the list.
		 */
		title = tracklist_find_title(&artist, tag);
		
		/* the title was found in the list, that means that the track is already
		 * on the player, so we will not need out tag object anymore and return the
		 * the pointer to the element in the list
		 */
		if (title) {
			free(tag);
			return title;
		} else {
			/* The track is not in there, so artist will point to the last element of tracks
			 * by that artist. We first assign it to title (not necessary but more
			 * elegant) and make its element field point to the new track.
			 */
			title = artist;
			title->element = tag;
		}
	} else {
		/* Similar to above, the artist was not found so root will point to the last
		 * element of artist, so that we make its next element point to the new tag.
		 */
		artist = root;
		artist->next = tag;
	}
	tag->next = 0;
	tag->element = 0;

	return tag;
}

s_id3_tag* tracklist_remove(s_id3_tag *tag) {
	/* Guess what: not implemented yet as it is not needed by now. */
	return 0;
}


s_id3_tag* tracklist_find_artist(s_id3_tag **root, s_id3_tag *tag) {
	s_id3_tag* r;

	/* If root or tag or *root or tag->artist or *root->artist is
	 * NULL (which should never be the case), return immediately.
	 * *root is assigned to r in this step
	 */
	if ((!root) || (!tag) || (!(r = *root)) ||
	    (!tag->artist) || (!r->artist)) return 0;
	
	while (r) {
		/* if the current element has the artist we are looking for */
		if (!strcmp(tag->artist, r->artist)) return r;	/* just return its pointer */
		if (!r->next) break;	/* otherwise proceed to the next element as long as there is */
		*root = r = r->next;	/* a pointer to it */
	}
	
	return 0;
}


s_id3_tag* tracklist_find_title(s_id3_tag **node, s_id3_tag *tag) {
	s_id3_tag *n;
	if ((!node) || (!(n = *node)) ||
	    (!tag->title) || (!n->title)) return 0;

	while (n) {
		if (!strcmp(tag->title, n->title)) return *node;
		if (!n->element) break;
		*node = n = n->element;
	}

	return 0;
}


s_id3_tag* tracklist_find_tag(s_id3_tag *array[], s_id3_tag *tag) {
	int index = 0;
	s_id3_tag *artist = 0;
	s_id3_tag *title = 0;
	s_id3_tag *root = 0;
	
	/* if neither the index array nor a tag is given, it is hard to say if one is contained in the other,
	 * therefore return NULL */
	if ((!array) || (!tag)) return 0;

	index = tracklist_get_index(tag);	/* Determine the index and get the pointer to the first artist */
	if (!(root = array[index])) return 0;	/* with the same index. If it does not exist, return NULL. */

	artist = tracklist_find_artist(&root, tag);	/* search for the artist */
	if (artist) {				/* ahh, the artist was found, now search for the title */
		title = tracklist_find_title(&artist, tag);

		/* ok, title was found as well, so compare the album string and if they are equal, return the
		 * pointer to that element*/
		if ((title) && (!strcmp(title->album, tag->album))) return title;
	}
	
	/* either artist or title was not found at this point so return 0 */
	return 0;
}


void tracklist_dump(s_id3_tag *array[]) {
	int i;
	s_id3_tag* t, *s;
	if (!array) return;

	for (i = 0; i < _MAX_INDEX; i++) {
		printf("\n%2d \\\n", i);
		if (!array[i]) {
			printf("    +- NULL\n");
			continue;
		} else {
			t = array[i];

			while (t) {
				printf("    +- %s - %s (%d)\n", t->artist, t->title, t->trackid);
				s = t->element;
				while (s) {
					printf("    |  +- %s - %s (%d)\n", s->artist, s->title, s->trackid);
					s = s->element;
				}
				t = t->next;
			}
		}
	}
	
	printf("\n");
	return;
}
