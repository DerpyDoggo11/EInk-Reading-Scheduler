#include <gtk-2.0/gtk/gtk.h>
#include <string>
#include <sstream>
#include <iomanip>

void clearWindow(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *l = children; l != NULL; l = l->next) {
        gtk_widget_destroy(GTK_WIDGET(l->data));
    };
    g_list_free(children);
};

std::string setMarkup(const std::string &text, int textSize) {
    std::string titleString = "<span size=\"" + std::to_string(textSize) + "\" foreground=\"black\">" + std::string(text) + "</span>";
    return titleString;
}

std::string setWhiteMarkup(const std::string &text, int textSize) {
    std::string titleString = "<span size=\"" + std::to_string(textSize) + "\" foreground=\"white\">" + std::string(text) + "</span>";
    return titleString;
}


void setBackground(GtkWidget *w, const char *hex) { 
    GdkColor c; 
    gdk_color_parse(hex, &c); 
    gtk_widget_modify_bg(w, GTK_STATE_NORMAL, &c); 
} 

std::string roundFloat(float value, int decimals) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(decimals) << value;
    return ss.str();
}

std::string getCurrentTime() {
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    
    std::stringstream ss;
    ss << std::setfill('0') 
       << std::setw(2) << ltm->tm_hour << ":"
       << std::setw(2) << ltm->tm_min;
    
    return ss.str();
}

std::string getCurrentDate() {
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    
    std::stringstream ss;
    ss << std::setfill('0') 
       << std::setw(4) << (1900 + ltm->tm_year) << "-"
       << std::setw(2) << (1 + ltm->tm_mon) << "-"
       << std::setw(2) << ltm->tm_mday;
    
    return ss.str();
}