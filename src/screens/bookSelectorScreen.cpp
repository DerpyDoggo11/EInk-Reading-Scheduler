#include "mainScreen.h"
#include "bookSelectorScreen.h"
#include "../utils/generalUtils.h"
#include "../utils/readDatabase.h"
#include "../utils/localDatabase.h"
#include "../icons/quit_icon.h"
#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>
#include <list> 
#include <string>
#include <map>

const std::string KOBO_DB_PATH = "./emulator/kobo/.kobo/KoboReader.sqlite";
const std::string LOCAL_DB_PATH = "./emulator/kobo/selected_books.db";

// actual kobo paths (uncomment for actual kobos)
// const std::string KOBO_DB_PATH = "/mnt/onboard/.kobo/KoboReader.sqlite";
// const std::string LOCAL_DB_PATH = "/mnt/onboard/.kobo/selected_books.db";

struct BookSelectorData {
    GtkWidget *combo;
    GtkWidget *spinButton;
    GtkWidget *vBox;
    GtkWidget *errorLabel;
    GtkWidget *confirmMi;
    std::map<std::string, float> *bookPagesMap;
};

void updateValidation(BookSelectorData *data) {
    gchar *selectedTitle = gtk_combo_box_get_active_text(GTK_COMBO_BOX(data->combo));
    int daysToFinish = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spinButton));
    
    bool isValid = true;
    std::string errorMsg = "";
    
    if (!selectedTitle || strlen(selectedTitle) == 0 || std::string(selectedTitle) == "-- select a book --") {
        isValid = false;
        errorMsg = "Select a book from the list";
    } else if (isBookSelected(LOCAL_DB_PATH, selectedTitle)) {
        isValid = false;
        errorMsg = "This book is already in your reading list";
    } else if (daysToFinish < 1) {
        isValid = false;
        errorMsg = "Days to finish must be at least 1";
    }
    
    if (!errorMsg.empty()) {
        std::string markup = "<span size=\"11000\" foreground=\"#ff3333\">" + errorMsg + "</span>";
        gtk_label_set_markup(GTK_LABEL(data->errorLabel), markup.c_str());
        gtk_widget_show(data->errorLabel);
    } else {
        gtk_widget_hide(data->errorLabel);
    }
    
    gtk_widget_set_sensitive(data->confirmMi, isValid);
    
    if (selectedTitle) g_free(selectedTitle);
}

void on_combo_changed(GtkWidget *combo, gpointer user_data) {
    updateValidation((BookSelectorData *)user_data);
}

void on_spin_changed(GtkWidget *spin, gpointer user_data) {
    updateValidation((BookSelectorData *)user_data);
}

void on_button_back(GtkWidget *menuItem, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    delete data->bookPagesMap;
    delete data;
    openMainScreen(GTK_WIDGET(((BookSelectorData *)user_data)->vBox));
}

void on_button_confirm(GtkWidget *menuItem, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    gchar *selectedTitle = gtk_combo_box_get_active_text(GTK_COMBO_BOX(data->combo));

    if (!selectedTitle || strlen(selectedTitle) == 0 || std::string(selectedTitle) == "-- Select a book --") {
        if (selectedTitle) g_free(selectedTitle);
        return;
    }

    int daysToFinish = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spinButton));

    if (daysToFinish < 1) {
        g_free(selectedTitle);
        return;
    }

    float totalPages = 100.0f;
    if (data->bookPagesMap && data->bookPagesMap->find(selectedTitle) != data->bookPagesMap->end()) {
        totalPages = (*data->bookPagesMap)[selectedTitle];
    }

    initLocalDatabase(LOCAL_DB_PATH);

    if (isBookSelected(LOCAL_DB_PATH, selectedTitle)) {
        g_free(selectedTitle);
        return;
    }

    if (addSelectedBook(LOCAL_DB_PATH, selectedTitle, daysToFinish, totalPages)) {
        delete data->bookPagesMap;
        GtkWidget *vBox = data->vBox;
        delete data;
        openMainScreen(vBox);
    } else {
        std::string markup = "<span size=\"11000\" foreground=\"#ff3333\">Failed to add book. Please try again.</span>";
        gtk_label_set_markup(GTK_LABEL(data->errorLabel), markup.c_str());
        gtk_widget_show(data->errorLabel);
    }

    g_free(selectedTitle);
}

void openBookSelectorScreen(GtkWidget *vBox){
    clearWindow(vBox);

    GtkWidget *screenVBox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vBox), screenVBox, TRUE, TRUE, 0);

    GtkWidget *backMi = gtk_menu_item_new();
    GtkWidget *backHbox = gtk_hbox_new(FALSE, 6);

    GtkWidget *backLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(backLabel), setMarkup("← Back", 10000).c_str());
    gtk_box_pack_start(GTK_BOX(backHbox), backLabel, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(backMi), backHbox);

    GtkWidget *menubarHBox = gtk_menu_bar_new();
    setBackground(menubarHBox, "#ffffff");
    gtk_menu_shell_append(GTK_MENU_SHELL(menubarHBox), backMi);
    gtk_box_pack_start(GTK_BOX(screenVBox), menubarHBox, FALSE, FALSE, 0);

    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titleLabel), setWhiteMarkup("Add Book to Reading List", 15000).c_str());
    gtk_box_pack_start(GTK_BOX(screenVBox), titleLabel, FALSE, FALSE, 10);

    GtkWidget *contentVBox = gtk_vbox_new(FALSE, 15);
    gtk_box_pack_start(GTK_BOX(screenVBox), contentVBox, TRUE, FALSE, 20);

    GtkWidget *bookLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(bookLabel), setWhiteMarkup("Select a book from your library:", 12000).c_str());
    gtk_misc_set_alignment(GTK_MISC(bookLabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(contentVBox), bookLabel, FALSE, FALSE, 5);

    GtkWidget *bookCombo = gtk_combo_box_new_text();

    std::list<bookData> libraryBooks = retrieveLibraryData(KOBO_DB_PATH);
    
    std::map<std::string, float> *bookPagesMap = new std::map<std::string, float>();

    if (libraryBooks.empty()) {
        gtk_combo_box_append_text(GTK_COMBO_BOX(bookCombo), "No books found in library");
    } else {
        gtk_combo_box_append_text(GTK_COMBO_BOX(bookCombo), "-- Select a book --");

        for (auto &book : libraryBooks) {
            if (!book.title.empty()) {
                gtk_combo_box_append_text(GTK_COMBO_BOX(bookCombo), book.title.c_str());
                float estimatedTotalPages = 300.0f;
                (*bookPagesMap)[book.title] = estimatedTotalPages;
            }
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(bookCombo), 0);
    }

    gtk_box_pack_start(GTK_BOX(contentVBox), bookCombo, FALSE, FALSE, 5);

    GtkWidget *finishLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(finishLabel), setWhiteMarkup("How fast do you want to finish? (in days)", 12000).c_str());
    gtk_misc_set_alignment(GTK_MISC(finishLabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(contentVBox), finishLabel, FALSE, FALSE, 5);

    GtkAdjustment *daysAdjustment = GTK_ADJUSTMENT(gtk_adjustment_new(7, 1, 365, 1, 10, 0));
    GtkWidget *daysSpin = gtk_spin_button_new(GTK_ADJUSTMENT(daysAdjustment), 1, 0);
    gtk_box_pack_start(GTK_BOX(contentVBox), daysSpin, FALSE, FALSE, 5);

    GtkWidget *errorLabel = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(contentVBox), errorLabel, FALSE, FALSE, 10);
    gtk_widget_set_no_show_all(errorLabel, TRUE); 

    GtkWidget *confirmMi = gtk_menu_item_new(); 
    GtkWidget *confirmLabel = gtk_label_new(NULL); 
    gtk_label_set_markup(GTK_LABEL(confirmLabel), setWhiteMarkup("Add Book to Reading List", 12000).c_str());
    gtk_container_add(GTK_CONTAINER(confirmMi), confirmLabel); 

    GtkWidget *confirmMenubar = gtk_menu_bar_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(confirmMenubar), confirmMi); 
    gtk_box_pack_start(GTK_BOX(contentVBox), confirmMenubar, FALSE, FALSE, 10); 

    BookSelectorData *callbackData = new BookSelectorData;
    callbackData->combo = bookCombo;
    callbackData->spinButton = daysSpin;
    callbackData->vBox = vBox;
    callbackData->bookPagesMap = bookPagesMap;
    callbackData->errorLabel = errorLabel;
    callbackData->confirmMi = confirmMi;

    g_signal_connect(G_OBJECT(bookCombo), "changed", G_CALLBACK(on_combo_changed), callbackData);
    g_signal_connect(G_OBJECT(daysSpin), "value-changed", G_CALLBACK(on_spin_changed), callbackData);
    
    g_signal_connect(G_OBJECT(backMi), "activate", G_CALLBACK(on_button_back), callbackData);
    g_signal_connect(G_OBJECT(confirmMi), "activate", G_CALLBACK(on_button_confirm), callbackData);

    updateValidation(callbackData);

    gtk_widget_show_all(vBox);
}