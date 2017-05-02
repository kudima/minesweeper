#ifndef DISPLAY_FIELD
#define DISPLAY_FIELD
#include <gtk/gtk.h>
#include "mines_field.h"

GdkPixmap *pixmap_opened;
GdkPixmap *pixmap_marked;
GdkPixmap *pixmap_closed;
GdkPixmap *pixmap_boom;
GdkPixmap *pixmap_mine;
GdkPixmap *pixmap_pressed;
GdkPixmap *pixmap_numbers[8];

enum actions 
{
    ACTION_OPEN,
    ACTION_MARK, 
    ACTION_OPEN_AROUND, 
};

enum pressed_states {
    PRESSED_STATE_ALONE, 
    PRESSED_STATE_AROUND,
    PRESSED_NONE
};

typedef void (*StateHandler) (GObject*, MinesField*);

/* data */
typedef struct 
{
    GtkWidget *widget; /* field display widget */
    GObject *object; /* an object which handle field state changes */
    StateHandler state_handler;
    gint cell_size;
    MinesField *field;
} DisplayField;

/* functions */
DisplayField* display_field_new(MinesField *mf, GObject *object,
       StateHandler state_handler, gint cell_size);

gboolean display_field_expose_event(GtkWidget *widget,
        GdkEventExpose *event,
	gpointer df);

gboolean display_field_button_event(GtkWidget *widget, 
	GdkEventButton *event,
	gpointer data);

gboolean display_field_motion_event(GtkWidget *widget, 
	GdkEventButton *event,
	gpointer data);

static gboolean display_field_show(GdkDrawable *canvas,
       	DisplayField *df, gboolean full_update);

void display_field_show_pressed(GdkDrawable *draw,
       	DisplayField *df, GdkGC *gc,
       	gint x, gint y);

void display_field_show_pressed_around(GdkDrawable *draw,
       	DisplayField *df, GdkGC *gc,
       	gint x, gint y);

void display_field_show_number(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
       	gint i, gint j, gint number);

void display_field_show_closed(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j);

void display_field_show_opened(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j);

void display_field_show_marked(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j);

void display_field_show_boom(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j);

void display_field_show_mine(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j);

void display_field_show_pressed(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j);

void display_field_show_xpm(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j, const gchar *xpm_file);

void display_field_show_pixmap(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j, GdkPixmap *pixmap);
#endif
