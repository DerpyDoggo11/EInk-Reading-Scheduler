#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>

void on_button_quit()
{
  g_print("Quit button clicked, goodbye!\n");
  gtk_main_quit();
}

GtkWidget* create_book_card(const char* title, int currentProgress, int totalProgress, int streak){
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);

    GtkWidget *box = gtk_vbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(frame), box);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new(title), FALSE, FALSE, 0);

    // Current Progress
    std::string currentProgressText = std::to_string(currentProgress) + (streak == 1 ? " page left today" : " pages left today");
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new(currentProgressText.c_str()), FALSE, FALSE, 0);

    // Total Progress
    std::string totalProgressText = std::to_string(totalProgress) + (streak == 1 ? " page until completion" : " pages until completion");
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new(totalProgressText.c_str()), FALSE, FALSE, 0);

    // Streak
    std::string streakText = std::to_string(streak) + (streak == 1 ? " day streak" : " days streak");
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new(streakText.c_str()), FALSE, FALSE, 0);

    return frame;
}


int main(int argc, char *argv[])
{

  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Reading Scheduler");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 800);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *addBookMi = gtk_menu_item_new_with_label("Add Book from Library");
  GtkWidget *quitMi = gtk_menu_item_new_with_label("Quit");
  // root / menubar
  GtkWidget *menubar = gtk_menu_bar_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), quitMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), addBookMi);
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

  // Populate screen with a center aligned scrollable widget
  GtkWidget *currentBooksScrollbar = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(currentBooksScrollbar), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), currentBooksScrollbar, TRUE, TRUE, 5);

  GtkWidget *centerAlign = gtk_alignment_new(0.5, 0, 0.5, 0);
  GtkWidget *listBox = gtk_vbox_new(FALSE, 10);

  gtk_container_add(GTK_CONTAINER(centerAlign), listBox);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(currentBooksScrollbar), centerAlign);

  // title, daily progress (current about of finished pages / how many pages left), streak, finish date
  GtkWidget *card1 = create_book_card("Digital Signal Progressing", 100, 90, 24);
  gtk_box_pack_start(GTK_BOX(listBox), card1, FALSE, FALSE, 10);
  GtkWidget *card2 = create_book_card("Rocket Propulsion Elements", 0, 1,1);
  gtk_box_pack_start(GTK_BOX(listBox), card2, FALSE, FALSE, 10);
  GtkWidget *card3 = create_book_card("Linear Algebra Introduction", 20, 50, 24);
  gtk_box_pack_start(GTK_BOX(listBox), card3, FALSE, FALSE, 10);
  GtkWidget *card4 = create_book_card("Quadruped Robot Leg Design", 50, 10, 4);
  gtk_box_pack_start(GTK_BOX(listBox), card4, FALSE, FALSE, 10);


  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
