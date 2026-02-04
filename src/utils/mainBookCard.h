#pragma once
#include <gtk/gtk.h>

GtkWidget* create_book_card(const char* title, float currentProgress, float totalProgress, float streak);
