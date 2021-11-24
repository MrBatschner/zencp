/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * tracklist.h - header file for a tracklist
 *
 * This file provides the prototypes of functions for a tracklist. It is 
 * used to store the list of tracks that are stored on the player and is 
 * made up of elements of struct id3_tag.
 * 
 * See tracklist.c for an ASCII art example of how this list is supposed to
 * work.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#ifndef __ZENCP_tracklist_H
#define __ZENCP_tracklist_H

#include <stdio.h>
#include "id3.h"
#include "misc.h"

/* the maximum number of track indices, 28 is used for each letter of the
 * alphabet + 1 index for numerical values + 1 index for everything else
 */
#define _MAX_INDEX 28


/**
 * tracklist_setup_tracklist() will set every element of an array to
 * NULL which is quite important so call this function before you attempt
 * to create a new tracklist
 */
void tracklist_setup_tracklist(s_id3_tag* array[]);

/**
 * tracklist_get_index() will return the index for the element tag. This
 * element is of type struct id3_tag and has a field artist of which the
 * first character is used to obtain the index value.
 */
int tracklist_get_index(s_id3_tag *tag);

/**
 * tracklist_insert() will insert the element new_tag into the tracklist
 * that is maintained by array[]. If the element is already contained in
 * the list, it will return a pointer to it. Otherwise, a pointer to the
 * newly inserted element is returned.
 */
s_id3_tag* tracklist_insert(s_id3_tag* array[], s_id3_tag *new_tag);

/**
 * tracklist_remove() does nothing as it is not implemented yet
 */
s_id3_tag* tracklist_remove(s_id3_tag *tag);

/**
 * tracklist_find_artist() takes the first (**root) element of a simply 
 * linked list of struct id3_tag elements and looks for an element that
 * has the same artist as the element tag. If this search is 
 * successfull, a pointer to that element is returned. Otherwise,
 * NULL is returned and **root will point to the last element in
 * that list.
 */
s_id3_tag* tracklist_find_artist(s_id3_tag **root, s_id3_tag *tag);

/**
 * tracklist_find_title() takes the first (**node) element of a simply 
 * linked list of struct id3_tag elements and looks for an element that
 * has the same title as the element tag. If this search is 
 * successfull, a pointer to that element is returned. Otherwise,
 * NULL is returned and **node will point to the last element in
 * that list.
 */
s_id3_tag* tracklist_find_title(s_id3_tag **node, s_id3_tag *tag);

/**
 * tracklist_find_tag() is the combination of the two functions
 * given above. It will look through the whole tracklist for an element
 * that has equal artist, title and album fields as tag and will 
 * return a pointer to it, if this search was successful. If not, NULL
 * is returned.
 */
s_id3_tag* tracklist_find_tag(s_id3_tag *array[], s_id3_tag *tag);

/**
 * tracklist_dump() prints the contents of the tracklist to the screen.
 */
void tracklist_dump(s_id3_tag *array[]);

#endif
