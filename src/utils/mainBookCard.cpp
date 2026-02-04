#include "mainBookCard.h" 

#include "../icons/streak_icon.h" 
#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>
#include <list> 
#include <string>


GtkWidget* create_book_card(const char *title, float currentProgress, float totalProgress, float streak){
  GtkWidget *event = gtk_event_box_new(); // used to set background color (unused)
  //GdkColor bg; gdk_color_parse("#868686", &bg); 
  //gtk_widget_modify_bg(event, GTK_STATE_NORMAL, &bg);
  //gtk_container_set_border_width(GTK_CONTAINER(event), 10);

  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);

  GtkWidget *box = gtk_vbox_new(FALSE, 2);
  gtk_container_add(GTK_CONTAINER(frame), box);

  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(title), FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(box), gtk_label_new("----------------------------------"), FALSE, FALSE, 0);

  // Current Progress
  std::string currentProgressText = std::to_string(currentProgress) + (currentProgress == 1 ? " page left today" : " pages left today");
  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(currentProgressText.c_str()), FALSE, FALSE, 0);

  // Total Progress
  std::string totalProgressText = std::to_string(totalProgress) + (totalProgress == 1 ? " page until completion" : " pages until completion");
  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(totalProgressText.c_str()), FALSE, FALSE, 0);

  GtkWidget *streakBox = gtk_hbox_new(FALSE, 5); // Load embedded fire icon 
  GdkPixbuf *streakPixbuf = gdk_pixbuf_new_from_inline(streak_png_len, streak_png, FALSE, NULL); 

  if (streakPixbuf) { 
    GtkWidget *streakIcon = gtk_image_new_from_pixbuf(streakPixbuf); 
    gtk_box_pack_start(GTK_BOX(streakBox), streakIcon, FALSE, FALSE, 0); 
    g_object_unref(streakPixbuf); 
  }

  // Streak
  std::string streakText = std::to_string(streak) + (streak == 1 ? " day streak" : " days streak");
  gtk_box_pack_start(GTK_BOX(streakBox), gtk_label_new(streakText.c_str()), FALSE, FALSE, 0); 
  gtk_box_pack_start(GTK_BOX(box), streakBox, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(event), frame);

  return event;
}
