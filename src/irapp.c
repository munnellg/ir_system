#include "irapp.h"
#include "irappwin.h"
#include "irapploading.h"

G_DEFINE_TYPE(IrApp, ir_app, GTK_TYPE_APPLICATION)

static void preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	
}

static void new_index_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	GtkWindow *win;
		
	win = gtk_application_get_active_window (GTK_APPLICATION (app));
		
	i_index_destroy();
	i_index_initialize();
	ir_app_window_list_files_replace(IR_APP_WINDOW(win), NULL);
}
/*
static GSList* get_files(GtkWindow *win) {
	GtkWidget *dialog;
	GSList *l;
	
	l = NULL;	
	
	dialog = gtk_file_chooser_dialog_new ((const gchar*)"Open File",
																				win,
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

	gtk_widget_destroy (dialog);
	return l;
}

static void load_files_into_index(GtkWindow *win, GSList *list) {
	GSList *p;
	IrAppLoading *loading;
	
	p = list;
	loading = ir_app_loading_new (IR_APP_WINDOW(win));
	
	gtk_dialog_run(GTK_DIALOG(loading));

	while(p) {
		i_index_file(p->data);
		p = p->next;
	}

	ir_app_window_list_files_append(IR_APP_WINDOW(win), p);
	}*/

static void add_to_index_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	IrAppWindow *win;
	GSList *files;
	
	win = gtk_application_get_active_window(GTK_APPLICATION(app));

	files = ir_app_window_get_files(win);
	ir_app_window_load_files_into_index(win, files);
	ir_app_window_list_files_append(IR_APP_WINDOW(win), files);

	g_slist_free (files);
}

static void quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	g_application_quit(G_APPLICATION(app));
}

static GActionEntry app_entries[] = {
	{"preferences", preferences_activated, NULL, NULL, NULL},
	{"quit", quit_activated, NULL, NULL, NULL},
	{"add_to_index", add_to_index_activated, NULL, NULL, NULL},
	{"new_index", new_index_activated, NULL, NULL, NULL},
};

static void ir_app_startup(GApplication *app) {
	GtkBuilder *builder;
	GMenuModel *app_menu;
	const gchar *quit_accels[2] = {"<Ctrl>Q", NULL};

	G_APPLICATION_CLASS(ir_app_parent_class)->startup(app);

	g_action_map_add_action_entries(G_ACTION_MAP(app),
																	app_entries, G_N_ELEMENTS(app_entries),
																	app);
	gtk_application_set_accels_for_action(GTK_APPLICATION(app),
																				"app.quit",
																				quit_accels);
	builder = gtk_builder_new_from_resource("/home/gary/programming/ir_system/menu.ui");
	app_menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
	gtk_application_set_menubar(GTK_APPLICATION(app), app_menu);
	g_object_unref(builder);
}

static void ir_app_init(IrApp *app) {
	i_index_initialize();
}

static void ir_app_activate(GApplication *app) {
	IrAppWindow *win;

	win = ir_app_window_new(IR_APP(app));
	gtk_window_present(GTK_WINDOW(win));
}

static void ir_app_open(GApplication *app, GFile **files, gint n_files, const gchar *hint) {
	GList *windows;
	IrAppWindow *win;
	int i;

	windows = gtk_application_get_windows(GTK_APPLICATION(app));
	if(windows) {
		win = IR_APP_WINDOW(windows->data);
	} else {
		win = ir_app_window_new(IR_APP(app));
	}

	for(i=0; i<n_files; i++) {
		ir_app_window_open(win, files[i]);
	}

	gtk_window_present(GTK_WINDOW(win));
}

static void ir_app_class_init(IrAppClass *class) {
	G_APPLICATION_CLASS(class)->activate = ir_app_activate;
	G_APPLICATION_CLASS(class)->open = ir_app_open;
	G_APPLICATION_CLASS(class)->startup = ir_app_startup;
}

IrApp *ir_app_new() {
	return g_object_new(IR_APP_TYPE, "application-id", "ie.tcd.scss.kdeg.irapp", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
