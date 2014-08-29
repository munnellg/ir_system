#ifndef _IRAPPLOADING_H_
#define _IRAPPLOADING_H_

#include <gtk/gtk.h>
#include "irappwin.h"

#define IR_APP_LOADING_TYPE (ir_app_loading_get_type())
#define IR_APP_LOADING(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IR_APP_LOADING_TYPE, IrAppLoading))

typedef struct _IrAppLoading {
	GtkDialog parent;
} IrAppLoading;

typedef struct _IrAppLoadingClass {
	GtkDialogClass parent_class;
} IrAppLoadingClass;

GType ir_app_loading_get_type(void);
IrAppLoading *ir_app_loading_new(IrAppWindow *win);

#endif /* _IRAPPLOADING_H_ */
