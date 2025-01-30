#ifndef SHEDULER_HPP
#define SHEDULER_HPP

#include <thread>
#include <tgbot/tgbot.h>
#include "../Db/dbmanager.hpp"
#include "../Http/requesthandler.hpp"


class Sheduler
{
public:
    Sheduler(TgBot::Bot& bot, DbManager& db);
    ~Sheduler();

    void start();
    void stop();

private:
    bool isRain();
    void run();

private:
    std::thread shedulerThread;
    bool running;
    
    TgBot::Bot& bot;
    DbManager& db;
};

#endif