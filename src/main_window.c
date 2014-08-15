#include "main_window.h"

GtkWidget *window;

static void print_hello(GtkWidget *widget, gpointer data) {
	g_print("Hello World\n");
}

void gui_initialize(int *argc, char*** argv) {
	GtkBuilder *builder;
	GObject *button;

	gtk_init(argc, argv);
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "gui.ui", NULL);

	window = (GtkWidget*)gtk_builder_get_object(builder, "window");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	button = gtk_builder_get_object(builder, "button1");
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

	button = gtk_builder_get_object(builder, "button2");
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

	button = gtk_builder_get_object(builder, "quit");
	g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
}

void gui_show() {
	
	gtk_widget_show_all(window);
	gtk_main();
}
