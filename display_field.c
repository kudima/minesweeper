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

#include <gtk/gtk.h>
#include "include/mines_field.h"
#include "include/display_field.h"

#define max(x, y) ( (x) > (y) ? (x) : (y) )
#define min(x, y) ( (x) < (y) ? (x) : (y) )

GdkPixmap *pixmap_opened;
GdkPixmap *pixmap_marked;
GdkPixmap *pixmap_closed;
GdkPixmap *pixmap_boom;
GdkPixmap *pixmap_mine;
GdkPixmap *pixmap_pressed;
GdkPixmap *pixmap_numbers[8];

static void display_field_init_draw_area(DisplayField *df, DFDrawArea *area);
static void display_field_update_draw_area(DFDrawArea *area, gint x, gint y);
static void display_field_request_update(DisplayField *df, gint, gint, DFDrawArea *area);
static void display_field_unpress( DisplayField *df, gint x, gint y);
static void display_field_unpress_around(DisplayField *df, gint x, gint y,
									   DFDrawArea *area);


void display_field_admit_state(DisplayField *df)
{
	df->state_handler(df->object, df->field);
}

DisplayField* display_field_new(MinesField *mf,
								GObject *object,
								StateHandler state_handler,
								gint cell_size)
{
	DisplayField *display_field;

	display_field = (DisplayField*)g_malloc(sizeof(DisplayField));
	display_field->widget = gtk_drawing_area_new();
	display_field->cell_size = cell_size;
	display_field->field = mf;
	display_field->object = G_OBJECT(object);
	display_field->state_handler = state_handler;
	display_field->full_update = TRUE;

	gtk_widget_set_size_request(display_field->widget,
								display_field->cell_size * mf->width,
								display_field->cell_size * mf->height);

	gtk_widget_add_events(display_field->widget, GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events(display_field->widget, GDK_BUTTON_RELEASE_MASK);
	gtk_widget_add_events(display_field->widget, GDK_BUTTON_MOTION_MASK);

	g_signal_connect(G_OBJECT(display_field->widget), "expose-event",
					 G_CALLBACK(display_field_expose_event),
					 (gpointer)display_field);
	g_signal_connect(G_OBJECT(display_field->widget), "button_press_event",
					 G_CALLBACK(display_field_button_event),
					 (gpointer)display_field);
	g_signal_connect(G_OBJECT(display_field->widget), "button_release_event",
					 G_CALLBACK(display_field_button_event),
					 (gpointer)display_field);

	/*	may be I should use it in future in order to change rendering behavor */
		g_signal_connect(G_OBJECT(display_field->widget), "motion_notify_event",
			G_CALLBACK(display_field_motion_event),
			(gpointer)display_field); 

	return (display_field);
}

void display_field_queue_draw(DisplayField *df, gboolean full_update)
{
		df->full_update = full_update;
		gtk_widget_queue_draw(df->widget);
}

void display_field_mark_cells_in_area(DisplayField *df, DFDrawArea *area)
{
	gint i_top = area->x_top/df->cell_size;
	gint j_top = area->y_top/df->cell_size;
	gint i_bottom = area->x_bottom/df->cell_size;
	gint j_bottom = area->y_bottom/df->cell_size;

	for (gint i=i_top; i<i_bottom; i++) {
		for (gint j=j_top; j<j_bottom; j++) {
			df->field->cell[i][j] |= NEED_UPDATE;
		}
	}
}

void display_field_queue_draw_area(DisplayField *df, DFDrawArea *area)
{
		df->full_update = FALSE;
		display_field_mark_cells_in_area(df, area);
		gtk_widget_queue_draw_area(df->widget, area->x_top, area->y_top,
						DF_AREA_WIDTH(area), DF_AREA_HEIGHT(area));
}

/* events functions */
gboolean display_field_expose_event(GtkWidget *widget,
									GdkEventExpose *event,
									gpointer df)
{
	((DisplayField *)df)->full_update = TRUE;
	display_field_show(widget->window, df);
	return FALSE;
}

gboolean display_field_button_event(GtkWidget *widget,
									GdkEventButton *event,
									gpointer data)
{
	DisplayField *df;
	MinesField *mf;
	gint action = -1;
	gint pressed_state = PRESSED_NONE;
	gint x, y;
	DFDrawArea area;

	df = (DisplayField*)data;
	mf = df->field;
	x = (gint)event->x / df->cell_size;
	y = (gint)event->y / df->cell_size;
	display_field_init_draw_area(df, &area);

	if (x == mf->width)
		x -= 1;

	if (y == mf->height)
		y -= 1;

	if (x > mf->width || y > mf->height) {
		return FALSE;
	}

	if (mf->is_fail || mf->fully_opened)
		return FALSE;

	df->x_prev = x;
	df->y_prev = y;

	if (event->type == GDK_BUTTON_RELEASE) {

			if (event->button == 1 && !(event->state & GDK_BUTTON3_MASK))
				action = ACTION_OPEN;

			if (((event->button == 1) && (event->state & GDK_BUTTON3_MASK))
					|| ((event->button == 3) && (event->state & GDK_BUTTON1_MASK))) {
				if (mf->opened_count != 0)
					action = ACTION_OPEN_AROUND;
			}

			if (event->button == 2) {
				display_field_unpress_around(df, df->x_prev, df->y_prev, &area);
				if (mf->opened_count != 0)
					action = ACTION_OPEN_AROUND;
			}
	}

	if (event->type == GDK_BUTTON_PRESS) {
		if (event->button == 3)
			action = ACTION_MARK;
	}

	if (event->type == GDK_BUTTON_PRESS
			|| event->type == GDK_2BUTTON_PRESS
			|| event->type == GDK_3BUTTON_PRESS) {

		if (event->button == 2)
			pressed_state = PRESSED_STATE_AROUND;
		else if ((event->button == 3)
				 && (event->state & GDK_BUTTON1_MASK))
			pressed_state = PRESSED_STATE_AROUND;
		else if ((event->button == 1)
				 && (event->state & GDK_BUTTON3_MASK))
			pressed_state = PRESSED_STATE_AROUND;
		else if (event->button == 1)
			pressed_state = PRESSED_STATE_ALONE;

	}

	if (action >= 0) {
		switch (action) {
		case ACTION_OPEN:
			mf_set_state(mf, x, y, OPENED, &area,
						   (mf_callback_t)display_field_update_draw_area);
			break;
		case ACTION_OPEN_AROUND:
			display_field_unpress_around(df, df->x_prev, df->y_prev, &area);
			mf_set_state_around(mf, x, y, mf->cell[x][y] & OPENED, &area,
							(mf_callback_t)display_field_update_draw_area);
			break;
		case ACTION_MARK:
			mf_set_state(mf, x, y, MARKED, &area,
						   (mf_callback_t)display_field_update_draw_area);
			break;
		}
		//display_field_request_update(df, x, y, &area);
	}

	switch (pressed_state) {
	case PRESSED_STATE_ALONE:
		display_field_show_pressed(widget->window, df,
								   gdk_gc_new(widget->window), x, y, &area);
		break;
	case PRESSED_STATE_AROUND:
		if (mf->opened_count == 0)
			break;
		display_field_unpress_around(df, df->x_prev, df->y_prev, &area);
		display_field_show_pressed_around(widget->window, df,
										  gdk_gc_new(widget->window), x, y, &area);
		break;
	}

	display_field_admit_state(df);

	if (mf->is_fail || mf->fully_opened) {
		display_field_queue_draw(df, TRUE);
	} else {
		display_field_queue_draw_area(df, &area);
	}

	return FALSE;
}

gboolean display_field_motion_event(GtkWidget *widget,
									GdkEventButton *event,
									gpointer data)
{
	gint x, y;
	DisplayField *df;
	MinesField *mf;
	DFDrawArea area;
	int pressed_state = PRESSED_NONE;

	df = data;
	mf = df->field;
	x = (gint)event->x/df->cell_size;
	y = (gint)event->y/df->cell_size;
	display_field_init_draw_area(df, &area);

	if (x == mf->width)
		x -= 1;

	if (y == mf->height)
		y -= 1;

	if (!mf_includes(mf, x, y)) {
		display_field_unpress_around(df, df->x_prev, df->y_prev, &area);
		display_field_queue_draw_area(df, &area);
		return FALSE;
	}

	if ( (x == df->x_prev) && (y == df->y_prev) )
		return FALSE;

	if (event->type == GDK_MOTION_NOTIFY) {

		if (event->state & GDK_BUTTON1_MASK) {
			pressed_state = PRESSED_STATE_ALONE;
		}

		if ((event->state & GDK_BUTTON1_MASK) && (event->state & GDK_BUTTON3_MASK)) {
			pressed_state = PRESSED_STATE_AROUND;
		}

		if (event->state & GDK_BUTTON2_MASK) {
			pressed_state = PRESSED_STATE_AROUND;
		}
	}

	/* draw the hints */
	switch (pressed_state) {
		case PRESSED_STATE_ALONE:
			display_field_show_pressed(widget->window, df,
									   gdk_gc_new(widget->window), x, y, &area);
			display_field_unpress(df, df->x_prev, df->y_prev);
			display_field_request_update(df, df->x_prev, df->y_prev, &area);
		break;
		case PRESSED_STATE_AROUND:
			if (mf->opened_count == 0)
				break;

			display_field_unpress_around(df, df->x_prev, df->y_prev, &area);
			display_field_show_pressed_around(widget->window, df,
											  gdk_gc_new(widget->window), x, y, &area);
		break;
	}

	df->x_prev = x;
	df->y_prev = y;

	if (mf->is_fail || mf->fully_opened)
		display_field_queue_draw(df, TRUE);
	else {
		display_field_queue_draw_area(df, &area);
	}

	display_field_admit_state(df);

	return FALSE;
}

/* display functions */
static gboolean display_field_show(GdkDrawable *canvas,
								   DisplayField *df)
{
	GdkGC *gc;
	int i, j;
	MinesField *mf;
	GdkColor color_black;

	mf = df->field;

	color_black.red = 42000;
	color_black.blue = 41000;
	color_black.green = 41000;

	gc = gdk_gc_new(canvas);

	for (i = 0; i < mf->width; i++) {
		for (j = 0; j < mf->height; j++) {
			if (mf->cell[i][j]&NEED_UPDATE || df->full_update) {
				mf->cell[i][j] &= ~NEED_UPDATE;

				gdk_gc_set_rgb_fg_color(gc, &color_black);
				gdk_draw_rectangle(canvas, gc, FALSE,
								   i * df->cell_size, j * df->cell_size,
								   df->cell_size, df->cell_size);

				if (!(mf->is_fail)) {

					if (mf->cell[i][j]&OPENED) {
						if (mf_get_number(mf, i, j) != 0) {
							display_field_show_number(canvas, df, gc, i, j,
													  mf_get_number(mf, i, j));
						} else
							display_field_show_opened(canvas, df, gc, i, j);
						continue;
					}

					if (mf->cell[i][j]&MARKED) {
						display_field_show_marked(canvas, df, gc, i, j);
						continue;
					}


					if (mf->cell[i][j]&PRESSED) {
						gdk_draw_drawable(canvas, gc, pixmap_pressed, 0, 0,
										  (gint)i * df->cell_size + 1, (gint)j * df->cell_size + 1,
										  df->cell_size - 1, df->cell_size - 1);
						continue;
					}

					display_field_show_closed(canvas, df, gc, i, j);
				}

				if (mf->is_fail) {

					if ((mf->cell[i][j]&OPENED) &&
							(mf->cell[i][j]&BOMBED)) {
						display_field_show_boom(canvas, df, gc, i, j);
						continue;
					}

					if ((mf->cell[i][j]&OPENED) &&
							!(mf->cell[i][j]&BOMBED)) {

						if (mf_get_number(mf, i, j) != 0)
							display_field_show_number(canvas, df, gc, i, j,
													  mf_get_number(mf, i, j));
						else
							display_field_show_opened(canvas, df, gc, i, j);

						continue;
					}

					if (!(mf->cell[i][j]&OPENED) &&
							(mf->cell[i][j]&BOMBED)) {
						display_field_show_mine(canvas, df, gc, i, j);
						continue;
					}

					if ((mf->cell[i][j]&MARKED) &&
							(mf->cell[i][j]&BOMBED)) {
						display_field_show_marked(canvas, df, gc, i, j);
						continue;
					}

					if ((mf->cell[i][j]&MARKED) &&
							!(mf->cell[i][j]&BOMBED)) {
						gdk_gc_set_rgb_fg_color(gc, &color_black);
						gdk_draw_line(canvas, gc, i * df->cell_size, j * df->cell_size,
									  (i + 1)*df->cell_size, (j + 1)*df->cell_size);
						gdk_draw_line(canvas, gc, (i + 1)*df->cell_size, j * df->cell_size,
									  i * df->cell_size, (j + 1)*df->cell_size);
						continue;
					}

					if (!(mf->cell[i][j]&OPENED) &&
							!(mf->cell[i][j]&BOMBED) &&
							!(mf->cell[i][j]&MARKED)) {
						display_field_show_closed(canvas, df, gc, i, j);
						continue;
					}

				}
			}
		}
	}

	df->full_update = FALSE;
	g_object_unref(G_OBJECT(gc));

	return FALSE;
}

void display_field_show_number(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							   gint i, gint j, gint number)
{
	gdk_draw_drawable(draw, gc, pixmap_numbers[number - 1], 0, 0,
					  (gint)i * df->cell_size + 1, (gint)j * df->cell_size + 1,
					  df->cell_size - 1, df->cell_size - 1);
}

void display_field_show_closed(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							   gint i, gint j)
{
	display_field_show_pixmap(draw, df, gc, i, j, pixmap_closed);
}

void display_field_show_opened(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							   gint i, gint j)
{
	display_field_show_pixmap(draw, df, gc, i, j, pixmap_opened);
}

void display_field_show_marked(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							   gint i, gint j)
{
	display_field_show_pixmap(draw, df, gc, i, j, pixmap_marked);
}

void display_field_show_boom(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							 gint i, gint j)
{
	display_field_show_pixmap(draw, df, gc, i, j, pixmap_boom);
}

void display_field_show_mine(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							 gint i, gint j)
{
	display_field_show_pixmap(draw, df, gc, i, j, pixmap_mine);
}

void display_field_show_pressed(GdkDrawable *draw,
								DisplayField *df, GdkGC *gc,
								gint x, gint y, DFDrawArea *area)
{
	MinesField *mf;

	mf = df->field;

	if (!mf_includes(mf, x, y))
			return;

	if (!(mf->cell[x][y]&OPENED)
			&& !(mf->cell[x][y]&MARKED)) {
		mf->cell[x][y] |= (PRESSED | NEED_UPDATE);
		display_field_update_draw_area(area, x, y);
	}
}

void display_field_request_update(DisplayField *df, gint x, gint y, DFDrawArea *area)
{
	df->field->cell[x][y] |= NEED_UPDATE;
	display_field_update_draw_area(area, x, y);
}

/* initialize the area so we always add the very first cell being updated */
void display_field_init_draw_area(DisplayField *df, DFDrawArea *area) 
{
	area->x_top = df->cell_size * df->field->height;
	area->y_top = df->cell_size * df->field->height;
	area->x_bottom = 0;
	area->y_bottom = 0;
	area->cell_size = df->cell_size;
}

/*
 * Adds a minefield cell into the area we need to redraw
 * after an action on minefield.
 * x, y are the coordinates of a cell on the minefield.
 */
void display_field_update_draw_area(DFDrawArea *area, gint x, gint y) 
{
		gint draw_x = x*area->cell_size;
		gint draw_y = y*area->cell_size;

		if (draw_x <= area->x_top) {
				area->x_top = draw_x;
		}

		if (draw_x >= area->x_bottom) {
				area->x_bottom = draw_x + area->cell_size;
		}

		if (draw_y <= area->y_top) {
				area->y_top = draw_y;
		}

		if (draw_y >= area->y_bottom) {
				area->y_bottom = draw_y + area->cell_size;
		}
}

void display_field_unpress( DisplayField *df, gint x, gint y)
{
	df->field->cell[x][y] &= ~PRESSED;
}

void display_field_unpress_around(DisplayField *df, gint x, gint y,
									   DFDrawArea *area)
{
	MinesField *mf;
	int i, j;

	mf = df->field;
	mf = df->field;
	if ((x < 0 || x >= mf->width)
			|| (y < 0 || y >= mf->height))
		return;

	for (i = max(0, x - 1); i < min(mf->width, x + 2); i++)
		for (j = max(0, y - 1); j < min(mf->height, y + 2); j++) {
			if (!(mf->cell[i][j]&OPENED) &&
					!(mf->cell[i][j]&MARKED)) {
				mf->cell[i][j] |= NEED_UPDATE;
				mf->cell[i][j] &= ~PRESSED;
				display_field_update_draw_area(area, i, j);
			}
		}
}

void display_field_show_pressed_around(GdkDrawable *draw,
									   DisplayField *df, GdkGC *gc,
									   gint x, gint y, DFDrawArea *area)
{
	MinesField *mf;
	int i, j;

	mf = df->field;
	mf = df->field;
	if ((x < 0 || x >= mf->width)
			|| (y < 0 || y >= mf->height))
		return;

	for (i = max(0, x - 1); i < min(mf->width, x + 2); i++)
		for (j = max(0, y - 1); j < min(mf->height, y + 2); j++) {
			if (!(mf->cell[i][j]&OPENED) &&
					!(mf->cell[i][j]&MARKED)) {
				mf->cell[i][j] |= NEED_UPDATE;
				mf->cell[i][j] |= PRESSED;
				display_field_update_draw_area(area, i, j);
			}
		}
}

void display_field_show_xpm(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							gint i, gint j, const gchar *xpm_file)
{
	GdkPixmap *pixmap;

	pixmap = gdk_pixmap_create_from_xpm(draw, NULL, NULL, xpm_file);
	gdk_draw_drawable(draw, gc, pixmap, 0, 0,
					  (gint)i * df->cell_size + 1, (gint)j * df->cell_size + 1,
					  df->cell_size - 1, df->cell_size - 1);
	g_object_unref(G_OBJECT(pixmap));
}

void display_field_show_pixmap(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
							   gint i, gint j, GdkPixmap *pixmap)
{
	gdk_draw_drawable(draw, gc, pixmap, 0, 0,
					  (gint)i * df->cell_size + 1, (gint)j * df->cell_size + 1,
					  df->cell_size - 1, df->cell_size - 1);
}

