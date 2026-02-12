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
    GtkWidget *vBox;
    GtkWidget *errorLabel;
    GtkWidget *confirmMi;
    GtkWidget *daysLabel;
    std::map<std::string, float> *bookPagesMap;
    std::string selectedBookTitle;
    int daysToFinish;
};

void updateDaysLabel(BookSelectorData *data) {
    std::string daysText = "Days to finish: " + std::to_string(data->daysToFinish);
    gtk_label_set_markup(GTK_LABEL(data->daysLabel),setWhiteMarkup(daysText, 13000).c_str());
}

void updateValidation(BookSelectorData *data) {
    bool isValid = true;
    std::string errorMsg = "";
    
    if (data->selectedBookTitle.empty()) {
        isValid = false;
        errorMsg = "Select a book from the list";
    } else if (isBookSelected(LOCAL_DB_PATH, data->selectedBookTitle)) {
        isValid = false;
        errorMsg = "This book is already in your reading list";
    } else if (data->daysToFinish < 1) {
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
}

void on_days_decrease(GtkWidget *button, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    if (data->daysToFinish > 1) {
        data->daysToFinish = data->daysToFinish - 1;
        updateDaysLabel(data);
        updateValidation(data);
    }
}

void on_days_big_decrease(GtkWidget *button, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    if ((data->daysToFinish - 9) > 1) {
        data->daysToFinish = data->daysToFinish - 10;
        updateDaysLabel(data);
        updateValidation(data);
    }
}


void on_days_increase(GtkWidget *button, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    if (data->daysToFinish < 365) {
        data->daysToFinish = data->daysToFinish + 1;
        updateDaysLabel(data);
        updateValidation(data);
    }
}

void on_days_big_increase(GtkWidget *button, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    if ((data->daysToFinish + 10) < 365) {
        data->daysToFinish = data->daysToFinish + 10;
        updateDaysLabel(data);
        updateValidation(data);
    }
}

void on_book_item_clicked(GtkWidget *button, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    const gchar *title = gtk_button_get_label(GTK_BUTTON(button));

    data->selectedBookTitle = std::string(title);
    updateValidation(data);

    std::cout << "Selected book: " << data->selectedBookTitle << std::endl;
}

void on_button_back(GtkWidget *menuItem, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;
    GtkWidget *vBox = data->vBox;
    delete data->bookPagesMap;
    delete data;
    openMainScreen(vBox);
}

void on_button_confirm(GtkWidget *menuItem, gpointer user_data) {
    BookSelectorData *data = (BookSelectorData *)user_data;

    if (data->selectedBookTitle.empty()) {
        return;
    }

    if (data->daysToFinish < 1) {
        return;
    }

    float totalPages = 100.0f;
    if (data->bookPagesMap && data->bookPagesMap->find(data->selectedBookTitle) != data->bookPagesMap->end()) {
        totalPages = (*data->bookPagesMap)[data->selectedBookTitle];
    }

    initLocalDatabase(LOCAL_DB_PATH);

    if (isBookSelected(LOCAL_DB_PATH, data->selectedBookTitle)) {
        return;
    }

    if (addSelectedBook(LOCAL_DB_PATH, data->selectedBookTitle, data->daysToFinish, totalPages)) {
        delete data->bookPagesMap;
        GtkWidget *vBox = data->vBox;
        delete data;
        openMainScreen(vBox);
    } else {
        std::string markup = "<span size=\"11000\" foreground=\"#ff3333\">Failed to add book. Please try again.</span>";
        gtk_label_set_markup(GTK_LABEL(data->errorLabel), markup.c_str());
        gtk_widget_show(data->errorLabel);
    }
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

    GtkWidget *bookLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(bookLabel), setWhiteMarkup("Select a book from your library:", 12000).c_str());
    //gtk_misc_set_alignment(GTK_MISC(bookLabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(screenVBox), bookLabel, FALSE, FALSE, 5);

    GtkWidget *booksScrollbar = gtk_scrolled_window_new(NULL, NULL);
    setBackground(booksScrollbar, "#fff");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(booksScrollbar), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(screenVBox), booksScrollbar, TRUE, TRUE, 5);

    gtk_widget_set_app_paintable(booksScrollbar, TRUE);
    gtk_widget_set_double_buffered(booksScrollbar, FALSE);

    //GtkWidget *centerAlign = gtk_alignment_new(0.5, 0, 0.95, 0);
    GtkWidget *listBox = gtk_vbox_new(FALSE, 5);
    setBackground(listBox, "#fff");
    gtk_widget_set_app_paintable(listBox, TRUE);
    gtk_widget_set_double_buffered(listBox, FALSE);

    //gtk_container_add(GTK_CONTAINER(centerAlign), listBox);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(booksScrollbar), listBox);

    std::list<bookData> libraryBooks = retrieveLibraryData(KOBO_DB_PATH);
    
    std::map<std::string, float> *bookPagesMap = new std::map<std::string, float>();

    BookSelectorData *callbackData = new BookSelectorData;
    callbackData->vBox = vBox;
    callbackData->bookPagesMap = bookPagesMap;
    callbackData->daysToFinish = 7; 
    callbackData->selectedBookTitle = ""; 

    if (libraryBooks.empty()) {
        GtkWidget *emptyLabel = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(emptyLabel), setWhiteMarkup("No books found in library", 12000).c_str());
        gtk_box_pack_start(GTK_BOX(listBox), emptyLabel, TRUE, FALSE, 50);
    } else {
        for (auto &book : libraryBooks) {
            if (!book.title.empty()) {

                GtkWidget *bookButton = gtk_button_new_with_label(book.title.c_str());
                gtk_button_set_relief(GTK_BUTTON(bookButton), GTK_RELIEF_NORMAL);
                gtk_widget_set_size_request(bookButton, -1, 50);

                float estimatedTotalPages = 300.0f;
                (*bookPagesMap)[book.title] = estimatedTotalPages;

                g_signal_connect(G_OBJECT(bookButton), "clicked", G_CALLBACK(on_book_item_clicked), callbackData);

                gtk_box_pack_start(GTK_BOX(listBox), bookButton, FALSE, FALSE, 2);
            }
        }
    }


    GtkWidget *errorLabel = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(screenVBox), errorLabel, FALSE, FALSE, 10);
    gtk_widget_set_no_show_all(errorLabel, TRUE); 
    callbackData->errorLabel = errorLabel;

    GtkWidget *daysSection = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(screenVBox), daysSection, FALSE, FALSE, 5);
    
    GtkWidget *finishLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(finishLabel), setWhiteMarkup("How fast do you want to finish? (in days)", 12000).c_str());
    gtk_misc_set_alignment(GTK_MISC(finishLabel), 0.5, 0.5);
    gtk_box_pack_start(GTK_BOX(daysSection), finishLabel, FALSE, FALSE, 5);

    GtkWidget *daysControlAlign = gtk_alignment_new(0.5, 0.5, 0, 0);
    GtkWidget *daysControlBox = gtk_hbox_new(FALSE, 15);
    gtk_container_add(GTK_CONTAINER(daysControlAlign), daysControlBox);
    gtk_box_pack_start(GTK_BOX(daysSection), daysControlAlign, FALSE, FALSE, 0);

    GtkWidget *bigDecreaseButton = gtk_button_new_with_label("--");
    gtk_widget_set_size_request(bigDecreaseButton, 60, 60);
    gtk_box_pack_start(GTK_BOX(daysControlBox), bigDecreaseButton, FALSE, FALSE, 0);

    GtkWidget *decreaseButton = gtk_button_new_with_label("-");
    gtk_widget_set_size_request(decreaseButton, 60, 60);
    gtk_box_pack_start(GTK_BOX(daysControlBox), decreaseButton, FALSE, FALSE, 0);

    GtkWidget *daysLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(daysLabel), setWhiteMarkup("Days to finish: 7", 13000).c_str());
    gtk_box_pack_start(GTK_BOX(daysControlBox), daysLabel, FALSE, FALSE, 20);
    callbackData->daysLabel = daysLabel;

    GtkWidget *increaseButton = gtk_button_new_with_label("+");
    gtk_widget_set_size_request(increaseButton, 60, 60);
    gtk_box_pack_start(GTK_BOX(daysControlBox), increaseButton, FALSE, FALSE, 0);

    GtkWidget *bigIncreaseButton = gtk_button_new_with_label("++");
    gtk_widget_set_size_request(bigIncreaseButton, 60, 60);
    gtk_box_pack_start(GTK_BOX(daysControlBox), bigIncreaseButton, FALSE, FALSE, 0);

    GtkWidget *confirmAlign = gtk_alignment_new(0.5, 0.5, 0, 0);
    GtkWidget *confirmMi = gtk_button_new();
    GtkWidget *confirmLabel = gtk_label_new(NULL); 
    gtk_label_set_markup(GTK_LABEL(confirmLabel), setWhiteMarkup("Add Book to Reading List", 13000).c_str());
    gtk_container_add(GTK_CONTAINER(confirmMi), confirmLabel);
    gtk_widget_set_size_request(confirmMi, 400, 60);
    
    gtk_container_add(GTK_CONTAINER(confirmAlign), confirmMi);
    gtk_box_pack_start(GTK_BOX(screenVBox), confirmAlign, FALSE, FALSE, 10);
    callbackData->confirmMi = confirmMi;

    g_signal_connect(G_OBJECT(decreaseButton), "clicked", G_CALLBACK(on_days_decrease), callbackData);
    g_signal_connect(G_OBJECT(bigDecreaseButton), "clicked", G_CALLBACK(on_days_big_decrease), callbackData);
    g_signal_connect(G_OBJECT(increaseButton), "clicked", G_CALLBACK(on_days_increase), callbackData);
    g_signal_connect(G_OBJECT(bigIncreaseButton), "clicked", G_CALLBACK(on_days_big_increase), callbackData);
    g_signal_connect(G_OBJECT(backMi), "activate", G_CALLBACK(on_button_back), callbackData);
    g_signal_connect(G_OBJECT(confirmMi), "clicked", G_CALLBACK(on_button_confirm), callbackData);

    updateValidation(callbackData);

    gtk_widget_show_all(vBox);
}