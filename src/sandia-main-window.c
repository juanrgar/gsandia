/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * gsandia
 * Copyright (C) jgblanco 2009 <jgblanco.mail@gmail.com>
 * 
 * gsandia is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gsandia is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtkwindow.h>
#include <gtk/gtkmain.h>

#include "sandia-main-window.h"
#include "sandia-host-browser.h"

typedef struct _SandiaMainWindowPrivate SandiaMainWindowPrivate;
struct _SandiaMainWindowPrivate
{
	GtkWidget *window;
	GtkWidget *host_browser;
	GtkWidget *status_bar;
};

#define SANDIA_MAIN_WINDOW_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SANDIA_TYPE_MAIN_WINDOW, SandiaMainWindowPrivate))



G_DEFINE_TYPE (SandiaMainWindow, sandia_main_window, G_TYPE_OBJECT);

static void
sandia_main_window_init (SandiaMainWindow *object)
{
	SandiaMainWindowPrivate *priv = SANDIA_MAIN_WINDOW_PRIVATE (object);
	object->priv = priv;

	GtkWidget *vbox;

	priv->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	priv->host_browser = sandia_host_browser_new (GTK_WINDOW(priv->window));
	priv->status_bar = gtk_statusbar_new ();
	
	gtk_window_resize (GTK_WINDOW(priv->window), 600, 300);
	gtk_window_set_title (GTK_WINDOW(priv->window), "Gsandia");

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(vbox), priv->host_browser, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(vbox), priv->status_bar, FALSE, FALSE, 0);
	
	/*gtk_container_add (GTK_CONTAINER(priv->window), priv->host_browser);*/
	gtk_container_add (GTK_CONTAINER(priv->window), vbox);

	g_signal_connect (G_OBJECT(priv->window), "delete_event",
	                  G_CALLBACK(gtk_main_quit), NULL);
}

static void
sandia_main_window_finalize (GObject *object)
{
	SandiaMainWindowPrivate *priv = SANDIA_MAIN_WINDOW_PRIVATE(object);

	g_object_unref (priv->window);
	g_object_unref (priv->host_browser);

	G_OBJECT_CLASS (sandia_main_window_parent_class)->finalize (object);
}

static void
sandia_main_window_class_init (SandiaMainWindowClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GObjectClass* parent_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (SandiaMainWindowPrivate));

	object_class->finalize = sandia_main_window_finalize;
}

SandiaMainWindow *
sandia_main_window_new()
{
	return g_object_new (SANDIA_TYPE_MAIN_WINDOW, NULL);
}

void
sandia_main_window_show (SandiaMainWindow *object)
{
	SandiaMainWindowPrivate *priv = SANDIA_MAIN_WINDOW_PRIVATE(object);
	gtk_widget_show_all (priv->window);
}