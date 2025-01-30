#include "sheduler.hpp"
#include <chrono>
#include "../Json/jsonhandler.hpp"

const std::string host = "api.openweathermap.org";
const std::string target = "/data/2.5/forecast?id=498817&appid=ae65875955cd3b617c2b0e19e6982580&units=metric";


Sheduler::Sheduler(TgBot::Bot& bot, DbManager& db) : bot(bot), db(db), running(true)
{
}

Sheduler::~Sheduler()
{
    stop();
}

bool Sheduler::isRain()
{
    RequestHandler request(host, target);
    nlohmann::json responce = request.getWeather();
    return JsonHandler::checkIfRain(responce);
}

void Sheduler::run()
{
    while (running) {
        time_t curDateAndTime = time(0);
        tm* timeStruct = localtime(&curDateAndTime);
        /* это время рассылки, для теста задать любое удобное */
        if (timeStruct->tm_hour == 9 && timeStruct->tm_min == 55){
            std::string weatherMsg;

            bool rain = isRain();
            if (rain) weatherMsg = "Ожидается дождь";

            std::cout << "Отправляю пользователям погоду" << std::endl;

            if (!weatherMsg.empty()){
                auto users = db.getAllUsers();
                for (const auto& userId : users) {
                    bot.getApi().sendMessage(userId, weatherMsg);
                }       
            }
            /* чтобы избежать повторной отправки */
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        /* ждем минуту перед следующей проверкой */
        std::this_thread::sleep_for(std::chrono::minutes(1));
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