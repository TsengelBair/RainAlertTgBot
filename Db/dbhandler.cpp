#include <iostream>
#include "dbhandler.h"

DbHandler* DbHandler::instance = nullptr;

DbHandler::DbHandler()
{
    openDb();
}

DbHandler::~DbHandler()
{
    if (m_db) {
        sqlite3_close(m_db);
    }
}

DbHandler* DbHandler::getInstance()
{
    if (!instance) {
        instance = new DbHandler();
    }
    return instance;
}

bool DbHandler::userExist(const int tg_id)
{
    std::string query = "SELECT COUNT(*) FROM users WHERE tg_id = " + std::to_string(tg_id) + ";";

    /* любой sql запрос (в виде строки) преобразуется в объект stmt в sqlite (это подготовленный запрос)*/
    sqlite3_stmt* stmt;
    /*
        1 - указатель на бд
        2 - указатель на строку запроса
        3 - длина строки (-1 означает, что неизвестно, sqlite сам определит)
        4 - указатель на подготовленный запрос
        5 - доп параметры (0 - значит не используются)
    */
    int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    /* выполняем запрос - функция вернет статус запроса */
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count > 0;  // Если count > 0, то пользователь существует
    }

    /* освобождение ресурсов */
    sqlite3_finalize(stmt);
    return false;
}

void DbHandler::addUser(const int tg_id)
{
    std::string query = "INSERT INTO users (tg_id) VALUES (" + std::to_string(tg_id) + ");";
    if (executeQuery(query)) {
        std::cout << "Пользователь добавлен успешно" << std::endl;
    } else {
        std::cout << "Ошибка при добавлении пользователя" << std::endl;
    }
}

void DbHandler::removeUser(const int telegram_id)
{
    std::string query = "DELETE FROM users WHERE tg_id = " + std::to_string(telegram_id) + ";";
    if (executeQuery(query)) {
        std::cout << "Пользователь успешно удален" << std::endl;
    } else {
        std::cout << "Ошибка при удалении пользователя" << std::endl;
    }
}

std::list<int> DbHandler::getAllUsers()
{
    std::list<int> users;

    std::string query = "SELECT tg_id FROM users;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(m_db) << std::endl;
        return users; /* пустой список в случае ошибки */
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt, 0);
        users.push_back(userId);
    }

    sqlite3_finalize(stmt);
    return users;
}

bool DbHandler::executeQuery(const std::string& query)
{
    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, query.c_str(), 0, 0, &errMsg);

    if (result != SQLITE_OK){
        std::cerr << "Ошибка SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

void DbHandler::openDb()
{
    int rc = sqlite3_open("tg_users.db", &m_db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
    } else {
        std::cout << "Opened database successfully" << std::endl;
        bool ok = createTables();
        if (ok) {
            std::cout << "Tables have been created succesfully" << std::endl;
        } else {
            std::cout << "Table creation failed" << std::endl;
        }
    }
}

bool DbHandler::createTables()
{
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tg_id INTEGER NOT NULL UNIQUE
        );
    )";

    char* errMsg;
    int rc = sqlite3_exec(m_db, sql, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
