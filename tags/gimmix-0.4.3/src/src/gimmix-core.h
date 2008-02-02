#ifndef GIMMIX_CORE_H
#define GIMMIX_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include "gimmix-config.h"

/* the SongInfo structure */
typedef struct songinfo
{
	char *title;
	char *artist;
	char *album;
	char *genre;
	char *file;
} SongInfo;

typedef enum { 	PLAY = 1,
				PAUSE,
				STOP,
				UNKNOWN,
} GimmixStatus;

/* create a mpd object and connect to mpd using the conf */
MpdObj * gimmix_mpd_connect (void);
void gimmix_disconnect (MpdObj *);

/* playback control */
bool gimmix_play (MpdObj *);
bool gimmix_stop (MpdObj *);
bool gimmix_prev (MpdObj *);
bool gimmix_next (MpdObj *);
bool gimmix_seek (MpdObj *, int);

/* get full image path (returned string should be freed) */
char *gimmix_get_full_image_path (const char *);

/* Gets the information of currently playing song (artist, title, genre.etc)*/
SongInfo * gimmix_get_song_info (MpdObj *);

/* Free memory allocated by gimmix_get_song_info() */
void gimmix_free_song_info (SongInfo *);

/* Get the "elapsed time / total time" as a string */
void gimmix_get_progress_status (MpdObj *, float *, char *);

/* Get the total time in the format "mm:ss" as a string for a mpd_Song * */
void gimmix_get_total_time_for_song (MpdObj *, mpd_Song *, char *);

/* Check mpd status for PLAY/PAUSE/STOP */
GimmixStatus gimmix_get_status (MpdObj *);

/* repeat / shuffle status functions */
bool is_gimmix_repeat (MpdObj *);
bool is_gimmix_shuffle (MpdObj *);

/* strips file extension */
void gimmix_strip_file_ext (char *string);

#endif
