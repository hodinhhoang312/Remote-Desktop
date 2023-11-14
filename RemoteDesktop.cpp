#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#include "Header.h"
#include "Capture.h"
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

int Client()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create a socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Sử dụng cùng một cổng (port 12345)
    char SERVER_IP_ADDRESS[16];

    // Connect to the server
    while (1)
    {
        if (RequestForIpAddress(SERVER_IP_ADDRESS))
        {
            serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
            if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
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

    //Receive data

    if (Recv_Screen(serverSocket))
        std::cerr << "Success!";

    // Close the socket
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

int Server()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create a socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); // Use port 12345

    // Bind an address and port to the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind an address and port to the socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Failed to listen for incoming connections." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for incoming connections..." << std::endl;

    // Accept a connection
    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept a connection." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connection established!" << std::endl;

    //Send Picture

    Send_Screen(clientSocket);

    // Close the sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

int main() {
    int t;
    std::cerr << "1.Server.\n";
    std::cerr << "2.Client.\n";
    std::cerr << "Choose:\n";
    std::cin >> t;
    if (t == 1)
        Server();
    else
        Client();
}
