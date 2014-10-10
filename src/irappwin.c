#include "irappwin.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 720
#define PANED_POSITION 250

typedef struct _IrAppWindowPrivate {
	GtkWidget *view;
	GtkWidget *list;
	GtkWidget *searchtext;
	GtkWidget *lbar;
} IrAppWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IrAppWindow, ir_app_window, GTK_TYPE_APPLICATION_WINDOW)

static GList *
slist_to_list(GSList *sl) {
	GList *l;

	l = NULL;
	for(; sl!=NULL; sl=sl->next) {
		l = g_list_prepend(l, sl->data);		
	}

	l = g_list_reverse(l);
	
	return l;
}

static void
clear_search(GtkEntry *entry, gpointer user_data) {
	IrAppWindow *win;
	GList *fnames;

	win = IR_APP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (entry)));
	fnames = i_indexer_get_documents();	
	ir_app_window_list_files_replace(win, fnames);
	gtk_entry_set_text(entry, "");
}

static void
execute_search(GtkEntry *entry, gpointer user_data) {
	char *query;
	GList *docs;
	IrAppWindow *win;


	win = IR_APP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (entry)));
	query = (char*)gtk_entry_get_text(entry);
	docs = i_indexer_query(query);
	
	ir_app_window_list_files_replace(win, docs);
}

static void
search_icon_release_activated(GtkEntry *entry, GtkEntryIconPosition pos, GdkEvent *event, gpointer user_data) {

	if (pos == GTK_ENTRY_ICON_PRIMARY) {
		clear_search(entry, user_data);
	} else {
		execute_search(entry, user_data);
	}
}

static gboolean
search_keypress_activated(GtkEntry *entry, GdkEventKey *event, gpointer user_data) {
	if(event->keyval == 65293) {
		execute_search(entry, user_data);		
	}
	return FALSE;
}

static void
open_selected_file(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
	GtkWidget *label;
	IrAppWindow *win;	
	GFile *f;

	win = IR_APP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (list_box)));
	label = gtk_bin_get_child(GTK_BIN(row));
	f = g_file_new_for_path(gtk_widget_get_tooltip_text(label));	
	ir_app_window_open(win, f);
	g_object_unref(f);
}

static void
ir_app_window_init(IrAppWindow *win) {
	GtkWidget *root, *container, *child, *object;
	IrAppWindowPrivate *priv;
	
	priv = ir_app_window_get_instance_private(win);
	
	/* The root container for the window. Holds all other widgets */
	root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	/* Build the search bar */
	child = gtk_entry_new();
	gtk_widget_set_hexpand(child, TRUE);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(child), GTK_ENTRY_ICON_PRIMARY, "edit-delete-symbolic");
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(child), GTK_ENTRY_ICON_SECONDARY, "edit-find-symbolic");
	gtk_entry_set_icon_sensitive(GTK_ENTRY(child), GTK_ENTRY_ICON_PRIMARY, TRUE);
	gtk_entry_set_icon_sensitive(GTK_ENTRY(child), GTK_ENTRY_ICON_SECONDARY, TRUE);
	gtk_entry_set_icon_activatable(GTK_ENTRY(child), GTK_ENTRY_ICON_PRIMARY, TRUE);
	gtk_entry_set_icon_activatable(GTK_ENTRY(child), GTK_ENTRY_ICON_SECONDARY, TRUE);
	gtk_widget_set_visible(child, TRUE);
	priv->searchtext = child;
	g_signal_connect(G_OBJECT(child), "icon-release", G_CALLBACK(search_icon_release_activated), G_OBJECT(child));		
	g_signal_connect(G_OBJECT(child), "key-press-event", G_CALLBACK(search_keypress_activated), G_OBJECT(child));
	
	container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(container), 5);
	gtk_container_add(GTK_CONTAINER(container), child);
	gtk_widget_set_visible(container, TRUE);
	
	gtk_container_add(GTK_CONTAINER(root), container);

	/* Create the display area for the document text */
	object = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(object), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(object), GTK_WRAP_WORD);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(object), FALSE);
	gtk_widget_set_visible(object, TRUE);
	priv->view = object;
	
	child = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_hexpand(child, TRUE);
	gtk_widget_set_vexpand(child, TRUE);
	gtk_container_add(GTK_CONTAINER(child), object);
	gtk_widget_set_visible(child, TRUE);
	
	container = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);	
	gtk_paned_add2(GTK_PANED(container), child);
	gtk_widget_set_visible(container, TRUE);
	
	/* Create the area for listing files that have been curated */
	object = gtk_list_box_new();
	gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(object), FALSE);
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(object), GTK_SELECTION_SINGLE);
	gtk_widget_set_visible(object, TRUE);
	priv->list = object;	
	g_signal_connect(G_OBJECT(object), "row-activated", G_CALLBACK(open_selected_file), G_OBJECT(object));
	
	child = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(child), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(child), object);
	gtk_widget_set_visible(child, TRUE);
	
	gtk_paned_add1(GTK_PANED(container), child);
	gtk_paned_set_position(GTK_PANED(container), PANED_POSITION);
	
	gtk_container_add(GTK_CONTAINER(root), container);

	child = gtk_progress_bar_new();
	gtk_widget_set_visible(child, FALSE);
	priv->lbar = child;
	
	gtk_container_add(GTK_CONTAINER(root), child);
	
	/* Set up the window */
	gtk_container_add(GTK_CONTAINER(win), root);	
	gtk_window_set_title(GTK_WINDOW(win), "Information Retrieval System");
	gtk_window_set_default_size(GTK_WINDOW(win), DEFAULT_WIDTH, DEFAULT_HEIGHT);

	/* Make everything visible */
	gtk_widget_set_visible(root, TRUE);
}

static void
ir_app_window_class_init(IrAppWindowClass *class) {

}

IrAppWindow *
ir_app_window_new(IrApp *app) {
	return g_object_new(IR_APP_WINDOW_TYPE, "application", app, NULL);
}

GList*
ir_app_window_get_files(IrAppWindow *win) {
	GtkWidget *dialog;
	GList  *l;
	GSList *sl;
	
	sl = NULL;	
	
	dialog = gtk_file_chooser_dialog_new ((const gchar*)"Open File",
																				GTK_WINDOW(win),
																				GTK_FILE_CHOOSER_ACTION_OPEN,
																				"Cancel",
																				GTK_RESPONSE_CANCEL,
																				"Open",
																				GTK_RESPONSE_ACCEPT,
																				NULL);

	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		sl = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
	}

	l = slist_to_list(sl);
	g_slist_free(sl);
	
	gtk_widget_hide(dialog);
	gtk_widget_destroy (dialog);

	return l;
}

void
ir_app_window_load_files_into_index(IrAppWindow *win, GList *list) {
	IrAppWindowPrivate *priv;
	GList *p;
	int len;
	int i;
	
	len= g_list_length(list);
	p = list;
	priv = ir_app_window_get_instance_private(win);
	gtk_widget_set_visible(GTK_WIDGET(priv->lbar), TRUE);
	for(p=list; p!=NULL; p=p->next) {
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->lbar), (float)++i/(float)len);
		i_indexer_index_document(p->data);
	}
	gtk_widget_set_visible(GTK_WIDGET(priv->lbar), FALSE);
}

void
ir_app_window_list_files_append(IrAppWindow *win, GList *files) {
	GtkWidget *list_item;
	GList *p;
	IrAppWindowPrivate *priv;
	gchar *fname;

	priv = ir_app_window_get_instance_private(win);
	p = files;

	while(p) {
		fname = basename((char*)p->data);
		list_item = gtk_label_new(fname);

		gtk_misc_set_alignment(GTK_MISC(list_item), 0, 0.5);
		gtk_widget_set_tooltip_text(list_item, (gchar*)p->data);
		gtk_label_set_justify(GTK_LABEL(list_item), GTK_JUSTIFY_LEFT);
		gtk_widget_set_hexpand(list_item, TRUE);
		gtk_list_box_insert (GTK_LIST_BOX(priv->list), list_item, -1);
		gtk_widget_set_visible(list_item, TRUE);
		p = p->next;
	}
}

void
ir_app_window_list_files_replace(IrAppWindow *win, GList *files) {
	GList *children, *p;
	IrAppWindowPrivate *priv;

	priv = ir_app_window_get_instance_private(win);

	children = gtk_container_get_children(GTK_CONTAINER(priv->list));
	for(p = children; p != NULL; p = g_list_next(p)) {
		gtk_widget_destroy(GTK_WIDGET(p->data));
	}

	g_list_free(children);

	ir_app_window_list_files_append(win, files);
}

void
ir_app_window_open(IrAppWindow *win, GFile *file) {
	gchar *basename;
	GtkTextBuffer *buffer;
	IrAppWindowPrivate *priv;
	gchar *contents;
	gsize length;

	priv = ir_app_window_get_instance_private(win);
	basename = g_file_get_basename(file);
		
	if(g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) {
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->view));
		gtk_text_buffer_set_text(buffer, contents, length);
		g_free(contents);
	}

	g_free(basename);
}
