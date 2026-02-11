#include "mainScreen.h"
#include "../screens/bookSelectorScreen.h"
#include "../utils/mainBookCard.h"
#include "../utils/readDatabase.h"
#include "../utils/localDatabase.h"
#include "../utils/generalUtils.h"

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
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>

const std::string KOBO_DB_PATH = "./emulator/kobo/.kobo/KoboReader.sqlite";
const std::string LOCAL_DB_PATH = "./emulator/kobo/selected_books.db";

// actual kobo paths (uncomment for actual kobos)
// const std::string KOBO_DB_PATH = "/mnt/onboard/.kobo/KoboReader.sqlite";
// const std::string LOCAL_DB_PATH = "/mnt/onboard/.kobo/selected_books.db";


void on_button_addBook(GtkWidget *menuItem, gpointer user_data) {
    GtkWidget *vBox = GTK_WIDGET(user_data);
    openBookSelectorScreen(vBox);
}

void openMainScreen(GtkWidget *vBox){

    clearWindow(vBox);
    setBackground(vBox, "#ffffff");

    initLocalDatabase(LOCAL_DB_PATH);
    
    std::string currentDate = getCurrentDate();

    GtkWidget *titleMi = gtk_menu_item_new();
    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titleLabel), setMarkup("KoScheduler", 15000).c_str());

    gtk_container_add(GTK_CONTAINER(titleMi), titleLabel);
    gtk_widget_set_sensitive(titleMi, FALSE);
    gtk_widget_set_can_focus(titleMi, FALSE);

    gtk_misc_set_alignment(GTK_MISC(titleLabel), 0.5, 0.5);

    GtkWidget *shopMi = gtk_menu_item_new();
    GtkWidget *shopHbox = gtk_hbox_new(FALSE, 6);

    GtkWidget *shopLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(shopLabel), setMarkup("Open Shop", 10000).c_str());
    gtk_box_pack_start(GTK_BOX(shopHbox), shopLabel, FALSE, FALSE, 0);

    GdkPixbuf *shopPixbuf = gdk_pixbuf_new_from_inline(shop_png_len, shop_png, FALSE, NULL);
    GdkPixbuf *shopScaled = gdk_pixbuf_scale_simple(shopPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *shopIcon = gtk_image_new_from_pixbuf(shopScaled);
    g_object_unref(shopPixbuf);
    gtk_box_pack_start(GTK_BOX(shopHbox), shopIcon, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(shopMi), shopHbox);
    
    GtkWidget *addBookMi = gtk_menu_item_new();
    GtkWidget *addBookHbox = gtk_hbox_new(FALSE, 6);

    GtkWidget *addBookLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(addBookLabel), setMarkup("Add Book From Library", 10000).c_str());
    gtk_box_pack_start(GTK_BOX(addBookHbox), addBookLabel, FALSE, FALSE, 0);

    GdkPixbuf *addPixbuf = gdk_pixbuf_new_from_inline(add_png_len, add_png, FALSE, NULL);
    GdkPixbuf *addScaled = gdk_pixbuf_scale_simple(addPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *addIcon = gtk_image_new_from_pixbuf(addScaled);
    g_object_unref(addPixbuf);
    gtk_box_pack_start(GTK_BOX(addBookHbox), addIcon, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(addBookMi), addBookHbox);

    GtkWidget *timeMi = gtk_menu_item_new();
    GtkWidget *timeLabel = gtk_label_new(NULL);
    std::string currentTime = getCurrentTime();
    gtk_label_set_markup(GTK_LABEL(timeLabel), setMarkup(currentTime, 10000).c_str());
    gtk_container_add(GTK_CONTAINER(timeMi), timeLabel);
    gtk_widget_set_sensitive(timeMi, FALSE);
    gtk_widget_set_can_focus(timeMi, FALSE);

    GtkWidget *quitMi = gtk_menu_item_new();

    GtkWidget *align = gtk_alignment_new(1.0, 0.5, 0, 0);

    GdkPixbuf *quitPixbuf = gdk_pixbuf_new_from_inline(quit_png_len, quit_png, FALSE, NULL);
    GdkPixbuf *quitScaled = gdk_pixbuf_scale_simple(quitPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *quitIcon = gtk_image_new_from_pixbuf(quitScaled);
    g_object_unref(quitPixbuf);

    gtk_container_add(GTK_CONTAINER(align), quitIcon);
    gtk_container_add(GTK_CONTAINER(quitMi), align);

    GtkWidget *mainMenuLeftMenubar = gtk_menu_bar_new();
    setBackground(mainMenuLeftMenubar, "#fff");

    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenuLeftMenubar), titleMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenuLeftMenubar), shopMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainMenuLeftMenubar), addBookMi);

    GtkWidget *rightMenubar = gtk_menu_bar_new();
    setBackground(rightMenubar, "#fff");

    gtk_menu_shell_append(GTK_MENU_SHELL(rightMenubar), timeMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(rightMenubar), quitMi);

    GtkWidget *menubarHBox = gtk_hbox_new(FALSE, 0);
    setBackground(menubarHBox, "#fff");

    gtk_box_pack_start(GTK_BOX(menubarHBox), mainMenuLeftMenubar, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(menubarHBox), rightMenubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vBox), menubarHBox, FALSE, FALSE, 0);

    GtkWidget *currentBooksScrollbar = gtk_scrolled_window_new(NULL, NULL);
    setBackground(currentBooksScrollbar, "#fff");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(currentBooksScrollbar), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vBox), currentBooksScrollbar, TRUE, TRUE, 5);

    gtk_widget_set_app_paintable(currentBooksScrollbar, TRUE); 
    gtk_widget_set_double_buffered(currentBooksScrollbar, FALSE);

    GtkWidget *centerAlign = gtk_alignment_new(0.5, 0, 0.5, 0);
    GtkWidget *listBox = gtk_vbox_new(FALSE, 10);
    setBackground(listBox, "#fff");
    gtk_widget_set_app_paintable(listBox, TRUE); 
    gtk_widget_set_double_buffered(listBox, FALSE);

    gtk_container_add(GTK_CONTAINER(centerAlign), listBox);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(currentBooksScrollbar), centerAlign);

    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(currentBooksScrollbar));
    
    std::list<selectedBookData> selectedBooks = getSelectedBooks(LOCAL_DB_PATH);

    if (selectedBooks.empty()) {
        GtkWidget *emptyLabel = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(emptyLabel), setWhiteMarkup("No books in your reading list\n\nAdd books in the 'Add Book From Library' tab.", 12000).c_str());
        gtk_label_set_justify(GTK_LABEL(emptyLabel), GTK_JUSTIFY_CENTER);
        gtk_box_pack_start(GTK_BOX(listBox), emptyLabel, TRUE, FALSE, 50);
        
    } else {
        std::list<bookData> libraryData = retrieveLibraryData(KOBO_DB_PATH);

        std::map<std::string, bookData> libraryMap;
        for (auto &book : libraryData) {
            libraryMap[book.title] = book;
        }

        for (auto &selectedBook : selectedBooks) {
            float currentProgress = 0.0f;

            auto it = libraryMap.find(selectedBook.title);
            if (it != libraryMap.end()) {
                currentProgress = it->second.progress;
            }
            
            updateBookProgress(LOCAL_DB_PATH, selectedBook.title, currentProgress, currentDate);
            
            float pagesRemaining = selectedBook.totalPages * (1.0f - currentProgress);
            
            float requiredDailyPages = 0.0f;
            if (selectedBook.daysToFinish > 0) {
                requiredDailyPages = pagesRemaining / selectedBook.daysToFinish;
            }
            
            std::cout << "Displaying selected book: " << selectedBook.title << std::endl;
            
            GtkWidget *card = create_book_card(
                selectedBook.title,
                currentProgress,
                requiredDailyPages,
                pagesRemaining,
                selectedBook.daysToFinish,
                selectedBook.streak,
                vBox  
            );
            setBackground(card, "#fff");
            gtk_box_pack_start(GTK_BOX(listBox), card, FALSE, FALSE, 10);
        }
    }

    g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(addBookMi), "activate", G_CALLBACK(on_button_addBook), vBox);

    gtk_widget_show_all(vBox);
}