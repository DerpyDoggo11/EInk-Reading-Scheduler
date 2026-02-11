#ifndef LOCAL_DATABASE_H
#define LOCAL_DATABASE_H

#include <string>
#include <list>

struct selectedBookData {
    std::string title;
    int daysToFinish;
    float lastProgress;
    std::string lastOpenDate;
    int streak;
    float totalPages;
};

bool initLocalDatabase(const std::string &databasePath);
bool addSelectedBook(const std::string &databasePath, const std::string &title, int daysToFinish, float totalPages);
std::list<selectedBookData> getSelectedBooks(const std::string &databasePath);
bool isBookSelected(const std::string &databasePath, const std::string &title);
bool removeSelectedBook(const std::string &databasePath, const std::string &title);
bool updateBookProgress(const std::string &databasePath, const std::string &title, float currentProgress, const std::string &currentDate);

#endif