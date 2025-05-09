#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "requesthandler.h"

namespace http = boost::beast::http;

const std::string RequestHandler::m_host = "api.openweathermap.org";
const std::string RequestHandler::m_target = "/data/2.5/forecast?id=498817&appid=ae65875955cd3b617c2b0e19e6982580&units=metric";

nlohmann::json RequestHandler::getWeather()
{
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::resolver resolver(ioc); /* resolver - преобразует домен а url */
    boost::asio::ip::tcp::socket socket(ioc);

    try {
        boost::asio::connect(socket, resolver.resolve(m_host, "80"));

        http::request<http::string_body> req(http::verb::get, m_target, 11);

        /* заголовок */
        req.set(http::field::host, m_host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);

        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);

        auto jsonResponse = nlohmann::json::parse(boost::beast::buffers_to_string(res.body().data()));

        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        return jsonResponse;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return nlohmann::json{};
    }
}
