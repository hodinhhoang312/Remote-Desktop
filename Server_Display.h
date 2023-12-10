#pragma once
#include "Tools.h"
#include "Capture.h"

void processEvent(const sf::Event& event) {
    INPUT input;
    input.type = INPUT_KEYBOARD;

    // Kiểm tra sự kiện từ sf::Event
    switch (event.type) {
    case sf::Event::KeyPressed:
        // Bấm phím
        input.ki.wVk = mapSfmlKeyToVirtualKey(event.key.code);        
        std::cerr << "Press: " << int(input.ki.wVk) << '\n';
        input.ki.dwFlags = 0; // Keydown
        SendInput(1, &input, sizeof(INPUT));
        break;

    case sf::Event::KeyReleased:
        // Nhả phím
        input.ki.wVk = mapSfmlKeyToVirtualKey(event.key.code);        
        std::cerr << "Release: " << int(input.ki.wVk) << '\n';
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
        break;
        /*
    case sf::Event::MouseButtonPressed:
        // Click chuột
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        input.mi.dx = event.mouseButton.x * 100 / resize;
        input.mi.dy = event.mouseButton.y * 100 / resize;
        SendInput(1, &input, sizeof(INPUT));
        break;

    case sf::Event::MouseButtonReleased:
        // Nhả chuột
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        input.mi.dx = event.mouseButton.x * 100 / resize;
        input.mi.dy = event.mouseButton.y * 100 / resize;
        SendInput(1, &input, sizeof(INPUT));
        break;

        // Thêm các trường hợp khác nếu cần thiết (ví dụ: di chuyển chuột, scroll chuột, v.v.)*/
    }
}

int Send_Screen(SOCKET clientSocket)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    sf::TcpListener listener;
    if (listener.listen(312) != sf::Socket::Done) {
        std::cerr << "Failed to listen on port 312.\n";
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 312...\n";

    sf::TcpSocket socket;
    if (listener.accept(socket) != sf::Socket::Done) {
        std::cerr << "Failed to accept client connection.\n";
        WSACleanup();
        return 1;
    }
    while (1)
    {
        sf::Event event;
        std::size_t receivedSize;
        if (socket.receive(&event, sizeof(event), receivedSize) != sf::Socket::Done) {
            std::cerr << "Error receiving event from client\n";
        }

        processEvent(event);


        cv::Mat screenshot = Capture_Screen(); // Chup man hinh

        reduceSize(screenshot, resize);

        if (!screenshot.empty()) {

            int bytesSend = sendMatOverSocket(screenshot, clientSocket);

            if (bytesSend > 0);
            // std::cerr << "Send Successful! BytesSend : " << bytesSend << ".\n";
            else
            {
                std::cerr << "Cannot Send Image!\n";
                return 2;
            }
        }
        else {
            std::cerr << "Error: Could not read the image." << std::endl;
        }
    }

    socket.disconnect();
    listener.close();
    WSACleanup();

    return 0;
}

int Server(sf::RenderWindow& window, bool& ServerConnected)
{
    if (ServerConnected)
        return 1;
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
    else
        std::cerr << "Successed to bind an address and port to the socket.\n";

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

    ServerConnected = 1;
    //Send Picture

    //std::thread THREAD_RECV(Receive_Event,std::ref(window), std::ref(ServerConnected));
   // THREAD_RECV.detach();

    Send_Screen(clientSocket);

    // Close the sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    ServerConnected = 0;

    return 0;
}

void Server_Display_Screen(sf::RenderWindow& window, sf::Event event, std::string txt, bool& Server_On)
{
    window.clear();

    //Draw_Background(window, "Resources/Image/background.png");

    Draw_Text(window, txt, 30, sf::Color::Black, width / 2 - 100, height / 2 - 200);

    bool empy = 0;
    Draw_Button(window, event, empy, 0, 0);

    Draw_Text(window, "Back", 30, sf::Color::Black, 0, 0);

    if (empy)
        Server_On = false;

    return;
}
