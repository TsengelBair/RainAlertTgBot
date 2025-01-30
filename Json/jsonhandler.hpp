#ifndef JSON_HANDLER_HPP
#define JSON_HANDLER_HPP

#include <nlohmann/json.hpp>

class JsonHandler
{
public:
    static bool checkIfRain(nlohmann::json& jsonObj);
}; 

#endif