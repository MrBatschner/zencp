/***************************************************************************
 * ZenCP - a command line utility for handling Creative Nomad Audio Players
 * ========================================================================
 *
 * player.c - implementation for functions that manage a Creative Player
 *
 * This file provides the implementation of functions used for managing the 
 * Creative Nomad audio player.
 *
 * General note: All API calls to libnjb will be of NJB_<function-name>.
 * Errors within these functions will normally be indictaed with a return
 * code < 0.
 * 
 * This file depends on a recent version (>= 2.0) of libnjb. 
 * Get it from <http://libnjb.sf.net>.
 *
 * Written by:     Thomas Buchner
 * Copyright (c):  2005 by Thomas Buchner
 * GitHub:         https://github.com/MrBatschner/zencp
 *
 ***************************************************************************/

#include "player.h"

/**
 * callback_progress() is a callback function that, guess what, provides for a
 * progress indicator. It is provided to data transfer functions and called for every
 * byte that is sent/received. Only used internally and not exported via the header
 * file.
 * This function will print a message about current progress to the screen whenever
 * the number of transmitted bytes is divisable by 10. 
 */
static int callback_progress(u_int64_t sent, u_int64_t total, const char* buf, unsigned len, void *data) {
	int sent_kb = sent / 1024;
	int total_kb = total / 1024;
	int percentage = (((float)sent / (float)total) * 100);
	
	if ((sent % 10 != 0) && (sent != total)) return 0;  /* return if number of tx bytes is
							       not divisable by 10 */

	printf("   %8d KB of %d KB sent (%2d%%)\r", sent_kb, total_kb, percentage);
	fflush(stdout);

	return 0;
}


int player_discovery(njb_t *njb_array) {
	int i = 0;
	
	if (NJB_Discover(njb_array, _MAX_PLAYERS, &i) == -1) return -1;
	
	return i;
}


njb_t* player_lock(njb_t *njb_array, int n) {
	njb_t *player = 0;

	if (n > _MAX_PLAYERS) return 0;	/* tried to capture a player outside of array borders */
	player = &(njb_array[n]);	/* we want to open the n-th player in the array */

	/* try to open the player: initialize all data pipes and establish player communication */
	if (NJB_Open(player) == -1) {
		NJB_Error_Dump(player, stderr);	/* if it did not work, return 0 and exit */
		return 0;
	}

	/* try to get a lock on the player, we will see the "Docked" info on the display now */
	if (NJB_Capture(player) == -1) {
		NJB_Error_Dump(player, stderr);	/* if it fails, close the player again */
		NJB_Close(player);		/* and exit */
		return 0;
	};
	
	return player;	/* everything went fine and we return a pointer to the captured player */
}


int player_release(njb_t **player) {
	if ((!player) || (!*player)) return 0;

	if (NJB_Release(*player) == -1) {
		NJB_Error_Dump(*player, stderr);
		return 0;
	}

	NJB_Close(*player);	/* no errors can occur within this call :-) */
	*player = 0;
	return 1;
}
		

unsigned long long player_get_disksize(njb_t *player) {
	u_int64_t s = 0;
	u_int64_t f = 0;
	
	if (!player) return 0;

	/* the following call will only return both, disksize and free space, so we will
	 * need code doubling. s stores the disksize, f the free space */
	if (NJB_Get_Disk_Usage(player, &s, &f) == -1) {
		NJB_Error_Dump(player, stderr);
		return 0;
	}

	return (unsigned long long)(s/1024); /* capacity in kB */
}


unsigned long long player_get_diskfree(njb_t *player) {
	u_int64_t s = 0;
	u_int64_t f = 0;
	
	if (!player) return 0;

	if (NJB_Get_Disk_Usage(player, &s, &f) == -1) {
		NJB_Error_Dump(player, stderr);
		return 0;
	}

	return (unsigned long long)(f/1024); /* same as above, but we return f here */
}


const char* player_get_owner(njb_t *player) {
	char *owner = 0;
	if (!player) return 0;
	
	if (!(owner = new_string(NJB_Get_Owner_String(player)))) {
		NJB_Error_Dump(player, stderr);
		return 0;
	}

	return (const char*)owner;
}


const char* player_get_model(njb_t *player) {
	const char *model = 0;
	if (!player) return 0;

	/* the following section will create usable strings from preprocessor definements,
	 * depending on the player model */
	switch (player->device_type) {
		case NJB_DEVICE_NJB1: model = new_string(NJB1_NAME);
			 break;
		case NJB_DEVICE_NJB2: model = new_string(NJB2_NAME);
			 break;
		case NJB_DEVICE_NJB3: model = new_string(NJB3_NAME);
			 break;
		case NJB_DEVICE_NJBZEN: model = new_string(NJBZEN_NAME);
			 break;
		case NJB_DEVICE_NJBZEN2: model = new_string(NJBZEN2_NAME);
			 break;
		case NJB_DEVICE_NJBZENNX: model = new_string(NJBZENNX_NAME);
			 break;
		case NJB_DEVICE_NJBZENXTRA: model = new_string(NJBZENXTRA_NAME);
			 break;
		case NJB_DEVICE_NJBZENTOUCH: model = new_string(NJBZENTOUCH_NAME);
			 break;
		case NJB_DEVICE_DELLDJ: model = new_string(DELLDJ_NAME);
			 break;
		default: model = new_string(UNKNOWN_NAME);
	}

	return model;
}


/**
 * On the Creative player, media files need to be equipped with special frames that contain
 * all needed information about the track, i.e. Artist, Title, Album (the ID3 information) as
 * well as the file size and the length of the track.
 * Formerly that was done by the NJB_Send_File method but now, we have to do it by ourselves.
 * The information is stored in a song-id object.
 */
unsigned int player_send_file(njb_t *player, struct id3_struct *tag) {
	unsigned int track = 0;
	njb_songid_t *songid = 0;
	njb_songid_frame_t *frame = 0;

	/* a lot of fields that are better not NULL, so we check */
	if ((!player) || (!tag)) return 0;
	if ((!tag->filename) || (!tag->title) || (!tag->album) || (!tag->genre) ||
		(!tag->artist) || (tag->time == 0) || (!tag->s_year)) 
		return 0;

	/* create a new player song-id object */
	songid = NJB_Songid_New();
	
	/* add a frame that indicates an MP3 file */
	frame = NJB_Songid_Frame_New_Codec(NJB_CODEC_MP3);
	NJB_Songid_Addframe(songid, frame);

	/* add a frame that indicates the file's size */
	frame = NJB_Songid_Frame_New_Filesize(tag->size);
	NJB_Songid_Addframe(songid, frame);
	
	/* add a frame that contains the (ID3) title of the track */
 	frame = NJB_Songid_Frame_New_Title(tag->title);
	NJB_Songid_Addframe(songid, frame);

	/* add a frame that indicates the (ID3) album */
	frame = NJB_Songid_Frame_New_Album(tag->album);
	NJB_Songid_Addframe(songid, frame);

	/* add a frame that specifies the (ID3) artist */
	frame = NJB_Songid_Frame_New_Artist(tag->artist);
	NJB_Songid_Addframe(songid, frame);

	/* add a frame that specifies the (ID3) genre */
	frame = NJB_Songid_Frame_New_Genre(tag->genre);
        NJB_Songid_Addframe(songid, frame);

	/* add a frame that specifies the (ID3) year */
	frame = NJB_Songid_Frame_New_Year(tag->year);	/* the integer is needed */
	NJB_Songid_Addframe(songid, frame);

	/* add a frame that specifies the (ID3) track number */
	frame = NJB_Songid_Frame_New_Tracknum(tag->trackno);
	NJB_Songid_Addframe(songid, frame);

	/* IMPORTANT: add a frame with information about the track's length */
	frame = NJB_Songid_Frame_New_Length(tag->time);
	NJB_Songid_Addframe(songid, frame);
	
	/* NJB_Send_Track will now send the track (identified by its filename),
	 * together with the song-id to the player and indicate its progress via the
	 * callback_progress function. The referenced track variable will contain the
	 * unique track-ID of that track on the player afterwards. */
	if (NJB_Send_Track (player, tag->filename, songid, callback_progress, NULL, &track) == -1) {
	      NJB_Error_Dump(player, stderr);
	      return 0;
	}

	return track;
}


void player_list_device(njb_t *player, int n) {
	const char *owner = 0, *model = 0;
	
	if (!player) return;
	owner = player_get_owner(player);
	model = player_get_model(player);

	printf("   %2d\t --- %s\t\tOwner: %s \n", n, model, owner);
	printf("\t     Capacity: %llu MB\t", (player_get_disksize(player)/1024));
	printf("\tFree:  %llu MB\n", (player_get_diskfree(player)/1024));
	printf("\t     DeviceID: %d\n\n", player_get_deviceid(player));

	return;
}


/** read the information in player.h 
 * What this function does is to create an ID that can be used to identify the player with
 * the -d switch. The algorithm is simple, not fail proven and I cannot guarantee that
 * two players connceted to the host will have unique IDs.
 */
unsigned int player_get_deviceid(njb_t *player) {
	unsigned int id = 0;
	char *owner = 0;
	int i, k = 0;
	
	if (!player) return 0;

	/* first, get the owner string an sum up the ASCII values of each character */
	owner = (char*)player_get_owner(player);
	for (i = 0; i < strlen(owner); i++) {
		k += (int)owner[i];
	}
	free(owner);

	id = player_get_disksize(player);	/* get the capacity of the player */
	id ^= k;				/* XOR it with the owner */
	id += player_get_diskfree(player);	/* add the free space */
	id ^= player_get_diskfree(player);	/* and do a second XOR with the free space */

	return id;				/* voila l'ID */
}


char* player_extract_frame_string(njb_songid_frame_t *playerframe) {
	char buff[32];	/* buffer needed for int->string conversion, 31 places should be
			   sufficient */
	char *r = 0;
	
	if (!playerframe) return 0;

	/* detect the way, data is encoded in the frame and return a string */
	if (playerframe->type == NJB_TYPE_STRING) {	/* strings wont be converted */
		r = new_string(playerframe->data.strval);
	} else if (playerframe->type == NJB_TYPE_UINT16) {	/* 16bit ints */
		snprintf(buff, 32, "%d", playerframe->data.u_int16_val);
		r = new_string(buff);
	} else if (playerframe->type == NJB_TYPE_UINT32) {	/* 32 bit ints */
		snprintf(buff, 32, "%u", playerframe->data.u_int32_val);
		r = new_string(buff);
	} else {
		return 0;
	}

	return r;
}


int player_delete_track(njb_t *player, s_id3_tag *tag) {
	int r;
	
	if ((!player) || (!tag)) return 0;
	if (tag->trackid == 0) return 0;

	r = NJB_Delete_Track(player, tag->trackid);

	if (!r) return 1;
	return 0;
}


/**
 * The function NJB_Get_Track_Tag() will return an object of type njb_songid_t which contains
 * the information about a certain track on the player. It is desirable to have this information
 * in an object of type s_id3_tag for further processing.
 */
s_id3_tag* player_get_id3_struct(njb_songid_t *playertag) {
	s_id3_tag *tag = 0;
	njb_songid_frame_t *playerframe = 0;

	if (!playertag) return 0;
	tag = (s_id3_tag*)malloc(sizeof(s_id3_tag));	/* allocate a new s_id3_tag */

	tag->filename = 0;	/* we wont need these (for now) so we set them to 0 */
        tag->size     = 0;

	/* search the frame that contains information about the artist and copy its contents
	 * to s_id3_tag */
	playerframe = NJB_Songid_Findframe(playertag, FR_ARTIST);
	tag->artist = player_extract_frame_string(playerframe);
	
	/* do the same with the title */
	playerframe = NJB_Songid_Findframe(playertag, FR_TITLE);
	tag->title  = player_extract_frame_string(playerframe);

	/* ... and the album */
	playerframe = NJB_Songid_Findframe(playertag, FR_ALBUM);
	tag->album  = player_extract_frame_string(playerframe);

	/* ... not to forget the genre */
	playerframe = NJB_Songid_Findframe(playertag, FR_GENRE);
	tag->genre  = player_extract_frame_string(playerframe);
	
	/* we do not need the following for now, so we leave them alone */
	tag->s_year = 0;
	tag->year   = 0;

	tag->trackno  = 0;
	tag->time     = 0;
	tag->frequency = 0;
	tag->bitrate  = 0;
	tag->trackid  = playertag->trid;	/* the track-ID on the player */

	tag->next     = 0;
	tag->element  = 0;

	return tag;
}


unsigned int player_get_tracklist(njb_t *player, s_id3_tag* array[]) {
	unsigned int songs = 0;
	s_id3_tag *tag = 0;
	njb_songid_t *playertag = 0;

	if (!player) return 0;

	/* NJB_Get_Track_Tag() will iterate through a linear list of tracks on the player.
	 * In order to make sure, it will start at the very first track, the following
	 * call is necessary. */
	NJB_Reset_Get_Track_Tag(player);

	/* NJB_Get_Track_Tag() will return the tag of the current song and automatically
	 * advance to the next song in the list. As long as something is returned, we will
	 * process the information. As soon as NULL is returned, we processed all tracks. */
	while ((playertag = NJB_Get_Track_Tag(player))) {
		/* the track information from the player is now in playertag, now it will
		 * be converted to s_id3_tag */
		tag = player_get_id3_struct(playertag);
		songs++;
		
		/* put that new track into the tracklist that is referred to by array
		 * see tracklist.h and tracklist.c for details */
		tracklist_insert(array, tag);
		
		/* we dont need the track information from the player anymore (it has
		 * all been copied to s_id3_tag) so we will free it here */
		NJB_Songid_Destroy(playertag);

		/* tracklist_insert will allocate a new object of type s_id3_tag and make
		 * an exact copy of the one it got, so we can free our tag right here */
		free(tag);
		tag = 0;
		playertag = 0;
	}
	
	return songs;	/* the number of songs processed */
}
