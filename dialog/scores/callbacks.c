#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"


void
on_window_score_activate_default       (GtkWindow       *window,
                                        gpointer         user_data)
{

}

gboolean 
on_button_ok_clicked	(GtkWidget 	*widget, 
			gpointer 	data)
{
   gtk_widget_destroy(GTK_WIDGET(data));   
}


