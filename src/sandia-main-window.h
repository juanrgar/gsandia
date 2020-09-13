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

#ifndef _SANDIA_MAIN_WINDOW_H_
#define _SANDIA_MAIN_WINDOW_H_

#include <glib-object.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkvbox.h>

G_BEGIN_DECLS

#define SANDIA_TYPE_MAIN_WINDOW             (sandia_main_window_get_type ())
#define SANDIA_MAIN_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SANDIA_TYPE_MAIN_WINDOW, SandiaMainWindow))
#define SANDIA_MAIN_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SANDIA_TYPE_MAIN_WINDOW, SandiaMainWindowClass))
#define SANDIA_IS_MAIN_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SANDIA_TYPE_MAIN_WINDOW))
#define SANDIA_IS_MAIN_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SANDIA_TYPE_MAIN_WINDOW))
#define SANDIA_MAIN_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SANDIA_TYPE_MAIN_WINDOW, SandiaMainWindowClass))

typedef struct _SandiaMainWindowClass SandiaMainWindowClass;
typedef struct _SandiaMainWindow SandiaMainWindow;

struct _SandiaMainWindowClass
{
	GObjectClass parent_class;
};

struct _SandiaMainWindow
{
	GObject parent_instance;
	gpointer priv;
};

GType sandia_main_window_get_type (void) G_GNUC_CONST;

SandiaMainWindow *sandia_main_window_new();
void sandia_main_window_show (SandiaMainWindow *object);

G_END_DECLS

#endif /* _SANDIA_MAIN_WINDOW_H_ */
