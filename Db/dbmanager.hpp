#ifndef DB_MANAGER_HPP
#define DB_MANAGER_HPP

#include <sqlite3.h>
#include <string>
#include <iostream>

class DbManager
{
public:
    DbManager(const std::string& dbName);
    ~DbManager();

    void addUser(const int telegram_id);
    void removeUser(const int telegram_id);
    
    bool isUserExist(const int telegram_id);
    std::list<int>getAllUsers();

private:
    bool executeQuery(const std::string& query);
    void createTable();

private:
    sqlite3* _db; 
    std::string _dbName; 
};

#endif