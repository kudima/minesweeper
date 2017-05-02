#ifndef FIELD_SNAPSHOT_H_
#define FIELD_SNAPSHOT_H_
#include "mines_field.h"

typedef struct
{
    MinesField field;
    gint index; // snapshot index
    GTimeVal time;
    gint pointer_x;
    gint pointer_y;
    gint pointer_mask;//GdkModifierType 
} FieldSnapshot;

gboolean video_recorder_function(gpointer *data);
gboolean video_player_function(gpointer *data);
#endif
