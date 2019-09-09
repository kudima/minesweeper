/*
 * MIT License
 *
 * Copyright (c) 2018 kudima
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DISPLAY_FIELD
#define DISPLAY_FIELD
#include <gtk/gtk.h>
#include "mines_field.h"

extern GdkPixmap *pixmap_opened;
extern GdkPixmap *pixmap_marked;
extern GdkPixmap *pixmap_closed;
extern GdkPixmap *pixmap_boom;
extern GdkPixmap *pixmap_mine;
extern GdkPixmap *pixmap_pressed;
extern GdkPixmap *pixmap_numbers[8];

enum actions
{
	ACTION_OPEN,
	ACTION_MARK,
	ACTION_OPEN_AROUND,
};

enum pressed_states
{
	PRESSED_STATE_ALONE,
	PRESSED_STATE_AROUND,
	PRESSED_NONE
};

typedef void (*StateHandler)(GObject*, MinesField*);

/* data */
typedef struct
{
	GtkWidget *widget; /* field display widget */
	GObject *object; /* an object which handle field state changes */
	StateHandler state_handler;
	gint cell_size;
	MinesField *field;
	gint x_prev;
	gint y_prev;
	gboolean full_update;
} DisplayField;


#ifndef ABS
#define ABS(x) (x >= 0 ? x : -x)
#endif

#define DF_AREA_WIDTH(area) ABS((area)->x_bottom - (area)->x_top)
#define DF_AREA_HEIGHT(area) ABS((area)->y_bottom - (area)->y_top)

/* determines the area we want to redraw when an action happend
 * on a minefield */
typedef struct {
		gint x_top;
		gint y_top;
		gint x_bottom;
		gint y_bottom;
		gint cell_size;
} DFDrawArea;

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
								   DisplayField *df);

void display_field_show_pressed_around(GdkDrawable *draw,
									   DisplayField *df, GdkGC *gc,
									   gint x, gint y, DFDrawArea *area);

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
								gint i, gint j, DFDrawArea *area);

void display_field_show_xpm(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							gint i, gint j, const gchar *xpm_file);

void display_field_show_pixmap(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							   gint i, gint j, GdkPixmap *pixmap);
#endif
