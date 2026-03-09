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
    std::string dateAdded;
};

struct xpData {
    int xp;
    int level;
};

bool initLocalDatabase(const std::string &databasePath);
bool addSelectedBook(const std::string &databasePath, const std::string &title, int daysToFinish, float totalPages);
std::list<selectedBookData> getSelectedBooks(const std::string &databasePath);
bool isBookSelected(const std::string &databasePath, const std::string &title);
bool removeSelectedBook(const std::string &databasePath, const std::string &title);
bool updateBookProgress(const std::string &databasePath, const std::string &title, float currentProgress, const std::string &currentDate);
int daysBetweenDates(const std::string &date1, const std::string &date2);
xpData getXpData(const std::string &databasePath);
bool addXp(const std::string &databasePath, int amount);
bool markBookCompleted(const std::string &databasePath, const std::string &title);
bool isBookCompleted(const std::string &databasePath, const std::string &title);

#endif