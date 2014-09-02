#ifndef _IRAPPPREFS_H_
#define _IRAPPPREFS_H_

#include <gtk/gtk.h>
#include "irappwin.h"

#define IR_APP_PREFS_TYPE (ir_app_prefs_get_type())
#define IR_APP_PREFS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IR_APP_PREFS_TYPE, IrAppPrefs))

typedef struct _IrAppPrefs      IrAppPrefs;
typedef struct _IrAppPrefsClass IrAppPrefsClass;

GType ir_app_prefs_get_type();
IrAppPrefs *ir_app_prefs_new(IrAppWindow *win);

#endif /* _IRAPPPREFS_H_ */
