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

#include "include/mines_field.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

gint random_number(gint min, gint max)
{
	static gint dev_random_fd = -1;

	gchar* next_random_byte;
	gint bytes_to_read;
	guint random_value;

	if (max < min) return -1;

	if (dev_random_fd == -1) {
		dev_random_fd = open("/dev/urandom", O_RDONLY);
		if (dev_random_fd == -1) return -1;
	}
	next_random_byte = (gchar*) &random_value;
	bytes_to_read = sizeof(random_value);

	do {
		gint bytes_read;
		bytes_read = read(dev_random_fd, next_random_byte, bytes_to_read);
		bytes_to_read -= bytes_read;
		next_random_byte += bytes_read;
	} while (bytes_to_read > 0);

	return min + (random_value % (max - min + 1));
}

MinesField* mf_new(MinesField* mf, gint width, gint height, gint bombs)
{
	gint i, j;
	gint bombed_cell;
	MinesField *mines_field;

	if (mf == NULL)
		mines_field = (MinesField*)g_malloc(sizeof(MinesField));
	else {
		mines_field = mf;
		for (i = 0; i < mf->width; i++)
			g_free(mf->cell[i]);
		g_free(mf->cell);
	}

	mines_field->cell = (gint**)g_malloc(sizeof(int*)*width);

	for (i = 0; i < width; i++)
		mines_field->cell[i] = (gint*)g_malloc(sizeof(int) * height);

	mines_field->width = width;
	mines_field->height = height;
	mines_field->is_fail = FALSE;
	mines_field->opened_count = 0;
	mines_field->fully_opened = FALSE;
	mines_field->bombs = 0;
	mines_field->marked_count = 0;

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++)  {
			mines_field->cell[i][j] = 0;
			mines_field->cell[i][j] |= NEED_UPDATE;
		}
	}

	for (; mines_field->bombs < bombs;) {
		bombed_cell = random_number(0, width * height - 1);
		if (bombed_cell < 0) {
			fprintf(stderr, "random failed\n");
			bombed_cell = 0;
		}
		i = (gint)(bombed_cell / mines_field->height);
		j = (gint)(bombed_cell % mines_field->height);

		if (!(mines_field->cell[i][j] & BOMBED)) {
			mines_field->cell[i][j] |= BOMBED;
			mines_field->bombs++;
		}
	}
	return (mines_field);
}

void mf_set_opened(MinesField *mf, gint x, gint y)
{
	if (x >= 0 && x < mf->height && y >= 0 && y < mf->width && !(mf->cell[x][y]&MARKED))
		mf->cell[x][y] |= OPENED;
}

void mf_set_mraked(MinesField *mf, gint x, gint y)
{
	if (x >= 0 && x < mf->height && y >= 0 && y < mf->width && !(mf->cell[x][y]&OPENED)) {
		mf->cell[x][y] |= MARKED;
		mf->marked_count++;
	}
}

gint mf_get_number(MinesField *mf, gint x, gint y)
{
	int bombs_count = 0;
	int i, j;
	for (i = max(0, x - 1); i < min(mf->width, x + 2); i++) {
		for (j = max(0, y - 1); j < min(mf->height, y + 2); j++) {
			if (mf->cell[i][j]&BOMBED)
				bombs_count++;
		}
	}

	return bombs_count;
}

gint mf_get_marked(MinesField *mf, gint x, gint y)
{
	int marked_count = 0;
	int i, j;
	for (i = max(0, x - 1); i < min(mf->width, x + 2); i++)
		for (j = max(0, y - 1); j < min(mf->height, y + 2); j++)
			if (mf->cell[i][j]&MARKED)
				marked_count++;

	return marked_count;
}

gboolean mf_set_state(MinesField *mf,
                      gint x, gint y,
                      SellStatus status)
{
	int i, j;
	SellStatus prev_status;

	if ((x < 0 || x >= mf->width)
	        || (y < 0 || y >= mf->height))
		return FALSE;

	prev_status = mf->cell[x][y];

	if ((status & OPENED) && !(mf->cell[x][y]&MARKED)
	        && !(mf->cell[x][y]&OPENED)) {

		mf->cell[x][y] |= OPENED;
		mf->opened_count++;
		if (mf_get_number(mf, x, y) == 0)
			for (i = max(0, x - 1); i < min(mf->width, x + 2); i++)
				for (j = max(0, y - 1); j < min(mf->height, y + 2); j++)
					if (!(mf->cell[i][j]&OPENED) && !(mf->cell[i][j]&MARKED))
						mf_set_state(mf, i, j, OPENED);
	}

	if ((status & MARKED) && (mf->cell[x][y]&MARKED)) {
		mf->cell[x][y] ^= MARKED;
		mf->marked_count--;
	} else if ((status & MARKED) && !(mf->cell[x][y]&OPENED)) {
		mf->cell[x][y] |= MARKED;
		mf->marked_count++;
	}

	if ((mf->cell[x][y]&OPENED) && (mf->cell[x][y]&BOMBED)) {
		if (mf->opened_count <= 1) {
			mf_new_field(mf);
			mf_set_state(mf, x, y, OPENED);
		} else {
			mf->is_fail = TRUE;
			return TRUE;
		}
	}

	if (prev_status != mf->cell[x][y])
		mf->cell[x][y] |= NEED_UPDATE;

	if ((mf->opened_count >= mf->height * mf->width - mf->bombs))
		mf->fully_opened = TRUE;

	return TRUE;
}

gboolean mf_set_state_around(MinesField *mf,
                             gint x, gint y,
                             gint state)
{
	int i, j;

	if ((x < 0 || x >= mf->width)
	        || (y < 0 || y >= mf->height))
		return FALSE;

	if ((mf_get_marked(mf, x, y) == mf_get_number(mf, x, y)
	        && state == OPENED && mf_get_marked(mf, x, y) > 0)) {
		for (i = max(0, x - 1); i < min(mf->width, x + 2); i++)
			for (j = max(0, y - 1); j < min(mf->height, y + 2); j++)
				if (!(mf->cell[i][j]&OPENED) && !(mf->cell[i][j]&MARKED))
					mf_set_state(mf, i, j, state);
	}

	return TRUE;
}

void mf_new_field(MinesField *mf)
{
	gint i, j;
	gint bombs;
	gint bombed_cell;

	mf->is_fail = FALSE;
	mf->opened_count = 0;
	mf->fully_opened = FALSE;

	bombs = mf->bombs;
	mf->bombs = 0;

	for (i = 0; i < mf->width; i++) {
		for (j = 0; j < mf->height; j++)  {
			if (!(mf->cell[i][j]&NEED_UPDATE))
				mf->cell[i][j] = 0;
			else {
				mf->cell[i][j] = 0;
				mf->cell[i][j] |= NEED_UPDATE;
			}
		}
	}

	for (; mf->bombs < bombs;) {

		bombed_cell = random_number(0, mf->width * mf->height - 1);
		i = (gint)(bombed_cell / mf->height);
		j = (gint)(bombed_cell % mf->height);

		if (!(mf->cell[i][j] & BOMBED)) {
			mf->cell[i][j] |= BOMBED;
			mf->bombs++;
		}
	}
}

gint mf_get_3BV_index(MinesField *mf, gint x, gint y)
{
	gint i, j;
	gint _3BV_index = 0;

	if ((x < 0 || x >= mf->width)
	        || (y < 0 || y >= mf->height))
		return -1;

	if (!(mf->cell[x][y]&MARKED)) {
		mf->cell[x][y] |= MARKED;
		_3BV_index = 1;
		if (mf_get_number(mf, x, y) == 0)
			for (i = max(0, x - 1); i < min(mf->width, x + 2); i++)
				for (j = max(0, y - 1); j < min(mf->height, y + 2); j++)
					if (!(mf->cell[i][j]&MARKED))
						mf_get_3BV_index(mf, i, j);
	}

	return _3BV_index;
}

gint mf_get_3BV(MinesField *mf)
{
	int i, j;
	int _3BV_value = 0;
	for (i = 0; i < mf->width; i++)
		for (j = 0; j < mf->height; j++)
			if (mf_get_number(mf, i, j) == 0)
				_3BV_value += mf_get_3BV_index(mf, i, j);

	for (i = 0; i < mf->width; i++)
		for (j = 0; j < mf->height; j++)
			if (mf_get_number(mf, i, j) != 0
			        && !(mf->cell[i][j]&BOMBED))
				_3BV_value += mf_get_3BV_index(mf, i, j);

	for (i = 0; i < mf->width; i++)
		for (j = 0; j < mf->height; j++)
			mf->cell[i][j] &= ~MARKED;

	return _3BV_value;
}


