#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <sqlite3.h>
#include <string>
#include <list>

class DbHandler
{
private:
    DbHandler();
    ~DbHandler();

    static DbHandler* instance;
    sqlite3* m_db;

private:
    void openDb();
    bool createTables();
    bool executeQuery(const std::string& query);

public:
    static DbHandler* getInstance();
    bool userExist(const int tg_id);
    void addUser(const int tg_id);
    void removeUser(const int telegram_id);
    std::list<int> getAllUsers();
};

#endif // DBHANDLER_H
