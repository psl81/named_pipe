cmake_minimum_required(VERSION 3.11)

project(named_pipe CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

add_executable(host
    host.cpp
)
target_include_directories(host PUBLIC CONAN_PKG::boost)
target_link_libraries(host PUBLIC CONAN_PKG::boost)

add_executable(client
    client.cpp
)
target_include_directories(client PUBLIC CONAN_PKG::boost)
target_link_libraries(client PUBLIC CONAN_PKG::boost)