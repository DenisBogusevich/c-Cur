#include "Client.h"
#include <iostream>
#include <thread>

Client::Client(const std::string& address, int port, const std::string& userId)
    : clientId(userId) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server with ID: " << clientId << std::endl;
}

Client::~Client() {
    closesocket(clientSocket);
    WSACleanup();
}

void Client::sendMessage(const std::string& recipientId, const std::string& message) {
    std::string fullMessage = clientId + ":" + recipientId + ":" + message;
    send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0);
}

void Client::receiveMessages() {
    wchar_t buffer[1024];
    int bytesRead;

    while (true) {
        bytesRead = recv(clientSocket, reinterpret_cast<char *>(buffer), sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cerr << "Disconnected from server" << std::endl;
            break;
        }
        buffer[bytesRead] = '\0';
        std::cout << "Received: " << reinterpret_cast<char *>(buffer) <<"\n"<< std::endl;
    }
}




void messageSender(Client& client) {
    while (true) {
        std::string recipientId, message;
        std::cout << "Enter recipient ID (or type 'exit' to quit): ";
        std::getline(std::cin, recipientId);
        if (recipientId == "exit") break;

        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        client.sendMessage(recipientId, message);
    }
}

void messageReceiver(Client& client) {
    client.receiveMessages();
}
#ifndef TEST_BUILD

int main() {
    std::string userId;
    std::cout << "Enter your unique client ID: ";
    std::cin >> userId;
    std::cin.ignore(); // Clear newline from input buffer

    Client client("127.0.0.1", 8080, userId);

    std::thread senderThread(messageSender, std::ref(client));
    std::thread receiverThread(messageReceiver, std::ref(client));

    senderThread.join();
    receiverThread.join();

    return 0;
}
#endif
