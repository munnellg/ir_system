#ifndef _IRAPPWIN_H_
#define _IRAPPWIN_H_

#include "irapp.h"
#include <gtk/gtk.h>
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

#endif /* _IRAPPWIN_H_ */
