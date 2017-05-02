/* Main structures and enumerations */
#ifndef MAIN_H
#define MAIN_H
#include "display_field.h"



enum menu_items 
{
    MENU_GAME_QUIT,
    MENU_GAME_LARGE, 
    MENU_GAME_MEDIUM,
    MENU_GAME_SMALL, 
    MENU_GAME_SCORES, 
    MENU_GAME_VIDEO
};

enum predefined 
{
    PREDEFINED_SMALL,
    PREDEFINED_MEDIUM,
    PREDEFINED_LARGE,
    PREDEFINED_NONE
};

enum modes
{
    MODE_NORMAL,
    MODE_VIDEO
};

typedef struct 
{
    DisplayField *df;
    GtkWidget *widget;
    GtkWidget *timer_widget;
    GtkWidget *_3BV_widget;
    GtkWidget *mines_count_widget;
    GTimeVal time; /* game duration */
    gint predefined_state;
    gint mode; /* Normar or playing video mode */
} MainWindow;

typedef struct
{
    gint bombs;
    gint width;
    gint height;
    GTimeVal small_time;
    GTimeVal medium_time;
    GTimeVal large_time;
} MinerConfig;

/* The main window widget */
MainWindow main_window;
/* The application current state */
MinerConfig miner_config;


#endif

