/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * misc.c - implementation file for miscellaneous functions
 *
 * This file provides the implementation for function that are used within
 * zencp and did not fit into any of the other files.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#include "misc.h"

char* new_string(const char* s) {
	unsigned int l;
	char *p;
	
	if (!s) return 0;	/* if the parameter is a NULL pointer, return */
	l = strlen(s) + 1;	/* determine the string length and add space for \0 */
	
	p = (char*)malloc(sizeof(char[l]));	/* allocate memory for the new string using the
						   size information from above */
	if (!p) {
		print_error(G_NOMEM);
		return 0;
	}
	
	strcpy(p, s);		/* copy the string contents */
	return p;		/* and return the pointer to the newly allocated string */
}


char is_digit(char c) {
	if ((c >= 48) && (c <= 57)) return 1;
	return 0;
}


void print_error(int msg_num) {
	fprintf(stderr, " ERROR: ");

	switch(msg_num) {
		case OPT_F: fprintf(stderr, "-F option was called without an argument\n\n");
			break;
		case OPT_FE: fprintf(stderr, "-F option cannot be used in conjunction with -e option\n\n");
			break;
		case OPT_D: fprintf(stderr, "-d option was called without device identifier\n\n");
			break;
		case OPT_P: fprintf(stderr, "-p option must be called with exactly one file\n\n");
			break;
		case ID3_RETR: fprintf(stderr, "ID3 tags could not be retrieved\n\n");
			break;
		case PL_DISC: fprintf(stderr, "error while discovering Creative MP3 players\n\n");
			break;
		case PL_COMM: fprintf(stderr, "player communication failed\n\n");
			break;
		case PL_NOID: fprintf(stderr, "no player with that ID was found\n\n");
			break;
		case G_ABRT: fprintf(stderr, "aborted by user\n\n");
			break;
		case PL_TRPR: fprintf(stderr, "track already exists on player\n\n");
			break;
		case G_NOMEM: fprintf(stderr, "unable to allocate new memory\n\n");
			break;
		default: fprintf(stderr, "unknown error\n\n");
			break;
	}
}
