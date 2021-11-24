/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * zencp.c - implementation file for the main routines of zencp
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#include "zencp.h"

/* global boolean flags that memorize which command line switch has been set */
static char _b_switch_l = 0;
static char _b_switch_p = 0;
static char _b_switch_h = 0;
static char _b_switch_V = 0;
static char _b_switch_f = 0;
static char _b_switch_e = 0;
static char _b_switch_i = 0;
static char _b_switch_y = 0;
static char _b_switch_T = 0;
static char _b_switch_unknown = 0;
/* two switches take arguments that are stored in these strings */
static char* _s_switch_d = 0;
static char* _s_switch_F = 0;

/* the number of players and the player array */
int players = 0;
njb_t player_array[_MAX_PLAYERS];


/**
 * sigint_cleanup() is a signal handler that is supposed to unlock every player
 * if a <CTRL>+<C> is received. This one is ugly and I believe that everybody who
 * knows how to implement a proper signal handler will get a heart attack.
 */
void sigint_cleanup(int sig) {
	unsigned int i, p = 0;
	njb_t *player;

	printf("\n\nCaught CTRL+C (SIGINT). Releasing all locked players.\n");
	
	/* go through the array of players and release every single player in
	 * there */
	for (i = 0; i < players; i++) {
		player = &(player_array[i]);
		if (player_release(&player)) {
			p++;
		}
	}

	printf("Released %d player%s. Exiting.\n", p, (p > 1) ? "s" : "");

	/* set the default handler and raise the signal again */
	signal(sig, SIG_DFL);
	raise(sig);
}


void print_help_screen(void) {
	printf(" Usage: zencp [ACTION] [OPTION]... MEDIAFILE...\n");
	printf("        zencp (-h | --help | -l | --list-devices)\n\n");
	
	printf(" Actions:\n");
	printf("   -l, --list-devices \t\t list all connected Jukebox devices\n");
	printf("   -T, --track-list \t\t print a list with all tracks on the Jukebox\n");
	printf("   -p, --print-id3 \t\t print ID3 information of file, do not transfer\n");
	printf("   -h, --help \t\t\t print this help screen\n");
	printf("   -V, --version \t\t print version information and exit\n\n");
	
	printf(" Options:\n");
	printf("   -d, --device DEV \t\t transfer files to Jukebox device DEV\n");
	printf("   -f, --force \t\t\t transfer and overwrite already present files on the Jukebox\n");
	printf("   -e, --empty-id3 \t\t allow emtpy ID3 tags\n");
	printf("   -F, --fill-id3 STRING \t fill empty ID3 tags with STRING for transfer\n");
	printf("   -i, --id3v1 \t\t\t use ID3v1 tags instead of ID3v2\n");
	printf("   -y, --yes \t\t\t transfer files without user interaction\n\n");
}


void print_version_information(void) {
	printf(" This copy of zencp was compiled on %s.\n", __DATE__);
	printf(" This program is released under the GNU General Public License (GPL).\n\n");
}


int parse_cmdline(int argc, char *argv[], mp3_file **file_list) {
	int i = 0;
	unsigned int songs = 0;
	int args = argc;
	
	for (i = 1; i < argc; i++) {

		/* simple do-and-exit actions first in the order they appear in the help screen */
		/* they will exit this for loop immediately */
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {
			_b_switch_h = 1;
			args--;
			break;
		}

		if ((!strcmp(argv[i], "-V")) || (!strcmp(argv[i], "--version"))) {
			_b_switch_V = 1;
			args--;
			break;
		}
		
		if ((!strcmp(argv[i], "-l")) || (!strcmp(argv[i], "--list-devices"))) {
			_b_switch_l = 1;
			args--;
			break;
		}


		/* now simple options that do not take arguments */
		if ((!strcmp(argv[i], "-T")) || (!strcmp(argv[i], "--track-list"))) {
			_b_switch_T = 1;
			args--;
			break;
		}
		
		if ((!strcmp(argv[i], "-p")) || (!strcmp(argv[i], "--print-id3"))) {
			_b_switch_p = 1;
			args--;
			continue;
		}

		if ((!strcmp(argv[i], "-f")) || (!strcmp(argv[i], "--force"))) {
			_b_switch_f = 1;
			args--;
			continue;
		}

		if ((!strcmp(argv[i], "-e")) || (!strcmp(argv[i], "--empty-id3"))) {
			/* if _b_switch_e is less than 0, the -F switch was called as well,
			 * this is wrong and we will stop further command line processing */
			if (_b_switch_e < 0) print_error(OPT_FE);
			_b_switch_e = 1;
			args--;
			continue;
		}
		
		if ((!strcmp(argv[i], "-i")) || (!strcmp(argv[i], "--id3v1"))) {
			_b_switch_i = 1;
			args--;
			continue;
		}
		
		if ((!strcmp(argv[i], "-y")) || (!strcmp(argv[i], "--yes"))) {
			_b_switch_y = 1;
			args--;
			continue;
		}


		/* and here the complex argumented command line options */
                if ((!strcmp(argv[i], "-d")) || (!strcmp(argv[i], "--device"))) {
			/* we expect an argument to this switch here, if there is nothing
			 * left in argv or the next element in argv begins with a -
			 * someone did call this option in the right way */
			if ((++i >= argc) || (argv[i][0] == '-')) {
				print_error(OPT_D);
				break;
			}
			
			_s_switch_d = argv[i];
                        args-=2;
                        continue; 
                }

                if ((!strcmp(argv[i], "-F")) || (!strcmp(argv[i], "--fill-id3"))) {
			/* we expect an argument to this switch here, if there is nothing
			 * left in argv or the next element in argv begins with a -
			 * someone did call this option in the right way */
			if ((++i >= argc) || (argv[i][0] == '-')) {
				print_error(OPT_F);
				break;
			}
			/* -F and -e cannot be called together */
			if (_b_switch_e) {
				print_error(OPT_FE);
				break;
			}
			_s_switch_F = argv[i];
			/* will be set to non-NULL to indicate that -F has been used before */
			_b_switch_e = -1;
                        args-=2;
                        continue; 
                }

		/* default */

		if (argv[i][0] == '-') {
			/* someone tried a command line switch that does not exist */
			fprintf(stderr, " ERROR: No such option: %s\n\n", argv[i]);
			_b_switch_unknown = 1;
			break;
		} else if (!(*file_list)) {
			/* the current argv element is a string that is treated as a filename
			 * and thus needs to be inserted into the file list. As it does not exist
			 * yet, the next line does the rest. */
			*file_list = append_new_file(*file_list, argv[i]);
			songs++;
		} else {
			/* a filename was given and the file_list already exists, so we will
			 * check for duplicates */
			if (!search_file(*file_list, argv[i])) {
				append_new_file(*file_list, argv[i]);
				songs++;
			}
		}
	}

	return songs;
}


/**
 * This is the main function (why, really...).
 */
int main (int argc, char *argv[]) {
	unsigned int songs = 0;		/* the number of songs received as cmdline args */
	unsigned int playersongs = 0;	/* the number of songs stored on the player */
	unsigned int id = 0;
	int i = 0;
	njb_t *player;			/* a pointer to the Creative player to be used */
	s_id3_tag* player_tracklist[_MAX_INDEX];	/* the index list for player tracks */
	s_id3_tag *tag = 0;		/* a pointer for an ID3 tag object */
	s_id3_tag *track_tag = 0;	/* ... */
	mp3_file *file_list = 0;	/* a list of filenames received as cmdline args */
	char yesno = 0;

	printf("zencp %s - Copyright (C) 2005 by Thomas Buchner\n\n", ZENCP_VERSION);
	tracklist_setup_tracklist(player_tracklist);	/* initialize the track list */
	signal(SIGINT, sigint_cleanup);			/* set the signal handler */
	songs = parse_cmdline(argc, argv, &file_list);	/* parse the command line */
	
	/* unknown cmd switch or -h or no argument at all was given */
	if ((_b_switch_unknown) || (_b_switch_h) || (argc == 1)) {
		print_help_screen();
		return 0;
	}

	/* someone wants to know the version of this program (-V) */
	if (_b_switch_V) {
		print_version_information();
		return 0;
	}

	/* someone just wants to know the ID3 information stored in an MP3 file  */
	if (_b_switch_p) {
		if (songs != 1) {	/* this only works with exactly one file name */
			print_error(OPT_P);
			return 1;
		}
		
		/* get the tag object from the specific file and check for errors */
		/* _b_switch_i will control wether ID3 v1 (true) or ID3 v2 (NULL) tags
		 * will be used */
		if (!(tag = id3_get_id3_struct(file_list->filename, _b_switch_i))) {
			print_error(ID3_RETR);
			/*TODO: insert a strtoerr into here after you got the dev manpages */
			return 2;
		}

		
		if (_b_switch_i) printf("Using ID3 v.1 tags:\n\n");
		id3_print_tags(tag);	/* print the tag to the screen */
		free(tag);		/* free the tag */
		return 0;		/* and exit */
	}

	/* no filenames for songs were given and the switched -l or -T (the only ones that 
	 * do not allow any filename) were not set -> the user needs help */
	if ((!_b_switch_l) && (!_b_switch_T) && (songs == 0)) {
		print_help_screen();
		return 0;
	}
	
	/* up to this point we needed no player connectivity but now we will discover creative
	 * players */
	players = player_discovery(player_array);
	if (players < 0) {		/* error while discovering players */
		print_error(PL_DISC);
		return 3;
	}
		
	if (players == 0) {		/* no player found */
		printf(" No Creative MP3 player discovered.\n\n");
		return -1;
	}

	printf(" %i Creative MP3 player%s discovered.\n\n", players, (players > 1) ? "s" : "");

	/* the user just wants to see a list of connected players */
	if (_b_switch_l) {
		for (i = 0; i < players; i++) {		/* go through the player array */
			player = player_lock(player_array, i);	/* lock the player */
			if (!player) {			/* lock impossible? -> ERROR */
				print_error(PL_COMM);
				/*TODO: player_print_errors(stderr)*/;
				return 3;
			}
			player_list_device(player, i);	/* print player info to screen */
			player_release(&player);	/* release the player */
		}
		return 0;				/* and exit */
	}

	
	/* the user wants to use a specified device */
	if (_s_switch_d) {
		/* convert the argument containing the ID string into an integer */
		id = (unsigned int)strtol(_s_switch_d, 0, 10);

		printf(" Using player with ID %d.\n\n", id);

		/* Selecting a certain player works by looping through the array that
		 * contains pointers to the players, locking each player, comparing its ID
		 * to the requested ID and, if the IDs are not equal, releasing the
		 * player again. If the IDs are equal, the loop is exited and the player
		 * stays captured. */

		/* go through the array of players ... */
		for (i = 0; i < players; i++) {
			player = player_lock(player_array, i);	/* lock each player */
			if (!player) {
				print_error(PL_COMM);
				return 3;
			}

			/* and compare the ID, if it is equal to the requested one, exit the
			 * loop and the requested player will be in the variable player */
			if (id == player_get_deviceid(player)) {
				break;
			}

			player_release(&player);	/* release any player that does
							   not have the requested ID */
		}
		
		/* the previous loop was exited without leaving a player object
		 * behing, thus no player with that ID exists */
		if (!player) {
			print_error(PL_NOID);
			return 3;
		}
	} else {
		/* the else part of _b_switch_d, the user does not want to have a specific
		 * player so we will use the first one in the array */
		player = player_lock(player_array, 0);
	}
	
	if (!player) {
		print_error(PL_COMM);
		return 3;
	}
		
	printf(" Using the following device:\n\n");
	player_list_device(player, 0);

	printf("Retrieving player tracklist...");
	playersongs = player_get_tracklist(player, player_tracklist);
	printf("\rRetrieved player tracklist: %d songs on the player\n", playersongs);
	printf("\n");

	/* the user just wants to see which tracks are stored on the device */
	if (_b_switch_T) {
		printf("The following tracks are stored on the player:\n");
		tracklist_dump(player_tracklist);
		player_release(&player);
		return 0;	/* only track listing, so release the player and exit at this
				   point */
	}
	
	/* ok, we've come this far, so the user wants to transfer a file to the player */
	if (_b_switch_i) printf("Using ID3 v.1 tags:\n\n");

	/* iterate over the list of filenames */
	while (file_list) {
		/* get an s_id3_tag object from the current filename */
		if (!(tag = id3_get_id3_struct(file_list->filename, _b_switch_i))) {
			print_error(ID3_RETR);
			/*TODO: insert a strtoerr into here after you got the dev manpages */
			printf(" Skipping %s\n", file_list->filename);
			file_list = list_remove(file_list);
			continue;
		}

		
		/* _b_switch_y controls wether we use user interaction */
		if (!_b_switch_y) {
			/* it was not set, so for every song we ask the user if he really wants
			 * to transfer it */

			/* look wether the track is already on the player */
			track_tag = tracklist_find_tag(player_tracklist, tag);

			/* if track_tag is non-NULL, the track is on the player and we
			 * skip this track if the -f (force) switch is not set */
			if ((track_tag) && (!_b_switch_f)) {
				printf(" %s - %s already exists, skipping.\n\n", tag->artist, tag->title);
				file_list = list_remove(file_list);	
				/* remove the current filename from the list and advance to
				 * the next one */
				continue;
			}
			
			/* tell the user which track he is transferring */
			id3_print_tags(tag);

			/* the track_tag is non-NULL, i.e. the track is on the player but
			 * -f was set, so the track will be overwritten */
			if ((track_tag) && (_b_switch_f)) {
				printf("%s - %s already exists,\n", tag->artist, tag->title);
				printf("and will be overwritten!\n\n");
			}
		
			/* TODO: this must be done in a better way !!!!!!!!! */

			printf("Really send this file ([Y]es/[n]o/[Q]uit)? ");
			do {
				yesno = fgetc(stdin);
			} while ((yesno != EOF) && (strchr("YyNnQ", yesno) == 0));
			
			if ((yesno == 'Y') || (yesno == 'y')) {
				/* if the track is on the player and -f is set, we have to
				 * delete the track first */
				if ((_b_switch_f) && (track_tag)) {
					player_delete_track(player, track_tag);
					track_tag = 0;
				}
				printf(" Sending %s - %s\n", tag->artist, tag->title);
				/* send the data */
				tag->trackid = player_send_file(player, tag);
				/* insert the new track in the player track list so that it
				 * cannot be sent twice in a row */
				tracklist_insert(player_tracklist, tag);
				printf("   Successfully sent %s - %s\n", tag->artist, tag->title);
			}

			if (yesno == 'Q') {
				print_error(G_ABRT);
				break;
			}

		} else {
		/* _b_switch_y was set, so we do not interact with the user but just transfer
		 * any non-existent track to the player */

			/* check if the track is already on the player and skip if so */
			if ((tracklist_find_tag(player_tracklist, tag))) {
				printf(" %s - %s already exists, skipping.\n\n", tag->artist, tag->title);
				file_list = list_remove(file_list);
				continue;
			}
			printf(" Sending %s - %s\n", tag->artist, tag->title);
			tag->trackid = player_send_file(player, tag);
			tracklist_insert(player_tracklist, tag);
			printf("   Successfully sent %s - %s\n", tag->artist, tag->title);
		}			
		printf("\n");

		/* all transfer was fine (or not), but we do not need the s_id3_tag for the
		 * current song any more, so we free it */
		free(tag);
		/* current song is remove from the filename list and the list-pointer is
		 * advanced to the next entry */
		file_list = list_remove(file_list);
	}
	
	/* all player communication done, release the player */
	player_release(&player);

	return 0;
}
