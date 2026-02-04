#include "mainScreen.h"
#include "../utils/mainBookCard.h"
#include "../utils/readDatabase.h"

#include "../icons/book_icon.h"
#include "../icons/quit_icon.h"
#include "../icons/streak_icon.h"
#include "../icons/add_icon.h"
#include "../icons/shop_icon.h"

#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>
#include <list> 
#include <string>

GtkWidget *mainMenuLeftMenubar;
GtkWidget *currentBooksScrollbar;

// void on_button_addBook()
// {
//   gtk_widget_hide(mainMenuLeftMenubar);
//   =gtk_widget_hide(currentBooksScrollbar);

//   gtk_widget_show(backButtonLeftMenubar);
//   gtk_widget_show(addBooksValueFields);
// }

// void on_button_back()
// {
//   gtk_widget_hide(backButtonLeftMenubar);
//   gtk_widget_hide(addBooksValueFields);

//   gtk_widget_show(mainMenuLeftMenubar);
//   gtk_widget_show(currentBooksScrollbar);
// }


GtkWidget* createMainScreen(GtkWidget *window){

    GtkWidget *screenVBox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), screenVBox);

    // Title
    GtkWidget *titleMi = gtk_menu_item_new();
    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titleLabel), "<b>KoScheduler</b>");

    gtk_container_add(GTK_CONTAINER(titleMi), titleLabel);
    gtk_widget_set_sensitive(titleMi, FALSE);
    gtk_widget_set_can_focus(titleMi, FALSE);

    gtk_misc_set_alignment(GTK_MISC(titleLabel), 0.5, 0.5);

    // Shop Button
    GtkWidget *shopMi = gtk_menu_item_new();
    GtkWidget *shopHbox = gtk_hbox_new(FALSE, 6);

    GtkWidget *shopLabel = gtk_label_new("Open Shop");
    gtk_box_pack_start(GTK_BOX(shopHbox), shopLabel, FALSE, FALSE, 0);

    GdkPixbuf *shopPixbuf = gdk_pixbuf_new_from_inline(shop_png_len, shop_png, FALSE, NULL);
    GdkPixbuf *shopScaled = gdk_pixbuf_scale_simple(shopPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *shopIcon = gtk_image_new_from_pixbuf(shopScaled);
    g_object_unref(shopPixbuf);
    gtk_box_pack_start(GTK_BOX(shopHbox), shopIcon, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(shopMi), shopHbox);
    
    // Add Book Button
    GtkWidget *addBookMi = gtk_menu_item_new();
    GtkWidget *addBookHbox = gtk_hbox_new(FALSE, 6);

    GtkWidget *addBookLabel = gtk_label_new("Add Book From Library");
    gtk_box_pack_start(GTK_BOX(addBookHbox), addBookLabel, FALSE, FALSE, 0);

    GdkPixbuf *addPixbuf = gdk_pixbuf_new_from_inline(add_png_len, add_png, FALSE, NULL);
    GdkPixbuf *addScaled = gdk_pixbuf_scale_simple(addPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *addIcon = gtk_image_new_from_pixbuf(addScaled);
    g_object_unref(addPixbuf);
    gtk_box_pack_start(GTK_BOX(addBookHbox), addIcon, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(addBookMi), addBookHbox);

    // Quit Button
    GtkWidget *quitMi = gtk_menu_item_new();

    GtkWidget *align = gtk_alignment_new(1.0, 0.5, 0, 0); // xalign=1.0 → right aligned

    GdkPixbuf *quitPixbuf = gdk_pixbuf_new_from_inline(quit_png_len, quit_png, FALSE, NULL);
    GdkPixbuf *quitScaled = gdk_pixbuf_scale_simple(quitPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *quitIcon = gtk_image_new_from_pixbuf(quitScaled);
    g_object_unref(quitPixbuf);

    gtk_container_add(GTK_CONTAINER(align), quitIcon);
    gtk_container_add(GTK_CONTAINER(quitMi), align);

    // Back Button
    GtkWidget *backMi = gtk_menu_item_new();
    GtkWidget *backLabel = gtk_label_new("Back");
    gtk_container_add(GTK_CONTAINER(backMi), backLabel);

    // Top Menubar
    mainMenuLeftMenubar = gtk_menu_bar_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenuLeftMenubar), titleMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenuLeftMenubar), shopMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenuLeftMenubar), addBookMi);

    GtkWidget *rightMenubar = gtk_menu_bar_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(rightMenubar), quitMi);

    GtkWidget *menubarHBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(menubarHBox), mainMenuLeftMenubar, TRUE, TRUE, 0);
    //gtk_box_pack_start(GTK_BOX(menubarHBox), backButtonLeftMenubar, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(menubarHBox), rightMenubar, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(screenVBox), menubarHBox, FALSE, FALSE, 0);
    


    // Scrollable Books Widget
    currentBooksScrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(currentBooksScrollbar), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(screenVBox), currentBooksScrollbar, TRUE, TRUE, 5);

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

    //   g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);
    //   g_signal_connect(G_OBJECT(addBookMi), "activate", G_CALLBACK(on_button_addBook), NULL);
    //   g_signal_connect(G_OBJECT(backMi), "activate", G_CALLBACK(on_button_back), NULL);

    return screenVBox;
}