#include <list>
#include "dbmanager.hpp"

DbManager::DbManager(const std::string& dbName) : _dbName(dbName), _db(nullptr)
{
    int connection = sqlite3_open(_dbName.c_str(), &_db);
    if (connection != SQLITE_OK){
        std::cerr << "Ошибка подключения к базе данных: " << sqlite3_errmsg(_db) << std::endl;
    } else {
        std::cout << "Успешное подключение к базе данных" << std::endl;
    }

    createTable();
};

DbManager::~DbManager() {
    if (_db) {
        sqlite3_close(_db); 
        std::cout << "База данных закрыта" << std::endl;
    }
}

bool DbManager::isUserExist(const int telegram_id)
{
    std::string query = "SELECT COUNT(*) FROM tg_users WHERE tg_id = " + std::to_string(telegram_id) + ";";

    /* любой sql запрос (в виде строки) преобразуется в объект stmt в sqlite (это подготовленный запрос)*/
    sqlite3_stmt* stmt;
    /*
        1 - указатель на бд
        2 - указатель на строку запроса
        3 - длина строки (-1 означает, что неизвестно, sqlite сам определит)
        4 - указатель на подготовленный запрос
        5 - доп параметры (0 - значит не используются)
    */
    int rc = sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(_db) << std::endl;
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

std::list<int> DbManager::getAllUsers()
{
    std::list<int> users;
    std::string query = "SELECT tg_id FROM tg_users;"; 
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(_db) << std::endl;
        return users; /* пустой список в случае ошибки */
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt, 0); 
        users.push_back(userId); 
    }

    sqlite3_finalize(stmt);
    return users; 
}

/* возможно добавить проверку на то, была ли таблица создана ранее */
void DbManager::createTable()
{
    std::string query = "CREATE TABLE IF NOT EXISTS tg_users (tg_id INT PRIMARY KEY);";
    if (executeQuery(query)) {
        std::cout << "Таблица успешно создана" << std::endl;
    } else {
        std::cout << "Ошибка при создании таблицы" << std::endl;
    }
};

/* при добавлении и удалении предварительно вызывать метод isUserExist в main.cpp */
void DbManager::addUser(const int telegram_id)
{
    std::string query = "INSERT INTO tg_users (tg_id) VALUES (" + std::to_string(telegram_id) + ");";
    if (executeQuery(query)) {
        std::cout << "Пользователь добавлен успешно" << std::endl;
    } else {
        std::cout << "Ошибка при добавлении пользователя" << std::endl;
    }   
}

void DbManager::removeUser(const int telegram_id)
{
    std::string query = "DELETE FROM tg_users WHERE tg_id = " + std::to_string(telegram_id) + ";";
    if (executeQuery(query)) {
        std::cout << "Пользователь добавлен успешно" << std::endl;
    } else {
        std::cout << "Ошибка при добавлении пользователя" << std::endl;
    }   
}

bool DbManager::executeQuery(const std::string& query)
{
    char* errMsg = nullptr;
    int result = sqlite3_exec(_db, query.c_str(), 0, 0, &errMsg);

    if (result != SQLITE_OK){
        std::cerr << "Ошибка SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}