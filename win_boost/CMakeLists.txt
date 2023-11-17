cmake_minimum_required(VERSION 3.15)

project(named_pipe CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

add_executable(host
    host.cpp
)
target_include_directories(host PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(host PUBLIC boost::boost)

add_executable(client
    client.cpp
)
target_include_directories(client PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(client PUBLIC boost::boost)