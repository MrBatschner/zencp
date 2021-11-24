/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * misc.h - header file for miscellaneous functions
 *
 * This file provides the prototypes for function that are used within
 * zencp and did not fit into any of the other files.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#ifndef __ZENCP_MISC_H
#define __ZENCP_MISC_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * An enumeration of error types that are used within zencp. Makes it
 * easier than remembering different numbers.
 */
enum zencp_errors {
	OPT_F,		/* Options: option -F fas not been correctly */
	OPT_FE, 	/* Options: option -e fas not been correctly */
	OPT_D, 		/* Options: option -D fas not been correctly */
	OPT_P,		/* Options: option -p fas not been correctly */
	ID3_RETR, 	/* ID3 Tags: error with ID3 tag processing */
	PL_DISC, 	/* Player: player discovery failed */
	PL_COMM, 	/* Player: player communictaion failed */
	PL_NOID, 	/* Player: a player with a certain ID could not be found */
	PL_TRPR,	/* Player: a track is already present */
	G_ABRT, 	/* General: user abort */
	G_NOMEM		/* General: out of memory */
};

/**
 * new_string() takes a string as argument and will copy its contents
 * into a newly allocated string. This is useful if you get a string from an
 * object that you want to free without destroying the string itself.
 */
char*	new_string(const char* s);

/**
 * print_error() will translate an error number (see zencp_errors) into
 * an error string and print it to stderr
 */
void	print_error(int msg_num);

/**
 * is_digit() returns 1 if the character provided as an argument is a digit
 * and will return 0 if it is something else
 */
char	is_digit(char c);

#endif
