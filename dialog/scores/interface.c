/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <glib/gprintf.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "../../include/main.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window_score (MinerConfig *config)
{
    GtkWidget *window_score;
    GtkWidget *vbox_score;
    GtkWidget *score_label;
    GtkWidget *hbox_small;
    GtkWidget *label_small;
    GtkWidget *label_small_time;
    GtkWidget *hbox_medium;
    GtkWidget *label_medium;
    GtkWidget *label_medium_time;
    GtkWidget *hbox_large;
    GtkWidget *label_large;
    GtkWidget *label_large_time;
    GtkWidget *button_ok;
    GtkWidget *alignment1;
    GtkWidget *hbox5;
    GtkWidget *image1;
    GtkWidget *label11;

    gchar str_time[0x10];
    gint second;
    gint dec_second;

    window_score = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (window_score), 2);
    gtk_window_set_title (GTK_WINDOW (window_score), "score");
    gtk_window_set_position (GTK_WINDOW (window_score), GTK_WIN_POS_CENTER_ON_PARENT);
    gtk_window_set_modal (GTK_WINDOW (window_score), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (window_score), FALSE);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (window_score), TRUE);
    gtk_window_set_type_hint (GTK_WINDOW (window_score), GDK_WINDOW_TYPE_HINT_DIALOG);

    vbox_score = gtk_vbox_new (TRUE, 2);
    gtk_widget_show (vbox_score);
    gtk_container_add (GTK_CONTAINER (window_score), vbox_score);
    gtk_widget_set_size_request (vbox_score, 175, 233);
    gtk_container_set_border_width (GTK_CONTAINER (vbox_score), 20);

    score_label = gtk_label_new ("Score");
    gtk_widget_show (score_label);
    gtk_box_pack_start (GTK_BOX (vbox_score), score_label, FALSE, FALSE, 0);
    gtk_label_set_justify (GTK_LABEL (score_label), GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize (GTK_LABEL (score_label), PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_single_line_mode (GTK_LABEL (score_label), TRUE);

    hbox_small = gtk_hbox_new (FALSE, 5);
    gtk_widget_show (hbox_small);
    gtk_box_pack_start (GTK_BOX (vbox_score), hbox_small, FALSE, FALSE, 10);

    label_small = gtk_label_new ("Small time:");
    gtk_widget_show (label_small);
    gtk_box_pack_start (GTK_BOX (hbox_small), label_small, FALSE, FALSE, 0);

    second = config->small_time.tv_sec;
    dec_second = config->small_time.tv_usec/10000;
    g_sprintf(str_time, "%3.3d:%2.2d", second, dec_second);
    label_small_time = gtk_label_new (str_time);
    gtk_widget_show (label_small_time);
    gtk_box_pack_start (GTK_BOX (hbox_small), label_small_time, FALSE, FALSE, 0);

    hbox_medium = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox_medium);
    gtk_box_pack_start (GTK_BOX (vbox_score), hbox_medium, FALSE, TRUE, 10);

    label_medium = gtk_label_new ("Medium time: ");
    gtk_widget_show (label_medium);
    gtk_box_pack_start (GTK_BOX (hbox_medium), label_medium, FALSE, FALSE, 0);

    second = config->medium_time.tv_sec;
    dec_second = config->medium_time.tv_usec/10000;
    g_sprintf(str_time, "%3.3d:%2.2d", second, dec_second);
    label_medium_time = gtk_label_new (str_time);
    gtk_widget_show (label_medium_time);
    gtk_box_pack_start (GTK_BOX (hbox_medium), label_medium_time, FALSE, FALSE, 0);

    hbox_large = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox_large);
    gtk_box_pack_start (GTK_BOX (vbox_score), hbox_large, FALSE, TRUE, 10);

    label_large = gtk_label_new ("Large time: ");
    gtk_widget_show (label_large);
    gtk_box_pack_start (GTK_BOX (hbox_large), label_large, FALSE, FALSE, 0);

    second = config->large_time.tv_sec;
    dec_second = config->large_time.tv_usec/10000;
    g_sprintf(str_time, "%3.3d:%2.2d", second, dec_second);
    label_large_time = gtk_label_new (str_time);
    gtk_widget_show (label_large_time);
    gtk_box_pack_start (GTK_BOX (hbox_large), label_large_time, FALSE, FALSE, 0);

    button_ok = gtk_button_new ();
    gtk_widget_show (button_ok);
    gtk_box_pack_end (GTK_BOX (vbox_score), button_ok, FALSE, FALSE, 0);
    gtk_widget_set_size_request (button_ok, 48, 32);
    GTK_WIDGET_UNSET_FLAGS (button_ok, GTK_CAN_FOCUS);
    gtk_button_set_relief (GTK_BUTTON (button_ok), GTK_RELIEF_HALF);

    alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_widget_show (alignment1);
    gtk_container_add (GTK_CONTAINER (button_ok), alignment1);

    hbox5 = gtk_hbox_new (FALSE, 2);
    gtk_widget_show (hbox5);
    gtk_container_add (GTK_CONTAINER (alignment1), hbox5);

    image1 = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show (image1);
    gtk_box_pack_start (GTK_BOX (hbox5), image1, FALSE, FALSE, 0);

    label11 = gtk_label_new_with_mnemonic ("ok");
    gtk_widget_show (label11);
    gtk_box_pack_start (GTK_BOX (hbox5), label11, FALSE, FALSE, 0);

    g_signal_connect ((gpointer) window_score, "activate_default",
		    G_CALLBACK (on_window_score_activate_default),
		    NULL);

    g_signal_connect ((gpointer) button_ok, "clicked",
		    G_CALLBACK (on_button_ok_clicked),
		    window_score);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (window_score, window_score, "window_score");
    GLADE_HOOKUP_OBJECT (window_score, vbox_score, "vbox_score");
    GLADE_HOOKUP_OBJECT (window_score, score_label, "score_label");
    GLADE_HOOKUP_OBJECT (window_score, hbox_small, "hbox_small");
    GLADE_HOOKUP_OBJECT (window_score, label_small, "label_small");
    GLADE_HOOKUP_OBJECT (window_score, label_small_time, "label_small_time");
    GLADE_HOOKUP_OBJECT (window_score, hbox_medium, "hbox_medium");
    GLADE_HOOKUP_OBJECT (window_score, label_medium, "label_medium");
    GLADE_HOOKUP_OBJECT (window_score, label_medium_time, "label_medium_time");
    GLADE_HOOKUP_OBJECT (window_score, hbox_large, "hbox_large");
    GLADE_HOOKUP_OBJECT (window_score, label_large, "label_large");
    GLADE_HOOKUP_OBJECT (window_score, label_large_time, "label_large_time");
    GLADE_HOOKUP_OBJECT (window_score, button_ok, "button_ok");
    GLADE_HOOKUP_OBJECT (window_score, alignment1, "alignment1");
    GLADE_HOOKUP_OBJECT (window_score, hbox5, "hbox5");
    GLADE_HOOKUP_OBJECT (window_score, image1, "image1");
    GLADE_HOOKUP_OBJECT (window_score, label11, "label11");

    return window_score;
}

