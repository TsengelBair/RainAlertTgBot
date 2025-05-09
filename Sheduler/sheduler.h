#ifndef SHEDULER_H
#define SHEDULER_H

#include <thread>
#include <tgbot/tgbot.h>

class Sheduler
{
public:
    Sheduler(TgBot::Bot& bot);
    ~Sheduler();

    void start();
    void stop();

private:
    bool isRain();
    void run();

private:
    TgBot::Bot &bot;

    std::thread shedulerThread;
    bool running;
};

#endif // SHEDULER_H
