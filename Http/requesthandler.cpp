#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <nlohmann/json.hpp>
#include "requesthandler.hpp"

namespace http = boost::beast::http;

RequestHandler::RequestHandler(const std::string& host, const std::string& target) : _host(host), _target(target)
{
}

RequestHandler::~RequestHandler()
{
}

nlohmann::json RequestHandler::getWeather()
{
    boost::asio::io_context ioc; /* объект ввода-вывода */
    boost::asio::ip::tcp::resolver resolver(ioc); /* resolver - преобразует домен а url */
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::connect(socket, resolver.resolve(_host, "80"));
    /*
        тело запроса строка
        1 - get 
        2 - url
        3 - HTTP 1.1
    */    
    http::request<http::string_body> req(http::verb::get, _target, 11);           

    /* заголовок */
    req.set(http::field::host, _host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(socket, req);
    {
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body>res;
        http::read(socket, buffer, res);

        auto jsonResponse = nlohmann::json::parse(boost::beast::buffers_to_string(res.body().data()));
        return jsonResponse;
    }                
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}