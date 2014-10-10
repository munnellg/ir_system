#ifndef _IRAPPVIDX_H_
#define _IRAPPVIDX_H_

#include <gtk/gtk.h>
#include "irappwin.h"

#define IR_APP_VIDX_TYPE (ir_app_vidx_get_type())
#define IR_APP_VIDX(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IR_APP_VIDX_TYPE, IrAppVIdx()))

typedef struct _IrAppVIdx IrAppVIdx;
typedef struct _IrAppVIdxClass IrAppVIdxClass;

GType ir_app_vidx_get_type();
IrAppVIdx *ir_app_vidx_new(IrAppWindow *win);

#endif /* _IRAPPVIDX_H_ */
