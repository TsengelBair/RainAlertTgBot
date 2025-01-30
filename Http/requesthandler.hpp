#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <string>
#include <nlohmann/json.hpp>

class RequestHandler
{
public:
    RequestHandler(const std::string& host, const std::string& target);
    ~RequestHandler();

    nlohmann::json getWeather();

private:
    std::string _host;
    std::string _target;
};

#endif