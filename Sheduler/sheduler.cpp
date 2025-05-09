#include <nlohmann/json.hpp>

#include "sheduler.h"
#include "Db/dbhandler.h"
#include "Request/requesthandler.h"
#include "Boundary/timeboundarymaker.h"
#include "Parser/parser.h"

Sheduler::Sheduler(TgBot::Bot &bot) : bot(bot), running(true)
{
}

Sheduler::~Sheduler()
{
    stop();
}

bool Sheduler::isRain()
{
    /// запрос и ответ в виде json
    nlohmann::json jsonResponce = RequestHandler::getWeather();
    /// диапазон времени (стартовая и конечная граница)
    TimeBoundaryMaker maker;
    std::pair<std::time_t, std::time_t> boundaries = maker.getBoundaries();

    /// передаем границы в конструктор, т.к. в методе isRain проверяем по диапазону границ
    Parser parser(boundaries);
    bool rain = parser.isRain(jsonResponce);

    return rain;
}

void Sheduler::run()
{
    while (running) {
        time_t curDateAndTime = time(0);
        tm* timeStruct = localtime(&curDateAndTime);

        if (timeStruct->tm_hour == 5) {
            bool rain = isRain();
            if (rain) {
                auto users = DbHandler::getInstance()->getAllUsers();
                if (!users.empty()) {
                    for (const auto& userId : users) {
                        bot.getApi().sendMessage(userId, "Ожидается дождь");
                    }
                    /// чтобы избежать повторной отправки
                    std::this_thread::sleep_for(std::chrono::minutes(60));
                }
            }
        }
        /// проверку диапазона времени осуществляем раз в 50 минут
        std::this_thread::sleep_for(std::chrono::minutes(50));
    }
}

void Sheduler::start()
{
    shedulerThread = std::thread(&Sheduler::run, this);
}

void Sheduler::stop()
{
    running = false;
    if (shedulerThread.joinable()){
        shedulerThread.join();
    }
}
