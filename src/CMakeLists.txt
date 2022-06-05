# add_subdirectory(../3rdparty/libevent)

# set(CMAKE_EXE_LINKER_FLAGS "-levent")
# set(CMAKE_EXE_LINKER_FLAGS "-pthread")

add_executable(server server.cpp)
add_executable(client client.cpp)
# target_link_libraries(server -levent)


