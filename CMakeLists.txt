cmake_minimum_required(VERSION 3.10)

project(RainAlertBot)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# Директории с заголовочными файлами
include_directories(Db)
include_directories(Http)
include_directories(Json)
include_directories(Sheduler)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})

add_executable(main main.cpp Db/dbmanager.cpp Http/requesthandler.cpp Json/jsonhandler.cpp Sheduler/sheduler.cpp)

target_link_libraries(main TgBot ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} pthread sqlite3)