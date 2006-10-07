/*
 * gimmixcore.c
 *
 * Copyright (C) 2006 Priyank Gosalia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */

#include "gimmixcore.h"

MpdObj * gimmix_mpd_connect(Conf *conf)
{
	MpdObj *mo;

	mo = mpd_new(conf->hostname, conf->port, conf->password);
	//mo = mpd_new_default();
	mpd_connect(mo);

	if(mpd_check_connected(mo))
		return mo;

	fprintf(stderr, "Error! Couldn't connect to mpd. Check whether mpd is running.\n");
	return NULL;
}

bool gimmix_is_playing(MpdObj *mo)
{
	int status;
	mpd_status_update(mo);
	status = mpd_player_get_state(mo);
	
	if(status == MPD_PLAYER_PAUSE || status == MPD_PLAYER_PLAY)
		return true;
	else
		return false;
}

int gimmix_play(MpdObj *mo)
{
	mpd_status_update(mo);
	if(mpd_playlist_get_playlist_length(mo))
	{
		int state;

		state = mpd_player_get_state(mo);

		if (state == MPD_PLAYER_PAUSE || state == MPD_PLAYER_STOP)
		{	
			mpd_player_play(mo);
			return 0;
		}

		else if(state == MPD_PLAYER_PLAY)
		{	
			mpd_player_pause(mo);
			return 1;
		}
	}
	return -1;
}

int gimmix_stop(MpdObj *mo)
{
	int state;

	mpd_status_update(mo);
	state = mpd_player_get_state(mo);
	
	if (state == MPD_PLAYER_PAUSE || state == MPD_PLAYER_PLAY)
	{
		mpd_player_stop(mo);
		return 0;
	}
	return 1;
}

int gimmix_prev(MpdObj *mo)
{
	/* does nothing for now */
	return 0;
}

int gimmix_next(MpdObj *mo)
{
	/* does nothing for now */
	return 0;
}

int gimmix_seek(MpdObj *mo, int seektime)
{
	int state;
	
	state = mpd_player_get_state(mo);
	if(state == MPD_PLAYER_PLAY || state == MPD_PLAYER_PAUSE)
	{
		mpd_player_seek(mo, seektime);
		return 1;
	}
	
	return 0;
}

void gimmix_set_volume(MpdObj *mo, int vol)
{
	mpd_status_set_volume(mo, vol);
}

int gimmix_get_volume(MpdObj *mo)
{
	int volume;
	volume = mpd_status_get_volume(mo);
	return volume;
}

int gimmix_get_total_song_time(MpdObj *mo)
{
	int time;
	time = mpd_status_get_total_song_time(mo);
	return time;
}

SongInfo * gimmix_get_song_info(MpdObj *mo)
{
	mpd_Song *ms;
	SongInfo *s = (SongInfo *)malloc(sizeof(SongInfo));

	ms = mpd_playlist_get_current_song(mo);
	s->file = ms->file;
	s->title = ms->title;
	s->artist = ms->artist;
	s->album = ms->album;
	s->genre = NULL;
	s->length = 0;
	//mpd_freeSong(ms);
	
	return s;
}

SongInfo * gimmix_get_full_song_info(MpdObj *mo)
{
	mpd_Song *ms;
	SongInfo *fsi = NULL;
	
	ms = mpd_playlist_get_current_song(mo);
	fsi->file = strdup(ms->file);
	fsi->title = strdup(ms->title);
	fsi->artist = strdup(ms->artist);
	fsi->album = strdup(ms->album);
	fsi->genre = strdup(ms->genre);
	//fsi->length = ms->time;
	mpd_freeSong(ms);
	return fsi;
}

int gimmix_get_progress_status(MpdObj *mo, float *fraction, char *time)
{
	int state;
	int total, elapsed;
		
	state = mpd_player_get_state(mo);
	
	switch(state)
	{
		case MPD_PLAYER_PLAY:
		case MPD_PLAYER_PAUSE:
			mpd_status_update(mo);
			total = mpd_status_get_total_song_time(mo);
			elapsed = mpd_status_get_elapsed_song_time(mo);
			snprintf(time, 20, "%02i:%02i / %02i:%02i", elapsed/60,
					elapsed%60,
					total/60,
					total%60);
			*fraction = (float)((float)elapsed/(float)total);
			break;

		case MPD_PLAYER_STOP:
		case MPD_PLAYER_UNKNOWN:
			return 0;
	}
	return 1;
}

