cmake_minimum_required(VERSION 2.6)

project(httpServer)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost  COMPONENTS  filesystem thread  system date_time REQUIRED)

if(Boost_FOUND)
  message(STATUS "found boost")
endif()

set(http_SRCS
  connection.cpp
  connection_manager.cpp
  io_service_pool.cpp
  mime_types.cpp
  reply.cpp
  request_handler.cpp
  request_parser.cpp
  server.cpp
  main.cpp
 ) 

set(HEADERS
  connection.hpp
  connection_manager.hpp
  handler.hpp
  header.hpp
  io_service_pool.hpp
  mime_types.hpp
  reply.hpp
  request_handler.hpp
  request.hpp
  request_parser.hpp
  server.hpp
  )

add_executable(server ${http_SRCS}) 
target_link_libraries(server
  ${Boost_THREAD_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
)
