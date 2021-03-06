#ifndef GIMMIX_PLAYLIST_H
#define GIMMIX_PLAYLIST_H

#include "gimmix-core.h"
#include "gimmix.h"
#include <string.h>
#include <stdbool.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include <gtk/gtk.h>

/* Initialize the playlist interface, file browser and setup signals */
void gimmix_playlist_init (void);

/* update current playlist depending on the mpd playlist */
void gimmix_update_current_playlist (void);

/* update library */
void gimmix_library_update (void);

#endif
