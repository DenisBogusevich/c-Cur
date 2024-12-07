cmake_minimum_required(VERSION 3.30)
project(KNUPROJECT)

set(CMAKE_CXX_STANDARD 20)


add_executable(Server
        Server.h
        Server.cpp)

add_executable(Client
        Client.h
        Client.cpp)


target_link_libraries(Server ws2_32)


target_link_libraries(Client ws2_32)

add_executable(Server_test Server_test.cpp Server.cpp)
target_compile_definitions(Server_test PRIVATE TEST_BUILD)
target_link_libraries(Server_test ws2_32)

add_executable(Client_test Client_test.cpp Client.cpp)
target_compile_definitions(Client_test PRIVATE TEST_BUILD)
target_link_libraries(Client_test ws2_32)
