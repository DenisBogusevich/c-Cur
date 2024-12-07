#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include <string>
#include <winsock2.h>
#include <mutex>

class Server {
public:
    Server(const std::string& address, int port);
    ~Server();

    void start();
    void stop();
    void handleClient(SOCKET clientSocket);
    void forwardMessageToClient(const std::string& clientId, const std::string& message);
    void logMessage(const std::string& message);


private:
    SOCKET serverSocket;
    bool running;
    std::unordered_map<std::string, SOCKET> clients; // Maps client IDs to their sockets
    std::mutex clientsMutex; // Mutex to handle concurrent access to clients map

    void listenForClients();
};

#endif
