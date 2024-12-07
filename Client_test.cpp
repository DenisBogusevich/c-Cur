#include <iostream>
#include <thread>

#include "Client.h"



void testSendMessage() {
    std::cout << "=== Test: Sending Message ===" << std::endl;

    // Create two clients
    Client clientA("127.0.0.1", 8080, "ClientA");
    Client clientB("127.0.0.1", 8080, "ClientB");

    // Start receiving threads for both clients
    std::thread receiverA(&Client::receiveMessages, std::ref(clientA));
    std::thread receiverB(&Client::receiveMessages, std::ref(clientB));

    // Client A sends a message to Client B
    clientB.sendMessage("ClientA", "Hello, ClientA!");

    // Allow some time for the message to be received
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Clean up
    clientA.~Client();
    clientB.~Client();
    receiverA.detach();
    receiverB.detach();

    std::cout << "=== End of Test ===" << std::endl;
}

void testSendInvalidData() {
    std::cout << "=== Test: Sending Invalid Data ===" << std::endl;

    // Create a client
    Client client("127.0.0.1", 8080, "ClientA");

    // Send an invalid message (e.g., missing recipient ID or message content)
    client.sendMessage("", "This message has no recipient");

    // Allow some time for the server to process
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Clean up
    client.~Client();

    std::cout << "=== End of Test ===" << std::endl;
}

void testClientDisconnection() {
    std::cout << "=== Test: Client Disconnection ===" << std::endl;

    // Create a client
    Client client("127.0.0.1", 8080, "ClientA");

    // Simulate normal message exchange
    client.sendMessage("Server", "Hello, Server!");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Close the client socket to simulate disconnection
    client.~Client();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "=== End of Test ===" << std::endl;
}

void testServerUnavailable() {
    std::cout << "=== Test: Server Unavailable ===" << std::endl;

    try {
        // Attempt to connect to an unavailable server
        Client client("127.0.0.1", 9999, "ClientA");
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    std::cout << "=== End of Test ===" << std::endl;
}

int main() {
    // Make sure the server is running before executing these tests.

    // Functional tests
    testSendMessage();

    // Invalid data tests
    testSendInvalidData();

    // Disconnection test
    testClientDisconnection();

    // Error handling test
    testServerUnavailable();

    return 0;
}
