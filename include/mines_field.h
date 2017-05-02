#ifndef MINES_FIELD 
#define MINES_FIELD 
#include <gtk/gtk.h>
typedef enum
{
    BOMBED = 1,
    OPENED = 1 << 1,
    MARKED = 1 << 2,
    NEED_UPDATE = 1 << 3,
} SellStatus;

typedef struct 
{
    gint height;
    gint width;
    gint bombs;
    gboolean is_fail;
    gint opened_count;
    gboolean fully_opened;
    gint marked_count;
    int **cell; // matrix
} MinesField;

#define max(x, y) ( (x) > (y) ? (x) : (y) )
#define min(x, y) ( (x) < (y) ? (x) : (y) )

/* mines field */
MinesField* mf_new(MinesField* mf, gint width, gint height, gint bombs);

gint mf_get_number(MinesField *mf, gint x, gint y);

gint mf_get_marked(MinesField *mf, gint x, gint y);

gboolean mf_set_state(MinesField *mf, 
	gint x, gint y,
       	SellStatus status);
/**
 * df_set_state_around(gint x, gint y)
 *
 * set all neigours status to opened if possible 
 *
 * @param gint x
 * @param gint y 
 */
gboolean mf_set_state_around(MinesField *mf,
       	gint x, gint y,
	gint state);

void mf_new_field(MinesField *mf);
gint mf_get_3BV(MinesField *mf);
#endif
