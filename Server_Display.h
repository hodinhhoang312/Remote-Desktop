#pragma once
#include "Tools.h"
#include "Capture.h"

void simulateKeyPress(sf::Keyboard::Key key) {

    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    // Chuyển đổi SFML KeyCode thành mã phím
    int vkCode = static_cast<int>(key);
    if (vkCode != -1) {
        input.ki.wVk = vkCode;

        // Gửi sự kiện bấm phím xuống
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(INPUT));

        // Chờ một chút (đảm bảo bấm phím được xử lý)
        Sleep(100);

        // Gửi sự kiện nâng phím lên
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
    else {
        std::cout << "Không thể chuyển đổi SFML KeyCode thành mã phím." << std::endl;
    }
}

WORD UnicodeToVK(wchar_t unicodeChar) {
    // Chuyển đổi Unicode thành mã Virtual-Key
    return VkKeyScanW(unicodeChar);
}

void simulateTextEntered(sf::Uint32 key) {

    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    // Chuyển đổi Unicode thành mã phím
    WORD vkCode = UnicodeToVK(key);
    if (vkCode != 0xFFFF) {
        input.ki.wVk = vkCode;

        // Gửi sự kiện bấm phím xuống
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(INPUT));

        // Chờ một chút (đảm bảo bấm phím được xử lý)
        Sleep(100);

        // Gửi sự kiện nâng phím lên
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
    else {
        std::cout << "Không thể chuyển đổi Unicode thành mã phím." << std::endl;
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

        if (event.type == sf::Event::KeyPressed ) {
            simulateKeyPress(event.key.code);
        }
        if (event.type == sf::Event::TextEntered) {

        }

        switch (event.type)
        {
        case sf::Event::TextEntered:
            simulateTextEntered(event.text.unicode);
            break;
        case sf::Event::KeyPressed:
            simulateKeyPress(event.key.code);
            break;
        }


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

    Draw_Background(window, "Resources/Image/background.png");

    Draw_Text(window, txt, 30, sf::Color::Black, width / 2 - 100, height / 2 - 200);

    bool empy = 0;
    Draw_Button(window, event, empy, 0, 0);

    Draw_Text(window, "Back", 30, sf::Color::Black, 0, 0);

    if (empy)
        Server_On = false;

    return;
}
