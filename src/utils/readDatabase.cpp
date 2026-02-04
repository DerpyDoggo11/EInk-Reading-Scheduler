#include "readDatabase.h" 
#include <gtk-2.0/gtk/gtk.h>
#include <sqlite3.h> 
#include <iostream>

std::list<bookData> retrieveLibraryData(const std::string &databasePath)
{
  sqlite3 *db = nullptr;
  sqlite3_stmt *stmt = nullptr;

  std::list<bookData> userData;

  if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
  {
    g_print("Failed to open Kobo database\n");
    return userData;
  }

  // join content and content settings database, then filter the values
  const char *sql =
      "SELECT c.Title, c.ContentID, cs.Progress "
      "FROM content c "
      "LEFT JOIN content_settings cs ON c.ContentID = cs.ContentID "
      "WHERE c.ContentType = 6 "
      "ORDER BY c.Title COLLATE NOCASE;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
  {
    g_print("Failed to prepare SQL statement\n");
    sqlite3_close(db);
    return userData;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    const unsigned char *titleText = sqlite3_column_text(stmt, 0);
    const unsigned char *idText = sqlite3_column_text(stmt, 1);
    float progress = sqlite3_column_int(stmt, 2);

    bookData entry;

    if (titleText) {
      entry.title = reinterpret_cast<const char *>(titleText);
    } else { entry.title = ""; }

    if (idText) {
      entry.contentID = reinterpret_cast<const char *>(idText);
    } else { entry.contentID = ""; }
    
    entry.progress = progress;

    userData.push_back(entry);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return userData;
}