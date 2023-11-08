#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>

#include "Header.h"


#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create a socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Sử dụng cùng một cổng (port 12345)
    char SERVER_IP_ADDRESS[16];

    // Connect to the server
    while(1)
    {

        if (RequestForIpAddress(SERVER_IP_ADDRESS))
        {   
            serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
            if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "Failed to connect to the server." << std::endl;
            }
            else
            {
                std::cout << "Connected to the server!" << std::endl;
                break;
            }
        }
        else
            std::cerr << "IP server not found! Please try again!" << std::endl;
    }


    //Text
    char buffer[1024];

    while (true) {
        std::string message;
        std::cout << "Client: ";
        std::getline(std::cin, message);

        send(clientSocket, message.c_str(), message.length(), 0);

        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cerr << "Connection closed by server." << std::endl;
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }
    //

    // Close the socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
