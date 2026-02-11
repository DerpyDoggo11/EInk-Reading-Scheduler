#include <gtk-2.0/gtk/gtk.h>
#include <sqlite3.h>

#include "utils/generalUtils.h"
#include "screens/mainScreen.h"
#include "screens/bookSelectorScreen.h"

#include <iostream>
#include <thread>
#include <list> 
#include <string>


int main(int argc, char *argv[])
{

  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "KoScheduler");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 800);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  openMainScreen(vbox);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
