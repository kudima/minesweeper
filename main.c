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

#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <math.h>

#include "include/display_field.h"
#include "include/mines_field.h"
#include "include/main.h"
#include "dialog/scores/interface.h"
#include "include/field_snapshot.h"

/*
 *   display_field_button_event
 * 		|
 * 	 check button state
 * 	        |
 *       change miner field
 *         	|                   
 *    refresh field representation 
 */

/*
 Constansts which contains a pixmap files names
 */

const gchar OPENED_XPM[] = "pixmaps/opened.xpm";
const gchar MARKED_XPM[] = "pixmaps/marked.xpm";
const gchar CLOSED_XPM[] = "pixmaps/closed.xpm";
const gchar BOOM_XPM[] = "pixmaps/boom.xpm";
const gchar MINE_XPM[] = "pixmaps/mine.xpm";
const gchar PRESSED_XPM[] = "pixmaps/pressed.xpm";
/* Global variables */

/* The main window widget */
static MainWindow main_window;
/* The application current state */
static MinerConfig miner_config;

/* 
 * This array contains tokens which function read_miner_config
 * expects to meet while reading config file 
 */
static const char *token[] = {"bombs", "width", "height",
		       "small", "medium", "large"};

/* TOKEN_ constants contains token index in the token array */
#define TOKEN_BOMBS  0
#define TOKEN_WIDTH  1
#define TOKEN_HEIGHT  2
#define TOKEN_SMALL_TIME 3
#define TOKEN_MEDIUM_TIME 4
#define TOKEN_LARGE_TIME 5


#define LINE_SIZE  0x100
#define TOKEN_SIZE  0x100
#define TOKEN_NUMBER  6

/* 
 * Converts a double to a GTimeVal pointed by time argument
 */
static inline void double_to_tv(GTimeVal *time, double double_val)
{
    time->tv_sec = (gulong)trunc(double_val);
    time->tv_usec = (glong)trunc(1000000*(double_val - time->tv_sec));
}
/*
 * Converts GTimeVal to a double and put its string representation
 * into the memory pointed by doublestr 
 */
static inline void tv_to_doublestr(GTimeVal *time, gchar *doublestr, gint size)
{
    g_snprintf(doublestr, size, "%lu.%lu", time->tv_sec, time->tv_usec);
}
/*
 * The fields has a three predefined states. It is a small 8x8x10,
 * middle 16x16x40 and large 16x32x99. (WidthxHeightxBombs)
 * predefined_state member of the MainWindow structure describes
 * the current predefined state of the field.
 */
static inline void set_predefined_state(MainWindow *main_window, MinerConfig *config)
{
    main_window->predefined_state = PREDEFINED_NONE;
    if (config->bombs == 10 &&
	    config->width == 8 &&
	    config->height == 8)
	main_window->predefined_state = PREDEFINED_SMALL;
    if (config->bombs == 40 &&
	    config->width == 16 &&
	    config->height == 16)
	main_window->predefined_state = PREDEFINED_MEDIUM;
    if (config->bombs == 99 &&
	    config->width == 30 &&
	    config->height == 16)
	main_window->predefined_state = PREDEFINED_LARGE;
}

/* 
 * Sets predefined state of the field using main_window
 * argument. It means is sets a width, height and number
 * of bumbs which correspond to the predefined state.
 */
static inline gboolean set_predefined_time(MinerConfig *config, 
	MainWindow *main_window)
{
    GTimeVal *time;
    gboolean new_record;

    time = &main_window->time;
    new_record = FALSE;
    switch (main_window->predefined_state) {
	case PREDEFINED_SMALL:
	    if (config->small_time.tv_sec > time->tv_sec ||
		    ((config->small_time.tv_usec > time->tv_usec) 
		     && (config->small_time.tv_sec == time->tv_sec))) {
		config->small_time.tv_sec = time->tv_sec;  
		config->small_time.tv_usec = time->tv_usec;  
		new_record = TRUE;
	    }
	    break;
	case PREDEFINED_MEDIUM:
	    if (config->medium_time.tv_sec >= time->tv_sec ||
		    ((config->medium_time.tv_usec > time->tv_usec)
		     && (config->medium_time.tv_sec == time->tv_sec))) {
		config->medium_time.tv_sec = time->tv_sec;  
		config->medium_time.tv_usec = time->tv_usec;  
		new_record = TRUE;
	    }
	    break;
	case PREDEFINED_LARGE:
	    if (config->large_time.tv_sec >= time->tv_sec ||
		    ((config->large_time.tv_usec > time->tv_usec)
		     && (config->large_time.tv_sec == time->tv_sec))) {
		config->large_time.tv_sec = time->tv_sec;  
		config->large_time.tv_usec = time->tv_usec;  
		new_record = TRUE;
	    }
	    break;
    }
    return new_record;
}
/* 
 * Sets default config state if somthing goes wrong
 * while reading setting from config file
 */
void init_miner_config(MinerConfig *config)
{
    config->bombs = 10;
    config->width = 8;
    config->height = 8;
    config->small_time.tv_sec = 999;
    config->small_time.tv_usec = 0;
    config->medium_time.tv_sec = 999;
    config->medium_time.tv_usec = 0;
    config->large_time.tv_sec = 999;
    config->large_time.tv_usec = 0;
}

/* 
 * Reads initial configuration 
 * and feels a MinerConfig structure.
 */
gboolean read_miner_conifg(MinerConfig *config)
{
    FILE *fd_config;
    gchar current_line[LINE_SIZE];
    gchar current_token[TOKEN_SIZE];
    gchar current_token_value[TOKEN_SIZE];
    gint line_pos=0;
    gint pos=0;
    gint i=0;
    double double_val;;

    /* At first set miner config to default values if 
     * somthing well goes wrong with config file */
    init_miner_config(config);
    fd_config = fopen(".miner", "r");
    if (fd_config == NULL) {
        return FALSE;
    }
    while (!feof(fd_config)) {
        fgets(current_line, LINE_SIZE, fd_config);
        line_pos = 0;
        pos = 0;
        /* remove spaces before */
        while (current_line[line_pos]==0x20 && line_pos<LINE_SIZE) 
            line_pos++;
        while (current_line[line_pos]!=0x20 
                && line_pos<LINE_SIZE
                && pos<TOKEN_SIZE
                && current_line[line_pos]
                && current_line[line_pos]!='\n') {
            current_token[pos] = current_line[line_pos];
            pos++;
            line_pos++;
        }
        current_token[pos] = '\0';
        while (current_line[line_pos]==0x20 && line_pos<LINE_SIZE)
            line_pos++;
        pos = 0;
        while (current_line[line_pos]!=0x20 
                && line_pos<LINE_SIZE
                && pos<TOKEN_SIZE
                && current_line[line_pos]
                && current_line[line_pos]!='\n') {
            current_token_value[pos] = current_line[line_pos];
            line_pos++;
            pos++;
        }
        current_token_value[pos] = '\0';
        for (i=0; i<TOKEN_NUMBER; i++)
            if (!g_ascii_strcasecmp(token[i], current_token)) {
                switch(i) {
                    case TOKEN_BOMBS:
                        config->bombs = atoi(current_token_value);
                        break;
                    case TOKEN_HEIGHT:
                        config->height = atoi(current_token_value);
                        break;
                    case TOKEN_WIDTH:
                        config->width = atoi(current_token_value);
                        break;
                    case TOKEN_SMALL_TIME:
                        double_val = strtod(current_token_value, NULL);
                        double_to_tv(&config->small_time, double_val);
                        break;
                    case TOKEN_MEDIUM_TIME:
                        double_val = strtod(current_token_value, NULL);
                        double_to_tv(&config->medium_time, double_val);
                        break;
                    case TOKEN_LARGE_TIME:
                        double_val = strtod(current_token_value, NULL);
                        double_to_tv(&config->large_time, double_val);
                        break;
                }
            }
    }
    fclose(fd_config);
    return TRUE;
}

/* Saves current miner configuration */
gboolean write_miner_config(MinerConfig *config)
{
    gint i=0;
    gchar current_line[LINE_SIZE];
    gint current_pos;
    gint value;
    gchar str_value[0x10];
    FILE *fd_config;

    fd_config = fopen(".miner", "w");

    for (i=0; i<TOKEN_NUMBER; i++) {
        current_pos = g_strlcpy(current_line, token[i], LINE_SIZE);
        current_line[current_pos++] = 0x20;
        switch (i) {
            case TOKEN_BOMBS:
                value = config->bombs;
                g_sprintf(str_value, "%d", value); 
                break;
            case TOKEN_HEIGHT:
                value = config->height;
                g_sprintf(str_value, "%d", value); 
                break;
            case TOKEN_WIDTH:
                value = config->width;
                g_sprintf(str_value, "%d", value); 
                break;
            case TOKEN_SMALL_TIME:
                tv_to_doublestr(&config->small_time, str_value, 0x10); 	
                break;
            case TOKEN_MEDIUM_TIME:
                tv_to_doublestr(&config->medium_time, str_value, 0x10); 	
                break;
            case TOKEN_LARGE_TIME:
                tv_to_doublestr(&config->large_time, str_value, 0x10); 	
                break;
        }
        current_pos += g_strlcpy((gchar*)&current_line[current_pos],
                str_value, LINE_SIZE);
        current_line[current_pos++] = '\n';
        current_line[current_pos] = '\0';
        fputs(current_line, fd_config);
    }
    fclose(fd_config);
    return TRUE;
}


/* Some functions */
/*
 * Destroy event handeler
 */
static void main_destroy(GtkWidget *widget, 
        GdkEvent *event,
        gpointer data)
{
    write_miner_config(&miner_config);
    gtk_main_quit();
}

/* 
 * Just show current game duration time.
 * Called "almost" every 1/500 sec.
 */
GTimeVal start_time;
gboolean timer_function(gpointer *data)
{
    char timer_string[0x10];
    timer_string[0xF] = 0;
    gint second;
    gint dec_second;
    MinesField *mf;
    GTimeVal current_time;

    mf = (MinesField*)data;
    if (main_window.time.tv_sec > 999
            || mf->fully_opened
            || mf->is_fail
            || mf->opened_count == 0)
        return FALSE;

    if (main_window.time.tv_sec==0
            && main_window.time.tv_usec==0) 
        g_get_current_time(&start_time); 

    g_get_current_time(&current_time);
    main_window.time.tv_sec = current_time.tv_sec - start_time.tv_sec;
    main_window.time.tv_usec = current_time.tv_usec - start_time.tv_usec;

    if (main_window.time.tv_usec<0) {
        main_window.time.tv_sec--;
        main_window.time.tv_usec = 1000000 + main_window.time.tv_usec;
    }

    second = main_window.time.tv_sec;
    dec_second = main_window.time.tv_usec/10000;
    g_sprintf(timer_string, "%3.3d:%2.2d", second, dec_second);
    gtk_label_set_text(GTK_LABEL(main_window.timer_widget),
            timer_string);

    return TRUE;
}

/*
 * This functions called every time when somthing
 * changes on the field. It refreshs information
 * about current field state. Else it called
 * every time when a button pressed. Structure
 * DisplayField contains a pointer to this function
 * and so we can call it everywhere where we have
 * access to some DisplayField structure in order to
 * inform player about a changes.
 */
void field_change_status_handler(GObject *widget,
        MinesField *mf)
{
    char string[0x10];

    if (mf->is_fail) 
        gtk_label_set_text(GTK_LABEL(widget), "FAIL");

    if (mf->fully_opened) {
        gtk_label_set_text(GTK_LABEL(widget), "OPENED");
        if (set_predefined_time(&miner_config, &main_window))
            gtk_label_set_text(GTK_LABEL(widget), "RECORD!");
    }

    if (mf->opened_count == 0) 
        gtk_label_set_text(GTK_LABEL(widget), "READY");

    if (mf->opened_count>0 && !mf->is_fail && !mf->fully_opened
            && main_window.time.tv_sec == 0 
            && main_window.time.tv_usec == 0) {
        sprintf(string, "3BV: %3.3d", mf_get_3BV(mf));
        gtk_label_set_text(GTK_LABEL(main_window._3BV_widget), string);
        g_timeout_add(5, (GSourceFunc)timer_function, (gpointer)mf);
        g_timeout_add(40, (GSourceFunc)video_recorder_function, (gpointer)mf);
    }

    if (mf->opened_count>0 && !mf->is_fail
            && !mf->fully_opened) {
        sprintf(string, "M: %2.2d", mf->bombs-mf->marked_count);
        gtk_label_set_text(GTK_LABEL(main_window.mines_count_widget), string);
    }
}

/* New button handler */
gboolean button_new_field_pressed(GtkWidget *widget, 
        gpointer data)
{
    DisplayField *df;
    MinesField *mf;

    df = (DisplayField*)data;
    main_window.time.tv_sec = 0;
    main_window.time.tv_usec = 0;
    gtk_label_set_text(GTK_LABEL(main_window.timer_widget), "000:00");
    gtk_label_set_text(GTK_LABEL(main_window._3BV_widget), "3BV: ---");
    gtk_label_set_text(GTK_LABEL(main_window.mines_count_widget), "M: ---");
    mf_new(df->field, 
            df->field->width,
            df->field->height,
            df->field->bombs);
    gdk_window_invalidate_rect(df->widget->window, NULL, FALSE);
    field_change_status_handler(G_OBJECT(df->object), df->field);
}

/* Menu handler */
void menu_handler(GtkMenuItem *menuitem, 
        gpointer data)
{
    DisplayField *df;
    GtkWidget *dialog;
    gboolean is_field_state_changed = FALSE;

    df = main_window.df;
    switch ((enum menu_items)data) {
        case MENU_GAME_QUIT:
            write_miner_config(&miner_config);
            gtk_main_quit();
            break;
        case MENU_GAME_SCORES:
            dialog = create_window_score(&miner_config);
            gtk_widget_show(dialog);
            break;
        case MENU_GAME_LARGE:
            mf_new(df->field, 30, 16, 99);
            miner_config.width = 30;
            miner_config.height = 16;
            miner_config.bombs = 99;
            is_field_state_changed = TRUE; 
            break;
        case MENU_GAME_MEDIUM:
            mf_new(df->field, 16, 16, 40);
            miner_config.width = 16;
            miner_config.height = 16;
            miner_config.bombs = 40;
            is_field_state_changed = TRUE; 
            break;
        case MENU_GAME_SMALL:
            mf_new(df->field, 8, 8, 10);
            miner_config.width = 8;
            miner_config.height = 8;
            miner_config.bombs = 10;
            is_field_state_changed = TRUE; 
            break;
        case MENU_GAME_VIDEO:
            g_print("Choose video\n");
            GtkWidget *choose_file_dialog;

            choose_file_dialog = gtk_file_chooser_dialog_new("Open File",
                    (GtkWindow *)main_window.widget,
                    GTK_FILE_CHOOSER_ACTION_OPEN,
                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                    NULL);
            if (gtk_dialog_run(GTK_DIALOG(choose_file_dialog)) ==
                    GTK_RESPONSE_ACCEPT) {
                gchar *filename;
                filename = gtk_file_chooser_get_filename(
                        GTK_FILE_CHOOSER(choose_file_dialog));
                g_print("file: %s\n", filename);
                g_free(filename);
            }
            gtk_widget_destroy(choose_file_dialog);
            break;
    }

    if (is_field_state_changed) {
        gtk_label_set_text(GTK_LABEL(main_window.timer_widget), "000:00");
        gtk_label_set_text(GTK_LABEL(main_window._3BV_widget), "3BV: ---");
        gtk_label_set_text(GTK_LABEL(main_window.mines_count_widget), "M: ---");
        set_predefined_state(&main_window, &miner_config);
        main_window.time.tv_sec = 0;
        main_window.time.tv_usec = 0;
        gdk_window_invalidate_rect(df->widget->window, NULL, FALSE);
        field_change_status_handler(df->object, df->field);
        gtk_window_resize(GTK_WINDOW(main_window.widget), 
                df->field->width*df->cell_size,
                df->field->height*df->cell_size);
        gtk_widget_set_size_request(df->widget, 
                df->cell_size*df->field->width,
                df->cell_size*df->field->height); 
    }
}

int main(int argc, char *argv[])
{    
    GtkWidget *window;
    GtkWidget *container;
    GtkWidget *hbox_top;
    GtkWidget *hbox_bottom;
    GtkWidget *button_new;
    GtkWidget *label_field_state;
    GtkWidget *label_timer;
    GtkWidget *label_3BV;
    GtkWidget *label_mines_count;
    gint signal_id;

    GtkWidget *menu_container;
    GtkWidget *menu_bar, *menu_game;
    GtkWidget *game_menu, *game_quit, *game_large, *game_medium, *game_small,
              *game_scores, *game_video;

    GValue *value;

    MinesField *mines_field;
    DisplayField *display_field;
    GdkDrawable *draw;
    gint i;
    gchar xpm_file_name[0x100];


    read_miner_conifg(&miner_config);

    gtk_init(&argc, &argv);

    /* create main window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Miner");
    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(main_destroy), NULL);
    g_signal_connect(G_OBJECT(window), "destroy",
            G_CALLBACK(main_destroy), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    value = g_malloc0(sizeof(GValue));

    g_value_init(value, G_TYPE_BOOLEAN);
    g_value_set_boolean(value, FALSE);
    g_object_set_property(G_OBJECT(window), "resizable", value);

    /* add main container */
    gtk_vbox_new(TRUE, 0);

    container = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), container);

    /* box for top panel */
    hbox_top = gtk_hbox_new(TRUE, 0);

    /* box for bottom panel */
    hbox_bottom = gtk_hbox_new(TRUE, 0);

    /* create field status indicator label */
    label_field_state = gtk_label_new("READY");
    gtk_box_pack_end(GTK_BOX(hbox_top), label_field_state, 
            FALSE, FALSE, 0);

    /* create field */
    mines_field = mf_new(NULL, miner_config.width, 
            miner_config.height, miner_config.bombs);

    /* create display field field */
    display_field = display_field_new(mines_field, 
            G_OBJECT(label_field_state), 
            field_change_status_handler, 20);

    /* create button new*/
    button_new = gtk_button_new_with_label("NEW");
    gtk_box_pack_end(GTK_BOX(hbox_top), button_new,
            FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(button_new), "clicked",
            G_CALLBACK(button_new_field_pressed), display_field);

    /* create mines count */
    label_mines_count = gtk_label_new("M: --");
    gtk_box_pack_end(GTK_BOX(hbox_top), label_mines_count, 
            FALSE, FALSE, 0);

    /* create timer */
    label_timer = gtk_label_new("000:00");
    gtk_box_pack_end(GTK_BOX(hbox_bottom), label_timer, 
            FALSE, FALSE, 0);

    /* create 3BV */
    label_3BV = gtk_label_new("3BV: ---");
    gtk_box_pack_end(GTK_BOX(hbox_bottom), label_3BV, 
            FALSE, FALSE, 0);


    /* construct menu*/
    game_menu = gtk_menu_new();
    game_small = gtk_menu_item_new_with_label("Small");
    game_quit = gtk_menu_item_new_with_label("Quit");
    game_large = gtk_menu_item_new_with_label("Large");
    game_medium = gtk_menu_item_new_with_label("Medium");
    game_scores = gtk_menu_item_new_with_label("Scores");
    game_video = gtk_menu_item_new_with_label("Play video");

    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), game_small);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), game_medium);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), game_large);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), game_video);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), game_scores);
    gtk_menu_shell_append(GTK_MENU_SHELL(game_menu), game_quit);

    g_signal_connect(G_OBJECT(game_small), "activate",
            G_CALLBACK(menu_handler), (gpointer)MENU_GAME_SMALL);
    g_signal_connect(G_OBJECT(game_quit), "activate",
            G_CALLBACK(menu_handler), (gpointer)MENU_GAME_QUIT);
    g_signal_connect(G_OBJECT(game_large),"activate",
            G_CALLBACK(menu_handler), (gpointer)MENU_GAME_LARGE);
    g_signal_connect(G_OBJECT(game_medium), "activate",
            G_CALLBACK(menu_handler), (gpointer)MENU_GAME_MEDIUM);
    g_signal_connect(G_OBJECT(game_video), "activate",
            G_CALLBACK(menu_handler), (gpointer)MENU_GAME_VIDEO);
    g_signal_connect(G_OBJECT(game_scores), "activate",
            G_CALLBACK(menu_handler), (gpointer)MENU_GAME_SCORES);

    menu_bar = gtk_menu_bar_new();
    menu_game = gtk_menu_item_new_with_label("Game");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_game), game_menu);
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), menu_game);

    /* pack widgets into main container */
    gtk_box_pack_start(GTK_BOX(container), menu_bar, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), hbox_top, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(container), display_field->widget, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(container), hbox_bottom, FALSE, TRUE, 0);

    /* set sizes */
    /* gtk_widget_set_size_request(window, 
       mines_field->width*display_field->cell_size+40,
       mines_field->height*display_field->cell_size+150); 
       gtk_widget_set_size_request(button_new, 60, 32); */

    /* set main window */
    main_window.df = display_field;
    main_window.widget = window;
    main_window.timer_widget = label_timer;
    main_window._3BV_widget = label_3BV;
    main_window.mines_count_widget = label_mines_count;
    main_window.mode = MODE_NORMAL;

    /* set predefined window state if it is */
    set_predefined_state(&main_window, &miner_config);
    /* show widgets */
    gtk_widget_show(window);
    gtk_widget_show(container);
    gtk_widget_show(hbox_top);
    gtk_widget_show(hbox_bottom);
    gtk_widget_show(button_new);
    gtk_widget_show(label_timer);
    gtk_widget_show(label_3BV);
    gtk_widget_show(label_mines_count);
    gtk_widget_show(display_field->widget);
    gtk_widget_show(label_field_state);
    gtk_widget_show(menu_bar);
    gtk_widget_show(menu_game);
    gtk_widget_show(game_menu);
    gtk_widget_show(game_quit);
    gtk_widget_show(game_large);
    gtk_widget_show(game_medium);
    gtk_widget_show(game_small);
    gtk_widget_show(game_scores);
    gtk_widget_show(game_video);

    /* create pixmaps */
    draw = display_field->widget->window;
    pixmap_opened = gdk_pixmap_create_from_xpm(draw, NULL, NULL, OPENED_XPM);
    pixmap_marked = gdk_pixmap_create_from_xpm(draw, NULL, NULL, MARKED_XPM);
    pixmap_closed = gdk_pixmap_create_from_xpm(draw, NULL, NULL, CLOSED_XPM);
    pixmap_boom = gdk_pixmap_create_from_xpm(draw, NULL, NULL, BOOM_XPM); 
    pixmap_mine = gdk_pixmap_create_from_xpm(draw, NULL, NULL, MINE_XPM); 
    pixmap_pressed = gdk_pixmap_create_from_xpm(draw, NULL, NULL, PRESSED_XPM); 

    for (i=1; i<9; i++) {
        g_snprintf(xpm_file_name, sizeof(xpm_file_name), "pixmaps/%d.xpm", i); 
        pixmap_numbers[i-1] = gdk_pixmap_create_from_xpm(draw,
                NULL, NULL, xpm_file_name);
    }

    gtk_main();
    return 0;
}
