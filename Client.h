#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class Client {
private:
    SOCKET clientSocket;
    std::string clientId; // User-provided unique client ID

public:
    // Constructor: Initializes the client with server address, port, and user-provided ID
    Client(const std::string& address, int port, const std::string& userId);

    // Destructor: Cleans up resources
    ~Client();

    // Sends a message to the server
    void sendMessage(const std::string& recipientId, const std::string& message);

    // Receives messages from the server
    void receiveMessages();
};

#endif // CLIENT_H
