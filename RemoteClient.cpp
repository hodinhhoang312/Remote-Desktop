#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#include "Capture.h"
#include "Header.h"

#pragma comment(linker, "/STACK:100000000") // Đặt kích thước stack là 10MB
#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

int main() {
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
    while(1)
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
