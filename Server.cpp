#include "Server.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

Server::Server(const std::string& address, int port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
    serverAddr.sin_port = htons(port);
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    logMessage("Server runnig " + address + ":" + std::to_string(port));

}

Server::~Server() {
    WSACleanup();
}

void Server::start() {
    running = true;
    std::thread listener(&Server::listenForClients, this);
    listener.detach();
}

void Server::stop() {
    running = false;
    closesocket(serverSocket);
}

void Server::listenForClients() {
    while (running) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        logMessage("Client socket" + std::to_string(clientSocket));
        clientThread.detach();
    }
}

void Server::handleClient(SOCKET clientSocket) {
    wchar_t buffer[1024];
    int bytesRead;

    while (running) {
        bytesRead = recv(clientSocket, reinterpret_cast<char *>(buffer), sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        std::string message(reinterpret_cast<char *>(buffer), bytesRead);

        // Parse the message format: "sender_id:recipient_id:message_content"
        std::istringstream stream(message);
        std::string senderId, recipientId, messageContent;

        std::getline(stream, senderId, ':');
        std::getline(stream, recipientId, ':');
        std::getline(stream, messageContent);

        std::lock_guard<std::mutex> lock(clientsMutex);

        // Register sender's socket
        clients[senderId] = clientSocket;

        logMessage("Sender: " + senderId + ", recipient: " + recipientId + ", Message: " + messageContent);

        // Forward the message if the recipient exists
        if (clients.count(recipientId)) {
            forwardMessageToClient(recipientId, messageContent);
        } else {
            std::string errorMessage = "Error: Recipient " + recipientId + " not connected.";

            send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}

void Server::forwardMessageToClient(const std::string& recipientId, const std::string& message) {
    SOCKET recipientSocket = clients[recipientId];
    send(recipientSocket, message.c_str(), message.length(), 0);
}


void Server::logMessage(const std::string& message) {
    std::ofstream logFile("server_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
    std::cout << message << std::endl;
}

#ifndef TEST_BUILD
int main() {


    Server server("127.0.0.1", 8080);
    server.start();

    // Keep server running
    std::this_thread::sleep_for(std::chrono::hours(1));

    server.stop();
    return 0;
}
#endif

