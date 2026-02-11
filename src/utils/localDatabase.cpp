#include "localDatabase.h"
#include "generalUtils.h"
#include <sqlite3.h>
#include <iostream>
#include <sys/stat.h>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>

bool createDirectoryRecursive(const std::string &path) {
    std::string dirPath = path;

    while (!dirPath.empty() && (dirPath.back() == '/' || dirPath.back() == '\\')) {
        dirPath.pop_back();
    }

    struct stat info;
    if (stat(dirPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) {
        return true;
    }

    size_t lastSlash = dirPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string parentDir = dirPath.substr(0, lastSlash);
        if (!createDirectoryRecursive(parentDir)) {
            return false;
        }
    }

    #ifdef _WIN32
        return mkdir(dirPath.c_str()) == 0 || errno == EEXIST;
    #else
        return mkdir(dirPath.c_str(), 0755) == 0 || errno == EEXIST;
    #endif
}

int daysBetweenDates(const std::string &date1, const std::string &date2) {
    struct tm tm1 = {0}, tm2 = {0};
    
    // format is year/month/day
    sscanf(date1.c_str(), "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday);
    sscanf(date2.c_str(), "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday);
    
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;
    
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    
    return (int)difftime(t2, t1) / (60 * 60 * 24);
}

bool initLocalDatabase(const std::string &databasePath) {

    size_t lastSlash = databasePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string dirPath = databasePath.substr(0, lastSlash);

        if (!createDirectoryRecursive(dirPath)) {
            return false;
        }
    }
    sqlite3 *db = nullptr;

    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        if (db) {
            sqlite3_close(db);
        }
        return false;
    }

    const char *sql = 
        "CREATE TABLE IF NOT EXISTS selected_books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL UNIQUE,"
        "days_to_finish INTEGER NOT NULL,"
        "last_progress REAL DEFAULT 0.0,"
        "last_open_date TEXT DEFAULT '',"
        "streak INTEGER DEFAULT 0,"
        "total_pages REAL DEFAULT 0.0,"
        "date_added DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char *errorMessage = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        sqlite3_free(errorMessage);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

bool addSelectedBook(const std::string &databasePath, const std::string &title, int daysToFinish, float totalPages) {
    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        return false;
    }

    const char *sql = "INSERT INTO selected_books (title, days_to_finish, total_pages, last_open_date) VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    std::string currentDate = getCurrentDate();
    
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, daysToFinish);
    sqlite3_bind_double(stmt, 3, totalPages);
    sqlite3_bind_text(stmt, 4, currentDate.c_str(), -1, SQLITE_TRANSIENT);

    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return success;
}

std::list<selectedBookData> getSelectedBooks(const std::string &databasePath) {
    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr;
    std::list<selectedBookData> selectedBooks;

    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        return selectedBooks;
    }

    const char *sql = "SELECT title, days_to_finish, last_progress, last_open_date, streak, total_pages FROM selected_books ORDER BY date_added DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return selectedBooks;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        selectedBookData book;
        const unsigned char *titleText = sqlite3_column_text(stmt, 0);
        const unsigned char *dateText = sqlite3_column_text(stmt, 3);

        if (titleText) {
            book.title = reinterpret_cast<const char *>(titleText);
        } else {
            book.title = "";
        }

        book.daysToFinish = sqlite3_column_int(stmt, 1);
        book.lastProgress = sqlite3_column_double(stmt, 2);
        
        if (dateText) {
            book.lastOpenDate = reinterpret_cast<const char *>(dateText);
        } else {
            book.lastOpenDate = "";
        }
        
        book.streak = sqlite3_column_int(stmt, 4);
        book.totalPages = sqlite3_column_double(stmt, 5);

        selectedBooks.push_back(book);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return selectedBooks;
}

bool isBookSelected(const std::string &databasePath, const std::string &title) {
    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr;
    
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    const char *sql = "SELECT COUNT(*) FROM selected_books WHERE title = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = (sqlite3_column_int(stmt, 0) > 0);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return exists;
}

bool removeSelectedBook(const std::string &databasePath, const std::string &title) {
    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr;
    
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    const char *sql = "DELETE FROM selected_books WHERE title = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return success;
}

bool updateBookProgress(const std::string &databasePath, const std::string &title, float currentProgress, const std::string &currentDate) {
    sqlite3 *db = nullptr;
    sqlite3_stmt *selectStmt = nullptr;
    sqlite3_stmt *updateStmt = nullptr;
    
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    // Get current book data
    const char *selectSql = "SELECT last_progress, last_open_date, streak FROM selected_books WHERE title = ?;";
    
    if (sqlite3_prepare_v2(db, selectSql, -1, &selectStmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_bind_text(selectStmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    
    float lastProgress = 0.0;
    std::string lastOpenDate = "";
    int currentStreak = 0;
    
    if (sqlite3_step(selectStmt) == SQLITE_ROW) {
        lastProgress = sqlite3_column_double(selectStmt, 0);
        const unsigned char *dateText = sqlite3_column_text(selectStmt, 1);
        if (dateText) {
            lastOpenDate = reinterpret_cast<const char *>(dateText);
        }
        currentStreak = sqlite3_column_int(selectStmt, 2);
    }
    
    sqlite3_finalize(selectStmt);
    
    int newStreak = currentStreak;
    
    if (!lastOpenDate.empty() && lastOpenDate != currentDate) {
        int daysDiff = daysBetweenDates(lastOpenDate, currentDate);
        
        if (daysDiff == 1 && currentProgress > lastProgress) {
            newStreak++;
        }

        else if (daysDiff > 1) {
            newStreak = (currentProgress > lastProgress) ? 1 : 0;
        }
    } else if (currentProgress > lastProgress && currentStreak == 0) {
        newStreak = 1;
    }
    
    const char *updateSql = "UPDATE selected_books SET last_progress = ?, last_open_date = ?, streak = ? WHERE title = ?;";
    
    if (sqlite3_prepare_v2(db, updateSql, -1, &updateStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare update statement\n";
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_bind_double(updateStmt, 1, currentProgress);
    sqlite3_bind_text(updateStmt, 2, currentDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(updateStmt, 3, newStreak);
    sqlite3_bind_text(updateStmt, 4, title.c_str(), -1, SQLITE_TRANSIENT);
    
    bool success = (sqlite3_step(updateStmt) == SQLITE_DONE);
    
    sqlite3_finalize(updateStmt);
    sqlite3_close(db);
    
    std::cout << "Updated " << title << ": progress=" << currentProgress 
              << ", streak=" << newStreak << "\n";
    
    return success;
}