#include "mainBookCard.h" 
#include "../screens/mainScreen.h"
#include "../utils/generalUtils.h"
#include "../utils/localDatabase.h"
#include "../icons/streak_icon.h" 
#include "../icons/quit_icon.h"
#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>
#include <list> 
#include <string>
#include <sstream>
#include <iomanip>

const std::string KOBO_DB_PATH = "./emulator/kobo/.kobo/KoboReader.sqlite";
const std::string LOCAL_DB_PATH = "./emulator/kobo/selected_books.db";

struct RemoveConfirmData {
    std::string title;
    GtkWidget *vBox;
};

void on_button_cancel_remove(GtkWidget *menuItem, gpointer user_data) {
    RemoveConfirmData *data = (RemoveConfirmData *)user_data;
    GtkWidget *vBox = data->vBox;
    delete data;
    openMainScreen(vBox);
}

void on_button_confirm_remove(GtkWidget *menuItem, gpointer user_data) {
    RemoveConfirmData *data = (RemoveConfirmData *)user_data;
    GtkWidget *vBox = data->vBox;
    delete data;
    openMainScreen(vBox);
}

void showRemoveConfirmationScreen(const std::string &title, GtkWidget *vBox) {
    clearWindow(vBox);
    setBackground(vBox, "#ffffff");
    
    GtkWidget *contentVBox = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(vBox), contentVBox, TRUE, TRUE, 40);
    
    GtkWidget *warningLabel = gtk_label_new(NULL);
    std::string warningText = "Are you sure you want to remove this book from your reading list?\n This will erase your current streak.";
    gtk_label_set_markup(GTK_LABEL(warningLabel), setMarkup(warningText, 14000).c_str());
    gtk_label_set_line_wrap(GTK_LABEL(warningLabel), TRUE);
    gtk_label_set_justify(GTK_LABEL(warningLabel), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(contentVBox), warningLabel, FALSE, FALSE, 10);
    
    GtkWidget *titleLabel = gtk_label_new(NULL);
    std::string titleMarkup = "<span size=\"16000\" weight=\"bold\">" + title + "</span>";
    gtk_label_set_markup(GTK_LABEL(titleLabel), titleMarkup.c_str());
    gtk_label_set_line_wrap(GTK_LABEL(titleLabel), TRUE);
    gtk_label_set_justify(GTK_LABEL(titleLabel), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(contentVBox), titleLabel, FALSE, FALSE, 5);
    
    GtkWidget *infoLabel = gtk_label_new(NULL);
    std::string infoText = "Your reading progress and streak for this book will be lost.";
    gtk_label_set_markup(GTK_LABEL(infoLabel), setMarkup(infoText, 11000).c_str());
    gtk_label_set_line_wrap(GTK_LABEL(infoLabel), TRUE);
    gtk_label_set_justify(GTK_LABEL(infoLabel), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(contentVBox), infoLabel, FALSE, FALSE, 10);
    
    GtkWidget *buttonBox = gtk_hbox_new(TRUE, 20);
    gtk_box_pack_start(GTK_BOX(contentVBox), buttonBox, FALSE, FALSE, 20);
    
    GtkWidget *cancelMi = gtk_menu_item_new();
    GtkWidget *cancelLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(cancelLabel), setMarkup("Cancel", 13000).c_str());
    gtk_container_add(GTK_CONTAINER(cancelMi), cancelLabel);
    
    GtkWidget *cancelMenubar = gtk_menu_bar_new();
    setBackground(cancelMenubar, "#ffffff");
    gtk_menu_shell_append(GTK_MENU_SHELL(cancelMenubar), cancelMi);
    gtk_box_pack_start(GTK_BOX(buttonBox), cancelMenubar, TRUE, TRUE, 0);
    
    GtkWidget *confirmMi = gtk_menu_item_new();
    GtkWidget *confirmLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(confirmLabel), "<span size=\"13000\" foreground=\"#ff3333\">Remove Book</span>");
    gtk_container_add(GTK_CONTAINER(confirmMi), confirmLabel);
    
    GtkWidget *confirmMenubar = gtk_menu_bar_new();
    setBackground(confirmMenubar, "#ffffff");
    gtk_menu_shell_append(GTK_MENU_SHELL(confirmMenubar), confirmMi);
    gtk_box_pack_start(GTK_BOX(buttonBox), confirmMenubar, TRUE, TRUE, 0);
    
    RemoveConfirmData *callbackData = new RemoveConfirmData;
    callbackData->title = title;
    callbackData->vBox = vBox;
    
    g_signal_connect(G_OBJECT(cancelMi), "activate", G_CALLBACK(on_button_cancel_remove), callbackData);
    g_signal_connect(G_OBJECT(confirmMi), "activate", G_CALLBACK(on_button_confirm_remove), callbackData);
    
    gtk_widget_show_all(vBox);
}

void on_button_remove_book(GtkWidget *button, gpointer user_data) {
    RemoveConfirmData *data = (RemoveConfirmData *)user_data;
    showRemoveConfirmationScreen(data->title, data->vBox);
}

GtkWidget* create_book_card(const std::string title, float currentProgress, float requiredDailyPages, float totalPagesRemaining, int daysUntilCompletion, int streak, GtkWidget *vBox){
    GtkWidget *outerBox = gtk_event_box_new(); 
    setBackground(outerBox, "#000"); 
    
    GtkWidget *innerBox = gtk_event_box_new();
    setBackground(innerBox, "#fff");  
    gtk_container_set_border_width(GTK_CONTAINER(innerBox), 20);
    gtk_container_add(GTK_CONTAINER(outerBox), innerBox);
    
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
    
    GtkWidget *mainBox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), mainBox);
    
    GtkWidget *topBar = gtk_hbox_new(FALSE, 10);
    setBackground(topBar, "#fff");  
    gtk_box_pack_start(GTK_BOX(mainBox), topBar, FALSE, FALSE, 0);

    GtkWidget *titleMi = gtk_menu_item_new();
    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titleLabel), setMarkup(title, 15000).c_str());

    gtk_container_add(GTK_CONTAINER(titleMi), titleLabel);
    gtk_widget_set_sensitive(titleMi, FALSE);
    gtk_widget_set_can_focus(titleMi, FALSE);

    gtk_misc_set_alignment(GTK_MISC(titleLabel), 0.5, 0.5);


    GtkWidget *deleteMi = gtk_menu_item_new();


    GdkPixbuf *quitPixbuf = gdk_pixbuf_new_from_inline(quit_png_len, quit_png, FALSE, NULL);
    GdkPixbuf *quitScaled = gdk_pixbuf_scale_simple(quitPixbuf, 16, 16, GDK_INTERP_BILINEAR);
    GtkWidget *quitIcon = gtk_image_new_from_pixbuf(quitScaled);
    g_object_unref(quitPixbuf);

    gtk_container_add(GTK_CONTAINER(deleteMi), quitIcon);

    GtkWidget *leftMenubar = gtk_menu_bar_new();
    setBackground(leftMenubar, "#fff");  
    gtk_menu_shell_append(GTK_MENU_SHELL(leftMenubar), titleMi);

    GtkWidget *rightMenubar = gtk_menu_bar_new();
    setBackground(rightMenubar, "#fff"); 
    gtk_menu_shell_append(GTK_MENU_SHELL(rightMenubar), deleteMi);

    gtk_box_pack_start(GTK_BOX(topBar), leftMenubar, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(topBar), rightMenubar, FALSE, FALSE, 0);
    
    
    GtkWidget *box = gtk_vbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(mainBox), box, FALSE, FALSE, 0);
    
    GtkWidget *spacer = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(box), spacer, TRUE, TRUE, 0);
    gtk_widget_set_size_request(spacer, -1, 4);
    
    std::stringstream progressStream;
    progressStream << std::fixed << std::setprecision(1) << (currentProgress * 100.0f) << "% complete";
    std::string currentProgressText = progressStream.str();
    
    GtkWidget *currentProgressLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(currentProgressLabel), setMarkup(currentProgressText, 10000).c_str());
    gtk_box_pack_start(GTK_BOX(box), currentProgressLabel, FALSE, FALSE, 0);
    
    std::stringstream requiredStream;
    requiredStream << std::fixed << std::setprecision(1) << requiredDailyPages;
    std::string requiredText = requiredStream.str() + (requiredDailyPages == 1.0f ? " page required today" : " pages required today");
    
    GtkWidget *requiredProgressLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(requiredProgressLabel), setMarkup(requiredText, 10000).c_str());
    gtk_box_pack_start(GTK_BOX(box), requiredProgressLabel, FALSE, FALSE, 0);
    
    std::stringstream totalStream;
    totalStream << std::fixed << std::setprecision(0) << totalPagesRemaining;
    std::string totalText = totalStream.str() + (totalPagesRemaining == 1.0f ? " page until completion" : " pages until completion");
    
    GtkWidget *totalProgressLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(totalProgressLabel), setMarkup(totalText, 10000).c_str());
    gtk_box_pack_start(GTK_BOX(box), totalProgressLabel, FALSE, FALSE, 0);
    
    std::string daysText = std::to_string(daysUntilCompletion) + (daysUntilCompletion == 1 ? " day until completion" : " days until completion");
    
    GtkWidget *daysLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(daysLabel), setMarkup(daysText, 10000).c_str());
    gtk_box_pack_start(GTK_BOX(box), daysLabel, FALSE, FALSE, 0);
    
    GtkWidget *streakBox = gtk_hbox_new(FALSE, 5);
    
    GdkPixbuf *streakPixbuf = gdk_pixbuf_new_from_inline(streak_png_len, streak_png, FALSE, NULL); 
    if (streakPixbuf) { 
        GtkWidget *streakIcon = gtk_image_new_from_pixbuf(streakPixbuf); 
        gtk_box_pack_start(GTK_BOX(streakBox), streakIcon, FALSE, FALSE, 0); 
        g_object_unref(streakPixbuf); 
    }
    
    std::string streakText = std::to_string(streak) + (streak == 1 ? " day streak" : " days streak");
    GtkWidget *streakLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(streakLabel), setMarkup(streakText, 10000).c_str());
    gtk_box_pack_start(GTK_BOX(streakBox), streakLabel, FALSE, FALSE, 0); 
    
    GtkWidget *align = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(align), streakBox);
    gtk_box_pack_start(GTK_BOX(box), align, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(innerBox), frame);
    
    RemoveConfirmData *removeData = new RemoveConfirmData;
    removeData->title = title;
    removeData->vBox = vBox;
    
    g_signal_connect(G_OBJECT(deleteMi), "activate", G_CALLBACK(on_button_remove_book), removeData);
    
    return outerBox;
}