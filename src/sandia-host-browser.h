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

#ifndef _SANDIA_HOST_BROWSER_H_
#define _SANDIA_HOST_BROWSER_H_

#include <glib-object.h>
#include <gtk/gtkframe.h>

G_BEGIN_DECLS

#define SANDIA_TYPE_HOST_BROWSER             (sandia_host_browser_get_type ())
#define SANDIA_HOST_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SANDIA_TYPE_HOST_BROWSER, SandiaHostBrowser))
#define SANDIA_HOST_BROWSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SANDIA_TYPE_HOST_BROWSER, SandiaHostBrowserClass))
#define SANDIA_IS_HOST_BROWSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SANDIA_TYPE_HOST_BROWSER))
#define SANDIA_IS_HOST_BROWSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SANDIA_TYPE_HOST_BROWSER))
#define SANDIA_HOST_BROWSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SANDIA_TYPE_HOST_BROWSER, SandiaHostBrowserClass))

typedef struct _SandiaHostBrowserClass SandiaHostBrowserClass;
typedef struct _SandiaHostBrowser SandiaHostBrowser;

struct _SandiaHostBrowserClass
{
	GtkFrameClass parent_class;
};

struct _SandiaHostBrowser
{
	GtkFrame parent_instance;
	gpointer priv;
};

GType sandia_host_browser_get_type (void) G_GNUC_CONST;

GtkWidget *sandia_host_browser_new (gpointer w);

G_END_DECLS

#endif /* _SANDIA_HOST_BROWSER_H_ */
