#include "parser.h"


Parser::Parser(const std::pair<time_t, time_t> &boundaries) : m_boundaries(boundaries)
{
}

bool Parser::isRain(const nlohmann::json &json)
{
    for (const auto& item : json["list"]) {
        long long dt = item["dt"];
        if (dt >= m_boundaries.first && dt <= m_boundaries.second) {
            std::cout << "Подходящий объект: " << "\n" << item << std::endl;
            auto weatherInfo = item["weather"];
            for (const auto& weather : weatherInfo) {
                /// возможно добавить также проверку на град, снегопад и прочее
                if (weather["main"] == "Rain") {
                    return true;
                }
            }
        }
    }

    return false;
}
