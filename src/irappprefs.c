#include "irapp.h"
#include "irappwin.h"
#include "irappprefs.h"

struct _IrAppPrefs {
	GtkDialog parent;
};

struct _IrAppPrefsClass {
	GtkDialogClass parent_class;
};

typedef struct _IrAppPrefsPrivate IrAppPrefsPrivate;

struct _IrAppPrefsPrivate {
	GSettings *settings;
	GtkWidget *stem;
	GtkWidget *rank;
};

G_DEFINE_TYPE_WITH_PRIVATE(IrAppPrefs, ir_app_prefs, GTK_TYPE_DIALOG)

static void ir_app_prefs_dispose(GObject *object) {
	IrAppPrefsPrivate *priv;

	priv = ir_app_prefs_get_instance_private(IR_APP_PREFS(object));
	g_clear_object(&priv->settings);

	G_OBJECT_CLASS(ir_app_prefs_parent_class)->dispose(object);
}

static void ir_app_prefs_init(IrAppPrefs *prefs) {
	GtkWidget *container, *child;
	IrAppPrefsPrivate *priv;
	
	priv = ir_app_prefs_get_instance_private(prefs);
	priv->settings = g_settings_new("org.kdeg.irsystem");

	container = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(container), 12);
	gtk_grid_set_column_spacing(GTK_GRID(container), 6);
	
	/* Build the stem check box */
	child = gtk_label_new("Enable Stemming");
	gtk_grid_attach(GTK_GRID(container), child, 0, 0, 1, 1);
	gtk_widget_set_visible(child, TRUE);

	child = gtk_check_button_new();

	gtk_widget_set_visible(child, TRUE);
	priv->stem = child;
	gtk_widget_set_visible(child, TRUE);
	
	gtk_grid_attach(GTK_GRID(container), child, 1, 0, 1, 1);

	child = gtk_label_new("?");
	gtk_misc_set_alignment(GTK_MISC(child), 0, 0.5);
	gtk_widget_set_tooltip_text(child, "Restart program to apply changes to stem");
	gtk_widget_set_visible(child, TRUE);

	gtk_grid_attach(GTK_GRID(container), child, 2, 0, 1, 1);
	
	/* Build combo box for ranking algorithm */
	child = gtk_label_new("Ranking Function");
	gtk_grid_attach(GTK_GRID(container), child, 0, 1, 1, 1);
	gtk_widget_set_visible(child, TRUE);

	child = gtk_combo_box_text_new();

	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(child), "boolean", "Boolean");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(child), "TF-IDF", "TF-IDF");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(child), "BM25", "BM25");
		
	priv->rank = child;
	
	gtk_widget_set_visible(child, TRUE);

	gtk_grid_attach(GTK_GRID(container), child, 1, 1, 2, 1);

	gtk_window_set_title(GTK_WINDOW(prefs), "Preferences");
	gtk_window_set_modal(GTK_WINDOW(prefs), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(prefs), FALSE);
	gtk_widget_set_visible(container, TRUE);

	child = container;
	
	/* The root container for the window. Holds all other widgets */
	container = gtk_dialog_get_content_area(GTK_DIALOG(prefs));

	gtk_container_add(GTK_CONTAINER(container), child);
	
	/* Make everything visible */
	gtk_widget_set_visible(container, TRUE);
	

	g_settings_bind(priv->settings, "rank",
									priv->rank, "active-id",
									G_SETTINGS_BIND_DEFAULT
									);

	g_settings_bind(priv->settings, "stem",
									priv->stem, "active",
									G_SETTINGS_BIND_DEFAULT
		);
}

static void ir_app_prefs_class_init(IrAppPrefsClass *class) {
	G_OBJECT_CLASS(class)->dispose = ir_app_prefs_dispose;
}

IrAppPrefs *ir_app_prefs_new(IrAppWindow *win) {
	return g_object_new(IR_APP_PREFS_TYPE, "transient-for", win, NULL, "use-header-bar", TRUE, NULL);
}
