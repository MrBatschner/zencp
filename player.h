/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * player.h - header file with functions for managing a Creative Player
 *
 * This file provides the prototypes of functions used for managing the 
 * Creative Nomad audio player.  
 * 
 * This file depends on a recent version (>= 2.0) of libnjb. 
 * Get it from <http://libnjb.sf.net>.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#ifndef __ZENCP_PLAYER_H
#define __ZENCP_PLAYER_H

#include <libnjb.h>
#include "id3.h"
#include "tracklist.h"
#include "misc.h"

/* the maximum number of players that are concurrently supported */
#define _MAX_PLAYERS 32

#define NJB1_NAME	"Creative Nomad Jukebox"
#define NJB2_NAME	"Creative Nomad Jukebox 2"
#define NJB3_NAME	"Creative Nomad Jukebox 3"
#define NJBZEN_NAME	"Creative Nomad Zen"
#define NJBZEN2_NAME	"Creative Nomad Zen USB2.0"
#define NJBZENNX_NAME	"Creative Nomad Zen NX"
#define NJBZENXTRA_NAME "Creative Nomad Zen XTra"
#define NJBZENTOUCH_NAME "Creative Zen Touch"
#define DELLDJ_NAME	"Dell Music DJ"
#define UNKNOWN_NAME	"Unkown supported device"

/**
 * player_discovery() tries to discover all Creative players on the USB
 * bus. If successful, it will return the number of devices that have been found
 * and njb_array contains the references to them. If an error occured, -1 will be
 * returned.
 */
int	player_discovery(njb_t *njb_array);

/**
 * player_lock() will capture the n-th player in njb_array. Capturing means acquiring
 * a lock on the player, stopping current playback and initializing the data communictation.
 * A Creative Zen Touch will show "Docked" on its display after this function was called
 * and it will behave very strangely if it is not released afterwards. If the player
 * was captured successfully, a reference to it is returned, NULL otherwise. If only one
 * player is connected to the host, it will be at array location 0.
 */
njb_t*	player_lock(njb_t* njb_array, int n);

/**
 * player_release() will release a formerly captured player.
 */
int	player_release(njb_t **player);

/**
 * player_list_device() will print some information about the given player to the
 * screen. Argument n is used in the output to give a number to that specific player
 * and is of no further importance.
 */
void	player_list_device(njb_t *player, int n);

/**
 * player_get_disksize() obtains the size of the disk in the player referenced by player.
 * The size will be returned in bytes and will be 0 if there was an error.
 */
unsigned long long player_get_disksize(njb_t *player);

/**
 * player_get_diskfree() is similar to player_get_disksize() but it returns the amount
 * of free space on the player's disk.
 */
unsigned long long player_get_diskfree(njb_t *player);

/**
 * player_get_owner() will return a string that contains the owner of the player as it has
 * been set in the Settings menu. NULL is returned in case of errors.
 */
const char* player_get_owner(njb_t *player);

/**
 * player_get_model() will return a string that identifies the model of the player.
 * The strings are defined above. NULL is returned in case of errors.
 */
const char* player_get_model(njb_t *player);

/**
 * player_get_deviceid() tries to create a unique player ID that can be used within
 * zencp to identify multiple players. This function might be dangerous by the algorithm
 * that is used, by it worked for now. 0 is returned in case of errors.
 */
unsigned int player_get_deviceid(njb_t *player);

/**
 * player_extract_frame_string() : also on the player, ID3 information about tracks is 
 * stored within frames bit this time, the way to retrieve their contents is different.
 * This function will return a string with the contents of a specific frame and NULL
 * if this was impossible.
 */
char* player_extract_frame_string(njb_songid_frame_t *playerframe);

/**
 * player_get_id3_struct() will retrieve every piece of information out of the given tag
 * and put it into a new element of struct id3_struct. As always, NULL is returned in case
 * of errors.
 */
s_id3_tag* player_get_id3_struct(njb_songid_t *playertag);

/**
 * player_get_tracklist() will retrieve a complete list of tracks from the player and put them
 * into a tracklist. The parameter array[] must point to an array that is used to maintain
 * the tracklist. It returns the number of tracks retrieved from the player.
 */
unsigned int player_get_tracklist(njb_t *player, s_id3_tag* array[]);

/**
 * player_send_file() will send an MP3 file that is represented by tag to the player that is
 * represented by player. It will return the unique track ID of that track when it has been
 * successfully transferred to the player and 0 in case of errors.
 */
unsigned int player_send_file(njb_t *player, struct id3_struct *tag);

/**
 * player_delete_track() will delete the track represented by tag from the given player.
 */
int player_delete_track(njb_t *player, s_id3_tag *tag);

#endif
