#include <gtk-2.0/gtk/gtk.h>
#include <sqlite3.h>
#include "icons/book_icon.h"
#include "icons/quit_icon.h"
#include "icons/streak_icon.h"
#include "icons/add_icon.h"
#include <iostream>
#include <thread>
#include <list> 
#include <string>

GtkWidget *create_book_card(const char *title, float currentProgress, float totalProgress, float streak)
{
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

struct bookData{
  std::string title;
  std::string contentID;
  float progress;
};

std::list<bookData> retrieveLibraryData(const std::string &databasePath)
{
  sqlite3 *db = nullptr;
  sqlite3_stmt *stmt = nullptr;

  std::list<bookData> userData;

  if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
  {
    g_print("Failed to open Kobo database\n");
    return userData;
  }

  // join content and content settings database, then filter the values
  const char *sql =
      "SELECT c.Title, c.ContentID, cs.Progress "
      "FROM content c "
      "LEFT JOIN content_settings cs ON c.ContentID = cs.ContentID "
      "WHERE c.ContentType = 6 "
      "ORDER BY c.Title COLLATE NOCASE;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
  {
    g_print("Failed to prepare SQL statement\n");
    sqlite3_close(db);
    return userData;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    const unsigned char *titleText = sqlite3_column_text(stmt, 0);
    const unsigned char *idText = sqlite3_column_text(stmt, 1);
    float progress = sqlite3_column_int(stmt, 2);

    bookData entry;

    if (titleText) {
      entry.title = reinterpret_cast<const char *>(titleText);
    } else { entry.title = ""; }

    if (idText) {
      entry.contentID = reinterpret_cast<const char *>(idText);
    } else { entry.contentID = ""; }
    
    entry.progress = progress;

    userData.push_back(entry);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return userData;
}

void on_button_quit()
{
  g_print("Quit button clicked, goodbye!\n");
  gtk_main_quit();
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

  // Quit Button
  GtkWidget *quitMi = gtk_menu_item_new();

  GdkPixbuf *quitPixbuf = gdk_pixbuf_new_from_inline(quit_png_len, quit_png, FALSE, NULL);
  GtkWidget *quitIcon = gtk_image_new_from_pixbuf(quitPixbuf);
  g_object_unref(quitPixbuf);
  gtk_container_add(GTK_CONTAINER(quitMi), quitIcon);

  // Add Book Button
  GtkWidget *addBookMi = gtk_menu_item_new();

  GtkWidget *hbox = gtk_hbox_new(FALSE, 6);

  GdkPixbuf *addPixbuf = gdk_pixbuf_new_from_inline(add_png_len, add_png, FALSE, NULL);
  GtkWidget *addIcon = gtk_image_new_from_pixbuf(addPixbuf);
  g_object_unref(addPixbuf);
  gtk_box_pack_start(GTK_BOX(hbox), addIcon, FALSE, FALSE, 0);

  GdkPixbuf *bookPixbuf = gdk_pixbuf_new_from_inline(book_png_len, book_png, FALSE, NULL);
  GtkWidget *bookIcon = gtk_image_new_from_pixbuf(bookPixbuf);
  g_object_unref(bookPixbuf);
  gtk_box_pack_start(GTK_BOX(hbox), bookIcon, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(addBookMi), hbox);

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

  // DEV PATH: "./emulator/kobo/.kobo/KoboReader.sqlite"
  // KOBO PATH: "/mnt/onboard/.kobo/KoboReader.sqlite"
  std::list<bookData> userData = retrieveLibraryData("./emulator/kobo/.kobo/KoboReader.sqlite");

  for (auto &book : userData) {
    std::cout << book.title;
    GtkWidget *card = create_book_card(
        book.title.c_str(),
        book.progress, // percentage of pages read today (percentage of overall yesterday - predicted percentage of overal today)
        book.progress * 100, // percentage left overall 
        1
    );

    gtk_box_pack_start(GTK_BOX(listBox), card, FALSE, FALSE, 10);
  }

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
