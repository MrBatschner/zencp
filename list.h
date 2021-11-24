/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * list.h - header file for a doubly linked list
 *
 * This file provides the prototypes and structures for a doubly linked list.
 * This list will consist of instances if struct mp3_file_struct.
 * A lot of functions that are used for list processing are defined in here
 * although they are actually not used for zencp.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#ifndef __ZENCP_LIST_H
#define __ZENCP_LIST_H

#include <malloc.h>
#include <string.h>
#include "misc.h"

/**
 * Implements a simple doubly linked list containing just a string. This 
 * struct is needed for the list of MP3 files that are provided as command
 * line parameters to zencp.
 */
struct mp3_file_struct {
	char *filename;

	struct mp3_file_struct *prev;
	struct mp3_file_struct *next;
};

typedef struct mp3_file_struct mp3_file;

/**
 * list_first_element() returns the very first element of the doubly
 * linked list. If an invalid parameter is given, it will either return
 * NULL or do some very nasty things with your memory.
 */
mp3_file* list_first_element(mp3_file *e);

/**
 * list_last_element() returns the last element of the doubly linked list
 * and will behave similarly to list_first_element().
 */
mp3_file* list_last_element(mp3_file *e);


/**
 * list_append() will append the element n to the doubly linked list
 * that e is an element of. It returns a pointer to the newly inserted
 * element or NULL if anything went wrong.
 */
mp3_file* list_append(mp3_file *e, mp3_file *n);

/**
 * list_insert_before() inserts element n into the doubly linked list
 * just before element e. It returns a pointer to the newly inserted
 * element.
 */
mp3_file* list_insert_before(mp3_file *e, mp3_file *n);

/**
 * list_insert_after() inserts the new element n into the doubly
 * linked list just after element e.
 */
mp3_file* list_insert_after(mp3_file *e, mp3_file *n);

/**
 * list_remove() removes element e from the doubly linked list it
 * is contained in. It will return a pointer to the element that
 * came immediately after element e before it had been removed.
 * If e was the last element in the list, NULL is returned.
 */
mp3_file* list_remove(mp3_file *e);


/**
 * append_new_file() will insert a new element with filename into 
 * the doubly linked list. Returns a pointer to the new element.
 */
mp3_file* append_new_file(mp3_file *e, char *filename);

/**
 * new_mp3_element() allocates memory for a new instance of 
 * struct mp3_file and fills its only field with filename. Returns
 * a pointer to that new element.
 */
mp3_file* new_mp3_element(char *filename);

/**
 * search_file() does a linear search through the doubly linked list
 * that e is an element of. If it finds an element that contains
 * the same string as filename, it will return its pointer and NULL
 * otherwise.
 */
mp3_file* search_file(mp3_file *e, char *filename);

#endif
