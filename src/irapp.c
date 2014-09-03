#include <gtk/gtk.h>

#include "irapp.h"
#include "irappwin.h"
#include "irappprefs.h"

G_DEFINE_TYPE(IrApp, ir_app, GTK_TYPE_APPLICATION)

static void about_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	GtkWindow *win;

	win=gtk_application_get_active_window(GTK_APPLICATION(app));
	gtk_show_about_dialog(win,
												"program-name", "Information Retrieval System",
												"version", "0.1",
												"copyright", "© 2014 Gary Munnelly",
												"website", "https://github.com/munnellg/ir_system",
												"website-label", "Github",
												"license-type", GTK_LICENSE_GPL_3_0,
												"comments", "A simple file curation system providing different \nmethods of searching across and ranking text documents",
												NULL
		);
}

static void preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	IrAppPrefs *prefs;
	GtkWindow *win;

	win = gtk_application_get_active_window(GTK_APPLICATION(app));
	prefs = ir_app_prefs_new(IR_APP_WINDOW(win));
	gtk_window_present(GTK_WINDOW(prefs));
}

static void new_index_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	GtkWindow *win;
		
	win = gtk_application_get_active_window (GTK_APPLICATION (app));
		
  i_index_destroy();
	i_index_initialize();
	ir_app_window_list_files_replace(IR_APP_WINDOW(win), NULL);
}

static void add_to_index_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
	GtkWindow *win;
	GSList *files;
	
	win = gtk_application_get_active_window(GTK_APPLICATION(app));

	files = ir_app_window_get_files(IR_APP_WINDOW(win));
	ir_app_window_load_files_into_index(IR_APP_WINDOW(win), files);
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
	{"about", about_activated, NULL, NULL, NULL},
};

static void ir_app_startup(GApplication *app) {
	GMenu *menu, *submenu, *section;
	
	G_APPLICATION_CLASS(ir_app_parent_class)->startup(app);

	g_action_map_add_action_entries(G_ACTION_MAP(app),
																	app_entries, G_N_ELEMENTS(app_entries),
																	app);

	gtk_application_add_accelerator(GTK_APPLICATION(app), "<Ctrl>N", "app.new_index", NULL);
	gtk_application_add_accelerator(GTK_APPLICATION(app), "<Ctrl>O", "app.add_to_index", NULL);
	gtk_application_add_accelerator(GTK_APPLICATION(app), "<Ctrl>P", "app.preferences", NULL);
	gtk_application_add_accelerator(GTK_APPLICATION(app), "<Ctrl>Q", "app.quit", NULL);

	menu = g_menu_new();

	submenu = g_menu_new();
	section = g_menu_new();
	g_menu_append(section, "_New Index", "app.new_index");
	g_menu_append(section, "_Add To Index", "app.add_to_index");
	g_menu_append(section, "Preferences", "app.preferences");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));
	section = g_menu_new();
	g_menu_append(section, "_Quit", "app.quit");
	g_menu_append_section(submenu, NULL, G_MENU_MODEL(section));	
	g_menu_append_submenu(menu, "File", G_MENU_MODEL(submenu));

	submenu = g_menu_new();
	g_menu_append(submenu, "About", "app.about");
	g_menu_append_submenu(menu, "Help", G_MENU_MODEL(submenu));

	gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu));
}

static void ir_app_init(IrApp *app) {
	GdkPixbuf *pixbuf;
	GSettings *settings;
	GError *error = NULL;
	int stem;
	char* rank;
	
	i_index_initialize();

	pixbuf = gdk_pixbuf_new_from_file("ir_system.png", &error);
	if(!pixbuf) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	} else {
		gtk_window_set_default_icon(pixbuf);
	}
	
	settings = g_settings_new("org.kdeg.irsystem");
	stem = g_variant_get_boolean(g_settings_get_value(settings, "stem"));
	rank = (char*)g_variant_get_string(g_settings_get_value(settings, "rank"), NULL);
	printf("Stemming Enabled: %s\n", stem? "Yes" : "No");
	printf("Ranking Function: %s\n", rank);
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
