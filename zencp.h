/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * zencp.h - header file for the main routines of zencp
 *
 * This file provides function prototypes and structures that are needed
 * by the main routines of zencp.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#ifndef __ZENCP_H
#define __ZENCP_H

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "list.h"
#include "id3.h"
#include "player.h"
#include "tracklist.h"
#include "misc.h"

#define ZENCP_VERSION "v.0.02"


/* Prototypes for all used functions */

/**
 * print_help_screen() will print a help screen to the screen. It takes no arguments.
 */
void print_help_screen(void);

/**
 * print_version_information() will just print a very short header about the program name
 * and its version.
 */
void print_version_information(void);

/**
 * parse_cmdline() is the command line parser. It will take the argc and argv variables
 * to parse them and will set some global variables (ugly, I know). The return value is the
 * number of songs that were received as command line arguments (note that these are only 
 * filenames, there is no check, if they exist or are even media files. argc and argv will 
 * not be changed by this function.
 */
int parse_cmdline(int argc, char *argv[], mp3_file **file_list);

#endif
