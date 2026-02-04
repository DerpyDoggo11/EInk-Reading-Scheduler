#include "bookInputFieldScreen.h"

#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>
#include <list> 
#include <string>

GtkWidget *backButtonLeftMenubar;
GtkWidget *addBooksValueFields;


void hideBookSelectorScreen() {

}

GtkWidget* createBookInputFieldScreen(GtkWidget *window) {

    GtkWidget *screenVBox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), screenVBox);


    backButtonLeftMenubar = gtk_menu_bar_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(backButtonLeftMenubar), backMi);
    gtk_widget_hide(backButtonLeftMenubar);

    // Add Book Fields
    addBooksValueFields = gtk_vbox_new(FALSE, 10);

    GtkWidget *finishLabel = gtk_label_new("How fast do you want to finish? (in days)");
    gtk_box_pack_start(GTK_BOX(addBooksValueFields), finishLabel, FALSE, FALSE, 5);
    GtkAdjustment *daysAdjustment = GTK_ADJUSTMENT(gtk_adjustment_new(1, 1, 365, 1, 10, 0));

    GtkWidget *daysSpin = gtk_spin_button_new(GTK_ADJUSTMENT(daysAdjustment), 1, 0);
    gtk_box_pack_start(GTK_BOX(addBooksValueFields), daysSpin, FALSE, FALSE, 5);



    GtkWidget *confirmMi = gtk_menu_item_new();
    GtkWidget *confirmLabel = gtk_label_new("Confirm");
    gtk_container_add(GTK_CONTAINER(confirmMi), confirmLabel);
    GtkWidget *confirmMenubar = gtk_menu_bar_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(confirmMenubar), confirmMi);  
    gtk_box_pack_start(GTK_BOX(addBooksValueFields), confirmMenubar, FALSE, FALSE, 20);

    gtk_box_pack_start(GTK_BOX(screenVBox), addBooksValueFields, FALSE, FALSE, 0);

    return 

}