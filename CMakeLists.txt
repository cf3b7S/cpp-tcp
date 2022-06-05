cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(tcp LANGUAGES CXX)

include_directories(
    /usr/local/include/
    /usr/include
    # /usr/local/opt/mbedtls/include
)

link_directories(
    /usr/local/lib
    /usr/lib
    # /usr/local/opt/mbedtls/lib
)

# add_subdirectory(3rdparty)
add_subdirectory(src)