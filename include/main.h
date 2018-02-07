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
    gint mode; /* Normal or playing video mode */
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



#endif

