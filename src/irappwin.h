#ifndef _IRAPPWIN_H_
#define _IRAPPWIN_H_

#include "irapp.h"
#include <gtk/gtk.h>
#include <libgen.h>
#include <unistd.h>

#define IR_APP_WINDOW_TYPE (ir_app_window_get_type())
#define IR_APP_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IR_APP_WINDOW_TYPE, IrAppWindow))

typedef struct _irappwindow {
	GtkApplicationWindow parent;
} IrAppWindow;

typedef struct _irappwindowclass {
	GtkApplicationWindowClass parent_class;
} IrAppWindowClass;

GType ir_app_window_get_type();
IrAppWindow *ir_app_window_new(IrApp *app);
void ir_app_window_open(IrAppWindow *win, GFile *file);
void ir_app_window_list_files_append(IrAppWindow *win, GSList *files);
void ir_app_window_list_files_replace(IrAppWindow *win, GSList *files);
void ir_app_window_load_files_into_index(IrAppWindow *win, GSList *files);
GSList *ir_app_window_get_files(IrAppWindow *win);

#endif /* _IRAPPWIN_H_ */
