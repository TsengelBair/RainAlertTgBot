#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <string>
#include <nlohmann/json.hpp>

class RequestHandler
{
public:
    static nlohmann::json getWeather();

private:
    static const std::string m_host;
    static const std::string m_target;
};

#endif // REQUESTHANDLER_H
