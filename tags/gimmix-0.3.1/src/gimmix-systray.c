/*
 * gimmix-systray.c
 *
 * Copyright (C) 2006 Priyank Gosalia
 *
 * Gimmix is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Gimmix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with Gimmix; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Priyank Gosalia <priyankmg@gmail.com>
 */
 
#include "gimmix-core.h"
#include "gimmix-systray.h"

#define GIMMIX_ICON  	"gimmix_logo_small.png"

EggTrayIcon			*icon = NULL;
GtkTooltips			*icon_tooltip = NULL;
extern GtkWidget 	*progress;

extern MpdObj		*gmo;
extern GladeXML 	*xml;
extern ConfigFile	conf;

extern GtkWidget	*volume_scale;

static void 	gimmix_systray_display_popup_menu (void);

/* system tray popup menu callbacks */
static void		cb_systray_popup_play_clicked (GtkMenuItem *menuitem, gpointer data);
static void		cb_systray_popup_stop_clicked (GtkMenuItem *menuitem, gpointer data);
static void		cb_systray_popup_next_clicked (GtkMenuItem *menuitem, gpointer data);
static void		cb_systray_popup_prev_clicked (GtkMenuItem *menuitem, gpointer data);
static void		cb_systray_popup_quit_clicked (GtkMenuItem *menuitem, gpointer data);

static gboolean	cb_gimmix_systray_icon_clicked (GtkWidget *widget, GdkEventButton *event, gpointer data);
static void		cb_systray_volume_scroll (GtkWidget *widget, GdkEventScroll *event);

void
gimmix_create_systray_icon (void)
{
	gchar 		*icon_file;
	GdkPixbuf	*icon_image;
	GtkWidget	*systray_icon;
	
	icon_file = gimmix_get_full_image_path (GIMMIX_ICON);
	/* create the tray icon */
	icon = egg_tray_icon_new (APPNAME);
	icon_image = gdk_pixbuf_new_from_file_at_size (icon_file, 20, 20, NULL);
	systray_icon = gtk_image_new_from_pixbuf (icon_image);
	gtk_container_add (GTK_CONTAINER (icon), systray_icon);
	g_free (icon_file);
	g_object_unref (icon_image);
	icon_tooltip = gtk_tooltips_new ();
	/* set the default tooltip */
	gtk_tooltips_set_tip (icon_tooltip, GTK_WIDGET(icon), APPNAME, NULL);
	
	g_signal_connect (icon, "button-press-event", G_CALLBACK (cb_gimmix_systray_icon_clicked), NULL);
	g_signal_connect (icon, "scroll_event", G_CALLBACK(cb_systray_volume_scroll), NULL);
	gtk_widget_show (GTK_WIDGET(systray_icon));
	gtk_widget_show (GTK_WIDGET(icon));

	return;
}

static gboolean
cb_gimmix_systray_icon_clicked (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	switch (event->button)
	{
		case 1: gimmix_window_visible_toggle ();
				break;
		case 3: gimmix_systray_display_popup_menu ();
				break;
		default: break;
	}
	
	return TRUE;
}

static void
cb_systray_volume_scroll (GtkWidget *widget, GdkEventScroll *event)
{
	gint volume;
	GtkAdjustment *volume_adj;
	if (event->type != GDK_SCROLL)
		return;
	
	volume_adj = gtk_range_get_adjustment (GTK_RANGE(volume_scale));
	switch (event->direction)
	{
		case GDK_SCROLL_UP:
			volume = gtk_adjustment_get_value (GTK_ADJUSTMENT(volume_adj)) + 2;
			gtk_adjustment_set_value (GTK_ADJUSTMENT (volume_adj), volume);
			break;
		case GDK_SCROLL_DOWN:
			volume = gtk_adjustment_get_value (GTK_ADJUSTMENT(volume_adj)) - 2;
			gtk_adjustment_set_value (GTK_ADJUSTMENT(volume_adj), volume);
			break;
		default:
			return;
	}
	
	return;
}

static void
gimmix_systray_display_popup_menu (void)
{
	GtkWidget *menu, *menu_item;

	menu = gtk_menu_new();

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gimmix_about_show), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);
	
	if (gimmix_get_status(gmo) == PLAY)
	{
		menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_PAUSE, NULL);
	}
	else
	{
		menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_PLAY, NULL);
	}
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (cb_systray_popup_play_clicked), NULL);
	
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_STOP, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (cb_systray_popup_stop_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (cb_systray_popup_prev_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_NEXT, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (cb_systray_popup_next_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
	g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (cb_systray_popup_quit_clicked), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	gtk_widget_show (menu_item);

	gtk_widget_show (menu);
	gtk_menu_popup (GTK_MENU(menu), NULL, NULL, NULL, NULL, 1, gtk_get_current_event_time());
	
	return;
}


static void
cb_systray_popup_play_clicked (GtkMenuItem *menuitem, gpointer data)
{
	gimmix_play (gmo);
	
	return;
}

static void
cb_systray_popup_stop_clicked (GtkMenuItem *menuitem, gpointer data)
{
	gimmix_stop (gmo);
	
	return;
}

static void
cb_systray_popup_prev_clicked (GtkMenuItem *menuitem, gpointer data)
{
	gimmix_prev (gmo);

	return;
}

static void
cb_systray_popup_next_clicked (GtkMenuItem *menuitem, gpointer data)
{
	gimmix_next (gmo);

	return;
}

static void
cb_systray_popup_quit_clicked (GtkMenuItem *menuitem, gpointer data)
{
	gimmix_save_window_pos ();
	gtk_main_quit ();
	
	return;
}

void
gimmix_disable_systray_icon (void)
{
	if (icon == NULL)
		return;

	gtk_widget_hide (GTK_WIDGET(icon));
	cfg_add_key (&conf, "enable_systray", "false");
	
	return;
}

void
gimmix_enable_systray_icon (void)
{
	if (icon == NULL)
	{	
		gimmix_create_systray_icon ();
		cfg_add_key (&conf, "enable_systray", "true");
	}
	else
	{
		gtk_widget_show (GTK_WIDGET(icon));
	}
	return;
}

void
gimmix_destroy_systray_icon (void)
{
	if (icon == NULL)
		return;
	gtk_widget_destroy (GTK_WIDGET(icon));
	
	if (!icon_tooltip)
		return;
		
	g_object_ref_sink (icon_tooltip);
	icon = NULL;
	icon_tooltip = NULL;
	
	return;
}	


void
gimmix_update_systray_tooltip (SongInfo *s)
{
	gchar 	*summary = NULL;
	
	if (icon == NULL)
		return;
		
	if (s == NULL)
	{
		gtk_tooltips_set_tip (icon_tooltip, GTK_WIDGET(icon), APPNAME, NULL);
		return;
	}
	
	if (s->title != NULL)
	{
		if (s->artist != NULL)
			summary = g_strdup_printf ("%s\n%s - %s", APPNAME, s->title, s->artist);
		else
			summary = g_strdup_printf ("%s\n%s", APPNAME, s->title);
	}
	else
	{	
		gchar *file;
		file = g_path_get_basename (s->file);
		summary = g_strdup_printf ("%s", file);
		g_free (file);
	}
	
	gtk_tooltips_set_tip (icon_tooltip, GTK_WIDGET(icon), summary, NULL);
	g_free (summary);
	
	return;
}

