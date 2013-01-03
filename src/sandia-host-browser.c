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

#include <gtk/gtkwidget.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtktoolbar.h>
#include <gtk/gtktoolitem.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkmessagedialog.h>
#include <gtk/gtktreeview.h>
#include <gtk/gtkstock.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcellrendererpixbuf.h>

#include <gio/gio.h>

#include <glib-object.h>
#include <gconf/gconf-client.h>

#include <string.h>

#include "common.h"
#include "sandia-host-browser.h"

enum
{
	NAME = 0,
	ICON,
	SIZE,
	N_COLS
};

enum
{
	PROP_0,
	PROP_MAIN_WINDOW
};

typedef struct _SandiaHostBrowserPrivate SandiaHostBrowserPrivate;
struct _SandiaHostBrowserPrivate
{
	GtkWidget *scroll;
	GtkWidget *align;
	GtkWidget *vbox;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkWidget *toolbar;
	GtkToolItem *up_button;
	GtkToolItem *home_button;
	GtkToolItem *show_hidden;
	GtkWidget *label;
	GtkWidget *path_entry;
	
	GtkWidget *list_tree_view;
	GtkListStore *list_model;

	gchar *current_path;
	gpointer main_window;

	GConfClient *conf;
};

#define SANDIA_HOST_BROWSER_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SANDIA_TYPE_HOST_BROWSER, SandiaHostBrowserPrivate))



G_DEFINE_TYPE (SandiaHostBrowser, sandia_host_browser, GTK_TYPE_FRAME);

static void
sandia_host_browser_get_property (GObject    *object,
                                  guint       param_id,
                                  GValue     *value,
                                  GParamSpec *pspec);
static void
sandia_host_browser_set_property (GObject    *object,
                                  guint       param_id,
                                  const GValue     *value,
                                  GParamSpec *pspec);
static void on_show_hidden_toggle (GtkToggleButton *button, gpointer data);
static void on_show_hidden (GConfClient *client,
                            guint cnxn_id,
                            GConfEntry *entry,
                            gpointer data);
static void on_up_button (GtkToolButton *button, gpointer data);
static void on_home_button (GtkToolButton *button, gpointer data);
static void on_press_enter (GtkEntry *entry, gpointer data);
static void on_activate_row (GtkTreeView *view,
                             GtkTreePath *path,
                             GtkTreeViewColumn *col,
                             gpointer data);
static void change_sort (GtkTreeViewColumn *col, gpointer data);
static gint sandia_host_browser_sort_by_name (GtkTreeModel *model,
                                              GtkTreeIter *a,
                                              GtkTreeIter *b,
                                              gpointer data);
static void sandia_host_browser_fill_model (SandiaHostBrowser *object);

static void
sandia_host_browser_init (SandiaHostBrowser *object)
{
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE (object);

	object->priv = priv;

	/* GConf */
	priv->conf = gconf_client_get_default();
	gconf_client_add_dir (priv->conf,
	                      SHOW_HIDDEN_DIR,
	                      GCONF_CLIENT_PRELOAD_NONE, NULL);

	/* Initialization */
	priv->vbox = gtk_vbox_new (FALSE, 14);
	priv->hbox1 = gtk_hbox_new (FALSE, 14);
	priv->hbox2 = gtk_hbox_new (FALSE, 8);
	priv->toolbar = gtk_toolbar_new();
	priv->align = gtk_alignment_new (0, 0, 1, 1);
	priv->scroll = gtk_scrolled_window_new (NULL, NULL);

	priv->home_button = gtk_tool_button_new_from_stock (GTK_STOCK_HOME);
	priv->up_button = gtk_tool_button_new_from_stock (GTK_STOCK_GO_UP);
	priv->show_hidden = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_ADD);
	priv->label = gtk_label_new (_("Location:"));
	priv->path_entry = gtk_entry_new();
	
	priv->list_tree_view = gtk_tree_view_new();

	GtkTreeViewColumn *col_name = gtk_tree_view_column_new();
	GtkTreeViewColumn *col_size = gtk_tree_view_column_new();

	gtk_tree_view_column_set_title (col_name, "Filename");
	gtk_tree_view_column_set_title (col_size, "Size");

	gtk_tree_view_column_set_expand (col_name, TRUE);
	gtk_tree_view_column_set_clickable (col_name, TRUE);
	gtk_tree_view_column_set_sort_indicator (col_name, TRUE);
	gtk_tree_view_column_set_reorderable (col_name, TRUE);
	gtk_tree_view_column_set_resizable (col_name, TRUE);
	gtk_tree_view_column_set_sort_column_id (col_name, NAME);
	gtk_tree_view_column_set_expand (col_size, FALSE);
	gtk_tree_view_column_set_resizable (col_size, TRUE);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->list_tree_view),
	                             col_name);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->list_tree_view),
	                             col_size);
	
	GtkCellRenderer *pixbuf_cell = gtk_cell_renderer_pixbuf_new();
	GtkCellRenderer *text_cell = gtk_cell_renderer_text_new();
	
	gtk_tree_view_column_pack_start (col_name, pixbuf_cell, FALSE);
	gtk_tree_view_column_pack_start (col_name, text_cell, TRUE);

	gtk_tree_view_column_add_attribute (col_name, pixbuf_cell,
	                                    "gicon", ICON);
	gtk_tree_view_column_add_attribute (col_name, text_cell,
	                                    "text", NAME);

	text_cell = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start (col_size, text_cell, FALSE);
	gtk_tree_view_column_set_attributes (col_size, text_cell,
	                                     "text", SIZE, NULL);

	priv->current_path = g_strdup (g_get_home_dir());
	priv->list_model = gtk_list_store_new (N_COLS,
	                                       G_TYPE_STRING,
	                                       G_TYPE_ICON,
	                                       G_TYPE_UINT64);
	gtk_tree_view_set_model (GTK_TREE_VIEW(priv->list_tree_view),
	                         GTK_TREE_MODEL(priv->list_model));


	/* Packing */
	gtk_box_pack_start (GTK_BOX(priv->hbox2), priv->label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(priv->hbox2), priv->path_entry, TRUE, TRUE, 0);

	gtk_toolbar_insert (GTK_TOOLBAR(priv->toolbar),
	                    GTK_TOOL_ITEM(priv->home_button), -1);
	gtk_toolbar_insert (GTK_TOOLBAR(priv->toolbar),
	                    GTK_TOOL_ITEM(gtk_separator_tool_item_new()), -1);
	gtk_toolbar_insert (GTK_TOOLBAR(priv->toolbar),
	                    GTK_TOOL_ITEM(priv->up_button), -1);
	gtk_toolbar_insert (GTK_TOOLBAR(priv->toolbar),
	                    GTK_TOOL_ITEM(gtk_separator_tool_item_new()), -1);
	gtk_toolbar_insert (GTK_TOOLBAR(priv->toolbar),
	                    GTK_TOOL_ITEM(priv->show_hidden), -1);

	gtk_box_pack_start (GTK_BOX(priv->hbox1), priv->toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(priv->hbox1), priv->hbox2, TRUE, TRUE, 0);

	gtk_box_pack_start (GTK_BOX(priv->vbox), priv->hbox1, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER(priv->scroll), priv->list_tree_view);
	gtk_box_pack_start (GTK_BOX(priv->vbox), priv->scroll, TRUE, TRUE, 0);
	
	gtk_container_add (GTK_CONTAINER(priv->align), priv->vbox);
	gtk_container_add (GTK_CONTAINER(object), priv->align);

	/* Initial setup */
	gtk_frame_set_label (GTK_FRAME(object), _("<b>Host</b>"));
	GtkWidget *label = gtk_frame_get_label_widget (GTK_FRAME(object));
	gtk_label_set_use_markup (GTK_LABEL(label), TRUE);
	gtk_frame_set_shadow_type (GTK_FRAME(object), GTK_SHADOW_NONE);

	gtk_alignment_set_padding (GTK_ALIGNMENT(priv->align),
	                           6, 0, 12, 0);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(priv->scroll),
	                                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(priv->scroll),
	                                     GTK_SHADOW_ETCHED_IN);

	gtk_toolbar_set_show_arrow (GTK_TOOLBAR(priv->toolbar), FALSE);
	gtk_tool_item_set_is_important (priv->up_button, TRUE);

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(priv->list_tree_view), TRUE);

	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE(priv->list_model),
	                                 NAME,
	                                 sandia_host_browser_sort_by_name,
	                                 NULL,
	                                 NULL);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(priv->list_model),
	                                      NAME,
	                                      GTK_SORT_ASCENDING);

	if (gconf_client_get_bool (priv->conf, SHOW_HIDDEN_KEY, NULL))
		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(priv->show_hidden),
		                                   TRUE);

	gconf_client_notify_add (priv->conf,
	                         SHOW_HIDDEN_KEY,
	                         (GConfClientNotifyFunc)on_show_hidden,
	                         object, NULL, NULL);

	g_signal_connect (G_OBJECT(priv->show_hidden),
	                  "toggled",
	                  G_CALLBACK(on_show_hidden_toggle),
	                  object);

	/* Signals connections */
	g_signal_connect (G_OBJECT(priv->up_button), "clicked",
	                  G_CALLBACK(on_up_button), object);
	g_signal_connect (G_OBJECT(priv->home_button), "clicked",
	                  G_CALLBACK(on_home_button), object);
	g_signal_connect (G_OBJECT(priv->path_entry), "activate",
	                  G_CALLBACK(on_press_enter), object);
	g_signal_connect (G_OBJECT(priv->list_tree_view), "row-activated",
	                  G_CALLBACK(on_activate_row), object);

	/* Init */
	on_home_button (GTK_TOOL_BUTTON(priv->home_button), object);
}

static void
sandia_host_browser_finalize (GObject *object)
{
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(object);
	g_free (priv->current_path);

	gconf_client_remove_dir (priv->conf, SHOW_HIDDEN_DIR, NULL);
	g_object_unref (priv->conf);
	g_object_unref (priv->list_model);

	G_OBJECT_CLASS (sandia_host_browser_parent_class)->finalize (object);
}

static void
sandia_host_browser_class_init (SandiaHostBrowserClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GtkFrameClass* parent_class = GTK_FRAME_CLASS (klass);

	g_type_class_add_private (klass, sizeof (SandiaHostBrowserPrivate));

	object_class->finalize = sandia_host_browser_finalize;

	object_class->get_property = sandia_host_browser_get_property;
	object_class->set_property = sandia_host_browser_set_property;
	
	g_object_class_install_property (object_class,
	                                 PROP_MAIN_WINDOW,
	                                 g_param_spec_pointer ("main-window",
	                                                       "Main Window",
	                                                       "Pointer to the main container",
	                                                       G_PARAM_CONSTRUCT_ONLY |
	                                                       G_PARAM_READWRITE));
}

static void
sandia_host_browser_get_property (GObject    *object,
                                  guint       param_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(object);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	switch (param_id)
	{
	case PROP_MAIN_WINDOW:
		g_value_set_pointer (value, priv->main_window);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	};
}

static void
sandia_host_browser_set_property (GObject      *object,
                                  guint         param_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(object);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	switch (param_id)
	{
		case PROP_MAIN_WINDOW:
			priv->main_window = g_value_get_pointer (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}

static gint
sandia_host_browser_sort_by_name (GtkTreeModel *model,
                                  GtkTreeIter *a,
                                  GtkTreeIter *b,
                                  gpointer data)
{
	gchar *a_name;
	gchar *b_name;

	gtk_tree_model_get (model, a, NAME, &a_name, -1);
	gtk_tree_model_get (model, b, NAME, &b_name, -1);

	return g_strcmp0 (a_name, b_name);
}

static void
sandia_host_browser_fill_model (SandiaHostBrowser *object)
{
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(object);
	GtkTreeIter iter;

	GFile *parent;
	GFileEnumerator *children;
	GFileInfo *child;
	GError *error = NULL;
	GValue name = {0};
	GValue size = {0};
	GValue icon = {0};
	
	GtkTreeModel *model = GTK_TREE_MODEL(priv->list_model);
	gtk_tree_model_get_iter_first (model, &iter);

	parent = g_file_new_for_path (priv->current_path);
	children =
		g_file_enumerate_children (parent,
		                           "standard::display-name,"
		                           "standard::is-hidden,"
		                           "standard::size,"
		                           "standard::icon,"
		                           "standard::type",
		                           G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		                           NULL, &error);
	if (children == NULL)
	{
		GtkWidget *dialog = NULL;
		switch (error->code)
		{
			case G_IO_ERROR_NOT_FOUND:
				dialog =
					gtk_message_dialog_new_with_markup (priv->main_window,
					                                    GTK_DIALOG_MODAL |
					                                    GTK_DIALOG_DESTROY_WITH_PARENT,
					                                    GTK_MESSAGE_ERROR,
					                                    GTK_BUTTONS_OK,
					                                    "<b><big>Could not find"
					                                    "\"%s\"</big></b>\n",
					                                    priv->current_path);
				break;
			case G_FILE_ERROR_NOTDIR:
				dialog =
					gtk_message_dialog_new_with_markup (priv->main_window,
					                                    GTK_DIALOG_MODAL |
					                                    GTK_DIALOG_DESTROY_WITH_PARENT,
					                                    GTK_MESSAGE_ERROR,
					                                    GTK_BUTTONS_OK,
					                                    "<b><big>\"%s\" is not"
					                                    "a directory.</big></b>",
					                                    priv->current_path);
				break;
		}
		gtk_dialog_run (GTK_DIALOG(dialog));
		gtk_widget_destroy (dialog);
		g_error_free (error);
		error = NULL;
		return;
	}

	/* GConf */
	gboolean show = gconf_client_get_bool (priv->conf,
	                                       SHOW_HIDDEN_KEY,
	                                       NULL);

	g_value_init (&name, G_TYPE_STRING);
	g_value_init (&size, G_TYPE_UINT64);
	g_value_init (&icon, G_TYPE_ICON);

	gtk_tree_view_set_model (GTK_TREE_VIEW(priv->list_tree_view), NULL);
	gtk_list_store_clear (GTK_LIST_STORE(priv->list_model));
	
	while ((child = g_file_enumerator_next_file (children, NULL,
	                                             &error)) != NULL)
	{
		if (!show && g_file_info_get_is_hidden (child))
			continue;
		
		gtk_list_store_append (GTK_LIST_STORE(priv->list_model),
		                       &iter);

		g_value_set_string (&name,
		                    g_file_info_get_attribute_as_string (child,
		                                                         "standard::display-name"));
		g_value_set_uint64 (&size,
		                    g_file_info_get_attribute_uint64 (child,
		                                                      "standard::size"));
		g_value_set_object (&icon,
		                    g_file_info_get_icon (child));
		
		gtk_list_store_set_value (GTK_LIST_STORE(priv->list_model),
		                          &iter, NAME, &name);
		gtk_list_store_set_value (GTK_LIST_STORE(priv->list_model),
		                          &iter, ICON, &icon);
		
		if (G_FILE_TYPE_DIRECTORY != g_file_info_get_attribute_uint32 (child, "standard::type"))
			gtk_list_store_set_value (GTK_LIST_STORE(priv->list_model),
			                          &iter, SIZE, &size);
		
		g_object_unref (child);
	}

	g_object_unref (children);
	g_object_unref (parent);

	gtk_tree_view_set_model (GTK_TREE_VIEW(priv->list_tree_view),
	                         GTK_TREE_MODEL(priv->list_model));
}

static void
on_show_hidden_toggle (GtkToggleButton *button, gpointer data)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(data);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	g_print ("here");
	gconf_client_set_bool (priv->conf,
	                       SHOW_HIDDEN_KEY,
	                       gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(priv->show_hidden)),
	                       NULL);
}

static void
on_show_hidden (GConfClient *client,
                guint cnxn_id,
                GConfEntry *entry,
                gpointer data)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(data);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	g_print ("aqui");
	gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(priv->show_hidden),
	                                   gconf_client_get_bool (priv->conf,
	                                                          SHOW_HIDDEN_KEY,
	                                                          NULL));

	sandia_host_browser_fill_model (data);
}

static void
on_home_button (GtkToolButton *button, gpointer data)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(data);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	g_free (priv->current_path);
	priv->current_path = g_strdup (g_get_home_dir());
	
	GtkEntry *entry = GTK_ENTRY(priv->path_entry);
	gtk_entry_set_text (entry, priv->current_path);
	
	sandia_host_browser_fill_model (s);
}

static void
on_up_button (GtkToolButton *button, gpointer data)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(data);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	gchar *old_path;
	int len, i;
	g_free (priv->current_path);
	priv->current_path =
		g_strdup (gtk_entry_get_text (GTK_ENTRY(priv->path_entry)));

	len = strlen (priv->current_path);
	if (len == 0)
		return;
	
	--len;
	if (priv->current_path[len] == '/')
	{
		priv->current_path[len] = '\0';
		--len;
	}
	
	for (i = len; i > 0 && priv->current_path[i] != '/'; i--)
		;
	priv->current_path[i] = '\0';
	old_path = priv->current_path;
	if (strlen (old_path) == 0)
		priv->current_path = g_strdup ("/");
	else
		priv->current_path = g_strdup (old_path);
	
	g_free (old_path);

	gtk_entry_set_text (GTK_ENTRY(priv->path_entry), priv->current_path);

	sandia_host_browser_fill_model (s);
}

static void
on_press_enter (GtkEntry *entry, gpointer data)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(data);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);

	g_free (priv->current_path);
	priv->current_path =
		g_strdup (gtk_entry_get_text (entry));

	sandia_host_browser_fill_model (s);
}

static void
on_activate_row (GtkTreeView *view,
                 GtkTreePath *path,
                 GtkTreeViewColumn *col,
                 gpointer data)
{
	SandiaHostBrowser *s = SANDIA_HOST_BROWSER(data);
	SandiaHostBrowserPrivate *priv = SANDIA_HOST_BROWSER_PRIVATE(s);
	
	GtkTreeIter it;
	GValue name = {0};

	gtk_tree_model_get_iter (GTK_TREE_MODEL(priv->list_model), &it,
	                         path);

	gtk_tree_model_get_value (GTK_TREE_MODEL(priv->list_model),
	                          &it, NAME, &name);
	const gchar *new_level = g_value_get_string (&name);
	
	int len = strlen (priv->current_path);
	if (priv->current_path[len - 1] != '/')
	{
		gchar *tmp = g_strconcat (priv->current_path, "/", NULL);
		g_free (priv->current_path);
		priv->current_path = tmp;
	}

	gchar *old_path = priv->current_path;
	priv->current_path = g_strconcat (old_path, new_level, NULL);
	g_free (old_path);

	gtk_entry_set_text (GTK_ENTRY(priv->path_entry), priv->current_path);
	sandia_host_browser_fill_model (s);
}

GtkWidget *
sandia_host_browser_new (gpointer w)
{
	return GTK_WIDGET(g_object_new (SANDIA_TYPE_HOST_BROWSER,
	                                "main-window", w, NULL));
}

