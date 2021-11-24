/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * list.c - implementation file for a doubly linked list
 *
 * This file provides the implementation for a doubly linked list. A lot of
 * functions that are used for list processing can be found in here although
 * they are actually not used for zencp.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#include "list.h"

mp3_file* list_first_element(mp3_file *e) {
	mp3_file *i = e;
	if (!i) return 0;
	while (i->prev) i = i->prev;
	return i;
}

mp3_file* list_last_element(mp3_file *e) {
	mp3_file *i = e;
	if (!i) return 0;
	while (i->next) i = i->next;
	return i;
}

mp3_file* list_append(mp3_file *e, mp3_file *n) {
	mp3_file *i = list_last_element(e);
	if ((!e) || (!n)) return 0;

	i->next = n;
	
	n->prev = i;
	n->next = 0;

	return n;
}

mp3_file* list_insert_before(mp3_file *e, mp3_file *n) {
	if ((!e) || (!n)) return 0;
	n->prev = e->prev;
	n->next = e;
	
	if (e->prev) e->prev->next = n;
	e->prev = n;

	return n;
}

mp3_file* list_insert_after(mp3_file *e, mp3_file *n) {
	if ((!e) || (!n)) return 0;
	n->prev = e;
	n->next = e->next;

	if (e->next) e->next->prev = n;
	e->next = n;

	return n;
}

mp3_file* list_remove(mp3_file *e) {
	mp3_file *t;
	
	if (!e) return 0;

	if ((!e->prev) && (!e->next)) return 0;

	if (!e->prev) {
		t = e->next;
		t->prev = 0;
	} else if (!e->next) {
		t = e->prev;
		t->next = 0;
	} else {
		t = e->prev->next = e->next;
		e->next->prev = e->prev;
	}

	free(e);
	return t;
}

mp3_file* new_mp3_element(char *filename) {
	mp3_file *n = (mp3_file*)malloc(sizeof(mp3_file));
	if (!n) return 0;
	
	n->filename = filename;
	n->prev = n->next = 0;

	return n;
}


mp3_file* append_new_file(mp3_file *e, char *filename) {
	mp3_file *n = new_mp3_element(filename);
	if (!n) return 0;
	if (!e) return n;
	
	return list_append(e, n);
}

mp3_file* search_file(mp3_file *e, char *filename) {
	mp3_file *i = list_first_element(e);
	if (!i) return 0;

	while (i) {
		if (!strcmp(i->filename, filename)) return i;
		i = i->next;
	}

	return 0;
}
