#include "irappwin.h"

typedef struct _IrAppWindowPrivate {
	GtkWidget *view;
	GtkWidget *list;
	GtkWidget *searchtext;
	GtkWidget *lbar;
} IrAppWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IrAppWindow, ir_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void clear_search(GtkEntry *entry, gpointer user_data) {
	IrAppWindow *win;
	GSList *fnames;

	fnames = NULL;
	win = IR_APP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (entry)));
	fnames = i_docs_get_list();	
	ir_app_window_list_files_replace(win, fnames);
	gtk_entry_set_text(entry, "");
}

static void execute_search(GtkEntry *entry, gpointer user_data) {
	char *query;
	GSList *docs, *p;
	IrAppWindow *win;
	char *data;
	
	win = IR_APP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (entry)));
	query = (char*)gtk_entry_get_text(entry);
	docs = i_index_query_boolean_and(query);
	
	for(p = docs; p!=NULL; p=p->next) {
		data = (char *) p->data;
	}
	ir_app_window_list_files_replace(win, docs);
}

static void search_icon_release_activated(GtkEntry *entry, GtkEntryIconPosition pos, GdkEvent *event, gpointer user_data) {
	if (pos == GTK_ENTRY_ICON_PRIMARY) {
		clear_search(entry, user_data);
	} else {
		execute_search(entry, user_data);
	}
}

static gboolean search_keypress_activated(GtkEntry *entry, GdkEventKey *event, gpointer user_data) {
	if(event->keyval == 65293) {
		execute_search(entry, user_data);		
	}
	return FALSE;
}

static void open_selected_file(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
	GtkWidget *label;
	IrAppWindow *win;	
	GFile *f;

	win = IR_APP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (list_box)));
	label = gtk_bin_get_child(GTK_BIN(row));
	f = g_file_new_for_path(gtk_widget_get_tooltip_text(label));	
	ir_app_window_open(win, f);
	g_object_unref(f);
}

static void ir_app_window_init(IrAppWindow *win) {
	gtk_widget_init_template(GTK_WIDGET(win));
}

static void ir_app_window_class_init(IrAppWindowClass *class) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/home/gary/programming/ir_system/window.ui");

	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), IrAppWindow, view);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), IrAppWindow, list);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), IrAppWindow, searchtext);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), IrAppWindow, lbar);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), open_selected_file);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), search_icon_release_activated);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), search_keypress_activated);
}

IrAppWindow *ir_app_window_new(IrApp *app) {
	return g_object_new(IR_APP_WINDOW_TYPE, "application", app, NULL);
}

GSList* ir_app_window_get_files(IrAppWindow *win) {
	GtkWidget *dialog;
	GSList *l;
	
	l = NULL;	
	
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
		l = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
	}
	
	gtk_widget_hide(dialog);
	gtk_widget_destroy (dialog);

	return l;
}

void ir_app_window_load_files_into_index(IrAppWindow *win, GSList *list) {
	IrAppWindowPrivate *priv;
	GSList *p;
	int len;
	int i;
	
	len= g_slist_length(list);
	p = list;
	priv = ir_app_window_get_instance_private(win);
	gtk_widget_set_visible(GTK_WIDGET(priv->lbar), TRUE);
	for(p=list; p!=NULL; p=p->next) {
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->lbar), (float)++i/(float)len);
		i_index_file(p->data);
	}
	gtk_widget_set_visible(GTK_WIDGET(priv->lbar), FALSE);
}

void ir_app_window_list_files_append(IrAppWindow *win, GSList *files) {
	GtkWidget *list_item;
	GSList *p;
	IrAppWindowPrivate *priv;
	gchar *fname;

	priv = ir_app_window_get_instance_private(win);
	p = files;

	while(p) {
		fname = basename((char*)p->data);
		list_item = gtk_label_new(fname);
		
		gtk_widget_set_tooltip_text(list_item, (gchar*)p->data);
		gtk_label_set_justify(GTK_LABEL(list_item), GTK_JUSTIFY_LEFT);
		gtk_widget_set_hexpand(list_item, TRUE);
		gtk_list_box_insert (GTK_LIST_BOX(priv->list), list_item, -1);
		gtk_widget_set_visible(list_item, TRUE);
		p = p->next;
	}
}

void ir_app_window_list_files_replace(IrAppWindow *win, GSList *files) {
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

void ir_app_window_open(IrAppWindow *win, GFile *file) {
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
