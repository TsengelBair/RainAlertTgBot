cmake_minimum_required(VERSION 3.14)

project(rainBot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})

add_executable(rainBot
  main.cpp
  Db/dbhandler.h
  Db/dbhandler.cpp
  Request/requesthandler.h
  Request/requesthandler.cpp
  Boundary/timeboundarymaker.h
  Boundary/timeboundarymaker.cpp
  Parser/parser.h
  Parser/parser.cpp
  Sheduler/sheduler.h
  Sheduler/sheduler.cpp
)

target_link_libraries(rainBot TgBot ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} pthread sqlite3)
