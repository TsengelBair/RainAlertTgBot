#include <ctime>
#include <iostream>
#include "jsonhandler.hpp"

bool JsonHandler::checkIfRain(nlohmann::json& jsonObj)
{
    // time_t curDateAndTime = time(0);
    // tm* timeStruct = gmtime(&curDateAndTime);
    time_t curDateAndTime = time(0);
    tm* timeStruct = localtime(&curDateAndTime);

    timeStruct->tm_sec = 0;
    timeStruct->tm_min = 0;
    timeStruct->tm_hour = 0;

    /* текущая дата в секундах в GMT, но время полночь */
    time_t curDateMidnight = mktime(timeStruct);

    /* mktime учитывает локаль, поэтому для GMT такие рассчеты */
    time_t startTimeFromSixPM = curDateMidnight + 3600 * 6;
    time_t endTimeWhenNextDay = curDateMidnight + 3600 * 24;

    std::cout << "стартовое время " <<  startTimeFromSixPM << "\n";
    std::cout << "Конечное время " <<  endTimeWhenNextDay << "\n";

    for (int i = 0; i < jsonObj["list"].size(); ++i){
        const auto& forecastForThreeHours = jsonObj["list"][i];
        auto forecastTime = forecastForThreeHours["dt"];
        std::cout << "Время итерации " << i << " " << forecastTime << "\n";

        if (forecastTime >= startTimeFromSixPM && forecastTime <= endTimeWhenNextDay){
            std::cout << "Подходящий объект " << "\n"<< forecastForThreeHours;
            const auto& weatherArray = forecastForThreeHours["weather"];
            for (const auto& weather : weatherArray){
                if (weather["main"] == "Rain"){
                    return true;
                }
            }
        }
    }

    return false;
}