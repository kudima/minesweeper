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

#include "include/field_snapshot.h"
#include "include/display_field.h"
#include "include/mines_field.h"
#include "include/main.h"

static gchar *current_video = NULL;
static gint current_video_index = 0;

gboolean video_recorder_function(gpointer *data)
{	/*
	   MinesField *mf;
	   FieldSnapshot fs;
	   gint pointer_mask;
	   gint pointer_x;
	   gint pointer_y;

	   mf = (MinesField*)data;
	   if (main_window.time.tv_sec > 999
	 || mf->fully_opened
	 || mf->is_fail
	 || mf->opened_count == 0) {
	FILE *fd_test_video;
	fd_test_video = fopen("current_test.video", "w");
	fwrite(current_video, sizeof(FieldSnapshot),
	       	current_video_index+1, fd_test_video);
	fclose(fd_test_video);
	g_free(current_video);
	current_video = NULL;
	      	return FALSE;
	   }*/
	/* Allocates memory for the next snapshot */
	/*    if (current_video == NULL) {
		current_video = g_malloc0(sizeof(FieldSnapshot));
		current_video_index = 0;
	    } else {
		current_video_index++;
		current_video = g_realloc(current_video,
		       	(current_video_index+1)*sizeof(FieldSnapshot));
	    }

	    g_memmove((gpointer)&fs.field, (gpointer)mf, sizeof(MinesField));
	    fs.index = current_video_index;
	    fs.time.tv_sec = main_window.time.tv_sec;
	    fs.time.tv_usec = main_window.time.tv_usec;
	    gdk_window_get_pointer(main_window.df->widget->window,
		    &pointer_x, &pointer_y, &pointer_mask);
	    fs.pointer_x = pointer_x;
	    fs.pointer_y = pointer_y;
	    fs.pointer_mask = pointer_mask;

	    g_memmove((gpointer)(current_video+
			current_video_index*sizeof(FieldSnapshot)),
		    (gpointer)&fs, sizeof(FieldSnapshot));*/

	return TRUE;
}

gboolean video_player_function(gpointer *data)
{
}
/*
 * filename - null terminated string which contains
 * video file name.
 */
gboolean play_video(gchar *filename, ...)
{
	/*
	FILE *fd_video;
	FieldSnapshot fs;
	gint ret;
	current_video_index = 0;
	current_video = NULL;

	fd_video = fopen(filename, "r");
	while (!eof(fd_video)) {
	ret = fread(&fs, sizeof(FieldSnapshot), 1, fd_video);
	if (ret != sizeof(FieldSnapshot))
	    break;
	current_video = g_realloc(current_video,
		(current_index+1)sizeof(FieldSnapshot));
	g_memmove((current_video+current_video_index*sizeof(FieldSnapshot)),
	       &fs, sizeof(FieldSnapshot));
	}*/

	return TRUE;
}



