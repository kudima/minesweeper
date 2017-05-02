#include <gtk/gtk.h>
#include "include/mines_field.h"
#include "include/display_field.h"

#define max(x, y) ( (x) > (y) ? (x) : (y) )
#define min(x, y) ( (x) < (y) ? (x) : (y) )

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

    gtk_widget_set_size_request(display_field->widget, 
	    display_field->cell_size*mf->width,
	    display_field->cell_size*mf->height); 

    gtk_widget_add_events(display_field->widget, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events(display_field->widget, GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events(display_field->widget, GDK_BUTTON_MOTION_MASK);

    g_signal_connect(G_OBJECT(display_field->widget), "expose_event", 
	    G_CALLBACK(display_field_expose_event), 
	    (gpointer)display_field);
    g_signal_connect(G_OBJECT(display_field->widget), "button_press_event", 
	    G_CALLBACK(display_field_button_event), 
	    (gpointer)display_field);
    g_signal_connect(G_OBJECT(display_field->widget), "button_release_event", 
	    G_CALLBACK(display_field_button_event), 
	    (gpointer)display_field);
    
/*  may be I should use it in future in order to change rendering behavor 
    g_signal_connect(G_OBJECT(display_field->widget), "motion_notify_event", 
	    G_CALLBACK(display_field_motion_event), 
	    (gpointer)display_field); */

    return (display_field);
}

/* events functions */
gboolean display_field_expose_event(GtkWidget *widget,
       	GdkEventExpose *event,
       	gpointer df)
{
    display_field_show(widget->window, df, TRUE);
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

    df = (DisplayField*)data;
    mf = df->field;
    x = (gint)event->x/df->cell_size;
    y = (gint)event->y/df->cell_size;

    if (x == mf->width)
        x -= 1;

    if (y == mf->height)
        y -= 1;

    if (x > mf->width || y > mf->height) {
        fprintf(stderr, "cell coordinates are out of the box (%d, %d)\n", x, y);
        return FALSE;
    }

    if (mf->is_fail || mf->fully_opened)
	    return FALSE;

    if (event->type == GDK_BUTTON_RELEASE) {
	if (event->button==1 && !(event->state&GDK_BUTTON3_MASK) ) 
	    action = ACTION_OPEN;
	if ( ((event->button==1) && (event->state&GDK_BUTTON3_MASK))
	    || ((event->button==3) && (event->state&GDK_BUTTON1_MASK)) ) 
	    if (mf->opened_count!=0)
		action = ACTION_OPEN_AROUND;
	if (event->button==2 && mf->opened_count!=0) 
    		action = ACTION_OPEN_AROUND;
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
	else if ( (event->button == 3) 
		   && (event->state&GDK_BUTTON1_MASK) )
		pressed_state = PRESSED_STATE_AROUND;
	else if ( (event->button == 1) 
		   && (event->state&GDK_BUTTON3_MASK) )
		pressed_state = PRESSED_STATE_AROUND;
	else if (event->button == 1) 
		pressed_state = PRESSED_STATE_ALONE;
    }

    switch (action) {
	case ACTION_OPEN:
	    mf_set_state(mf, x, y, OPENED);
	    break;
	case ACTION_OPEN_AROUND:
	    mf_set_state_around(mf, x, y, mf->cell[x][y] & OPENED);
	    break;
	case ACTION_MARK:
	    mf_set_state(mf, x, y, MARKED);
	    break;
    }

    switch (pressed_state) {
	case PRESSED_STATE_ALONE:
	    display_field_show_pressed(widget->window, df, 
		     gdk_gc_new(widget->window), x, y); 
	    break;
	case PRESSED_STATE_AROUND:
	    display_field_show_pressed_around(widget->window, df, 
		     gdk_gc_new(widget->window), x, y); 
    }

    if (pressed_state == PRESSED_NONE)
	display_field_show(widget->window, df, FALSE);

    if (mf->is_fail || mf->fully_opened)
	display_field_show(widget->window, df, TRUE);

    display_field_admit_state(df);
    return FALSE;
}

gboolean display_field_motion_event(GtkWidget *widget, 
	GdkEventButton *event,
	gpointer data)
{
 /*   gint x, y;
    DisplayField *df;
    MinesField *mf;
    int pressed_state;

    df = data;
    mf = df->field;
    x = (gint)event->x/df->cell_size;
    y = (gint)event->y/df->cell_size;

    if ( (x != prev_x) && (y != prev_y) ) {
	if (event->type == GDK_BUTTON_PRESS) {
	    if (event->button == 1) 
		pressed_state = PRESSED_STATE_ALONE;
	    if (event->button == 2)
		pressed_state = PRESSED_STATE_AROUND;
	    if ( (event->button == 3) 
		    && (event->state&GDK_BUTTON1_MASK) )
		pressed_state == PRESSED_STATE_AROUND;
	    if ( (event->button == 1) 
		    && (event->state&GDK_BUTTON3_MASK) )
		pressed_state == PRESSED_STATE_AROUND;
	}
	if (prev_state == PRESSED_STATE_ALONE) 
	    mf_set_state(mf, prev_x, prev_y, PRESSED);
	if (prev_state == PRESSED_STATE_AROUND)
	    mf_set_state_around(mf, prev_x, prev_y, PRESSED);
	switch (pressed_state) {
	    case PRESSED_STATE_ALONE:
		mf_set_state(mf, x, y, PRESSED);
		break;
	    case PRESSED_STATE_AROUND:
		mf_set_state_around(mf, x, y, PRESSED);
		break;
	}
	prev_x = x;
	prev_y = y;
	prev_state = pressed_state;
	display_field_refresh(widget, df);
    }*/
    return FALSE;
}

/* display functions */
static gboolean display_field_show(GdkDrawable *canvas,
       	DisplayField *df, gboolean full_update)
{
    GdkGC *gc;
    int i, j;
    MinesField *mf;
    GdkColor color_white;
    GdkColor color_black;

    mf = df->field;

    color_white.red = 65535;
    color_white.blue = 65535;
    color_white.green = 65535;
 
    color_black.red = 42000;
    color_black.blue = 41000;
    color_black.green = 41000;

    gc = gdk_gc_new(canvas);
    gdk_gc_set_rgb_fg_color(gc, &color_white);

    for (i=0; i<mf->width; i++) {
		for (j=0; j<mf->height; j++) {
			if (mf->cell[i][j]&NEED_UPDATE || full_update) {
			    mf->cell[i][j] &= ~NEED_UPDATE;
			    
			    gdk_gc_set_rgb_fg_color(gc, &color_white);
			    gdk_draw_rectangle(canvas, gc, TRUE, 
				    i*df->cell_size, j*df->cell_size,
				    df->cell_size, df->cell_size);
		
			    gdk_gc_set_rgb_fg_color(gc, &color_black);
			    gdk_draw_rectangle(canvas, gc, FALSE, 
				    i*df->cell_size, j*df->cell_size,
				    df->cell_size, df->cell_size);
		
			    if (!(mf->is_fail)) {
			    	
					if (mf->cell[i][j]&OPENED) {
					    if (mf_get_number(mf, i, j) != 0) {
							display_field_show_number(canvas, df, gc, i, j,
								mf_get_number(mf, i, j));
					    }
					    continue;
					} 
		
		    		if (mf->cell[i][j]&MARKED) {
		    		    display_field_show_marked(canvas, df, gc, i, j); 
				    	continue;
					}
		
					display_field_show_closed(canvas, df, gc, i, j); 
			    }
		
			    if (mf->is_fail) {
			    	
					if ( (mf->cell[i][j]&OPENED) && 
						(mf->cell[i][j]&BOMBED) ) {
					    display_field_show_boom(canvas, df, gc, i, j);
					    continue;
					}
		
					if ( (mf->cell[i][j]&OPENED) &&
						   !(mf->cell[i][j]&BOMBED) ) {
			
					    if (mf_get_number(mf, i, j) != 0) 
							display_field_show_number(canvas, df, gc, i, j,
								mf_get_number(mf, i, j));
								
					    continue;
					}
		
					if ( !(mf->cell[i][j]&OPENED) &&
						   (mf->cell[i][j]&BOMBED) ) {
					    display_field_show_mine(canvas, df, gc, i, j);
					    continue;
					}
		
		    		if ( (mf->cell[i][j]&MARKED) &&
					(mf->cell[i][j]&BOMBED) ) {
		    		    display_field_show_marked(canvas, df, gc, i, j); 
					    continue;
					}
		
		    		if ( (mf->cell[i][j]&MARKED) &&
						!(mf->cell[i][j]&BOMBED) ) {
			    	    gdk_gc_set_rgb_fg_color(gc, &color_black);
		    		    gdk_draw_line(canvas, gc, i*df->cell_size, j*df->cell_size,
					    	(i+1)*df->cell_size, (j+1)*df->cell_size);
				    	gdk_draw_line(canvas, gc, (i+1)*df->cell_size, j*df->cell_size,
					    	i*df->cell_size, (j+1)*df->cell_size);
				    	continue;
					}
		
					if ( !(mf->cell[i][j]&OPENED) &&
						!(mf->cell[i][j]&BOMBED) &&
					    !(mf->cell[i][j]&MARKED)) {
				    	display_field_show_closed(canvas, df, gc, i, j);
				    	continue;
					}
			    }
			}
		}
    }

    g_object_unref(G_OBJECT(gc));

    return FALSE;
}

void display_field_show_number(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
       	gint i, gint j, gint number)
{
    gdk_draw_drawable(draw, gc, pixmap_numbers[number-1], 0, 0, 
	    (gint)i*df->cell_size+1, (gint)j*df->cell_size+1,
	    df->cell_size-1, df->cell_size-1);
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
       	gint x, gint y)
{
    MinesField *mf;

    mf = df->field;
    
    if ((x<0 || x>=mf->width)
	    || (y<0 || y>=mf->height))
		return;

    if ( !(mf->cell[x][y]&OPENED) 
	   && !(mf->cell[x][y]&MARKED) ) {
		mf->cell[x][y] |= NEED_UPDATE;
		gdk_draw_drawable(draw, gc, pixmap_pressed, 0, 0, 
			(gint)x*df->cell_size+1, (gint)y*df->cell_size+1,
			df->cell_size-1, df->cell_size-1);
    }
}

void display_field_show_pressed_around(GdkDrawable *draw,
       	DisplayField *df, GdkGC *gc,
       	gint x, gint y)
{
    MinesField *mf;
    int i, j;

    mf = df->field;
    mf = df->field;
        if ((x<0 || x>=mf->width)
	    || (y<0 || y>=mf->height))
	    return;
    for (i=max(0, x-1); i<min(mf->width, x+2); i++)
		for (j=max(0, y-1); j<min(mf->height, y+2); j++) {
	    	if ( !(mf->cell[i][j]&OPENED) &&
				!(mf->cell[i][j]&MARKED) ) {
				mf->cell[i][j] |= NEED_UPDATE;
				gdk_draw_drawable(draw, gc, pixmap_pressed, 0, 0, 
					(gint)i*df->cell_size+1, (gint)j*df->cell_size+1,
					df->cell_size-1, df->cell_size-1);
	   	}
	}
}

void display_field_show_xpm(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j, const gchar *xpm_file)
{
    GdkPixmap *pixmap;

    pixmap = gdk_pixmap_create_from_xpm(draw, NULL, NULL, xpm_file);
    gdk_draw_drawable(draw, gc, pixmap, 0, 0, 
	    (gint)i*df->cell_size+1, (gint)j*df->cell_size+1,
	    df->cell_size-1, df->cell_size-1);
    g_object_unref(G_OBJECT(pixmap));
}

void display_field_show_pixmap(GdkDrawable *draw, DisplayField *df, GdkGC *gc,
	gint i, gint j, GdkPixmap *pixmap)
{
    gdk_draw_drawable(draw, gc, pixmap, 0, 0, 
	    (gint)i*df->cell_size+1, (gint)j*df->cell_size+1,
	    df->cell_size-1, df->cell_size-1);
}

