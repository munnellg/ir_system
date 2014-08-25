 #include <gtk/gtk.h>

#include "irapp.h"
#include "irappwin.h"
#include "irapploading.h"

typedef struct _IrAppLoadingPrivate{
	GtkWidget *loadingbar;
	GtkWidget *loadingquit;
} IrAppLoadingPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IrAppLoading, ir_app_loading, GTK_TYPE_DIALOG)

static void ir_app_loading_init(IrAppLoading *loading) {
	IrAppLoadingPrivate *priv;
	
	priv = ir_app_loading_get_instance_private(loading);
	gtk_widget_init_template(GTK_WIDGET(priv));	
}

static void ir_app_loading_dispose(GObject *object) {
	G_OBJECT_CLASS(ir_app_loading_parent_class)->dispose(object);
}

static void ir_app_loading_class_init(IrAppLoadingClass *class) {
	G_OBJECT_CLASS(class)->dispose = ir_app_loading_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/home/gary/programming/ir_system/loading.ui");
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), IrAppLoading, loadingbar);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), IrAppLoading, loadingquit);
}

IrAppLoading * ir_app_loading_new (IrAppWindow *win)
{
	return g_object_new (IR_APP_LOADING_TYPE, "transient-for", win, "use-header-bar", FALSE, NULL);
}
