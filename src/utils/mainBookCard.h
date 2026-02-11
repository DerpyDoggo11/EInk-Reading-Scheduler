#ifndef MAIN_BOOK_CARD_H
#define MAIN_BOOK_CARD_H

#include <gtk-2.0/gtk/gtk.h>
#include <string>

GtkWidget* create_book_card(const std::string title, float currentProgress, float requiredDailyPages, float totalPagesRemaining, int daysUntilCompletion, int streak, GtkWidget *vBox);

#endif