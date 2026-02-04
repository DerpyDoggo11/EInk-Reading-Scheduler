#pragma once
#include <list>
#include <string>

struct bookData {
    std::string title;
    std::string contentID;
    float progress;
};

std::list<bookData> retrieveLibraryData(const std::string& databasePath);