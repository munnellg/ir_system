#ifndef _IRAPP_H_
#define _IRAPP_H_

#include <gtk/gtk.h>
#include "index.h"

#define IR_APP_TYPE (ir_app_get_type())
#define IR_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IR_APP_TYPE, IrApp))

typedef struct _irapp {
	GtkApplication parent;
} IrApp;

typedef struct _irappclass {
	GtkApplicationClass parent_class;
} IrAppClass;

GType ir_app_get_type();
IrApp *ir_app_new();

#endif /* _IRAPP_H_ */
