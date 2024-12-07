#include "Server.h"
#include <iostream>

// A simple macro for asserting conditions
#define ASSERT_TRUE(condition)                                         \
    if (!(condition)) {                                                \
        std::cerr << "Assertion failed at " << __FILE__ << ":"         \
                  << __LINE__ << " in " << __FUNCTION__ << std::endl;  \
        return false;                                                  \
          }

#define ASSERT_EQ(expected, actual)                                    \
    if ((expected) != (actual)) {                                      \
        std::cerr << "Assertion failed at " << __FILE__ << ":"         \
                  << __LINE__ << " in " << __FUNCTION__ << std::endl;  \
        std::cerr << "  Expected: " << (expected) << std::endl;        \
        std::cerr << "  Actual: " << (actual) << std::endl;            \
        return false;                                                  \
}

// Helper macro for running tests
#define RUN_TEST(test)                                                 \
    if (!(test())) {                                                   \
        std::cerr << #test << " failed." << std::endl;                 \
        ++failedTests;                                                 \
    } else {                                                           \
        std::cout << #test << " passed." << std::endl;                 \
    }




bool testInitialization() {
    try {
        Server server("127.0.0.1", 8080);
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    }
    return true;
}


bool testStartStop() {
    Server server("127.0.0.1", 8080);
    try {
        server.start();
        server.stop();
    } catch (const std::exception& e) {
        std::cerr << "Exception during start/stop: " << e.what() << std::endl;
        return false;
    }
    return true;
}
#include <thread>
#include <winsock2.h>

bool testClientConnection() {
    Server server("127.0.0.1", 8080);
    server.start();

    bool clientConnected = false;

    std::thread clientThread([&clientConnected]() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != SOCKET_ERROR) {
            clientConnected = true;
        }

        closesocket(clientSocket);
        WSACleanup();
    });

    clientThread.join();
    server.stop();

    ASSERT_TRUE(clientConnected);
    return true;
}

int main() {
    int failedTests = 0;

    RUN_TEST(testInitialization);
    RUN_TEST(testStartStop);
    RUN_TEST(testClientConnection);

    std::cout << (failedTests == 0 ? "All tests passed!" : "Some tests failed.") << std::endl;
    return failedTests;
}

