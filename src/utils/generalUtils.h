#pragma once
#include <gtk/gtk.h>
#include <string>

void clearWindow(GtkWidget *container);
std::string setMarkup(const std::string &text, int textSize);
std::string setWhiteMarkup(const std::string &text, int textSize);
void setBackground(GtkWidget *w, const char *hex);
std::string roundFloat(float value, int decimals);
std::string getCurrentTime();
std::string getCurrentDate();