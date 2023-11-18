#pragma once
#include <SFML/Graphics.hpp>
#include <thread>
#include "Header.h"
#include "Capture.h"

const int width = 1920;
const int height = 1080;

sf::Event Get_Event(sf::RenderWindow& window)
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
    return event;
}

void Draw_Background(sf::RenderWindow& window, std::string loca)
{
    sf::Texture background;
    background.loadFromFile(loca);
    sf::Sprite sprite_background_image(background);
    window.draw(sprite_background_image);
    return;
}

void Draw_Button(sf::RenderWindow& window,sf::Event event, bool &isButtonClicked, int widthpos, int heightpos)
{
    // Load textures for button states (normal and clicked)
    sf::Texture textureNormal, textureClicked;
    textureNormal.loadFromFile("Resources/Image/button_normal.png");
    textureClicked.loadFromFile("Resources/Image/button_clicked.png");

    // Create sprites for button states
    sf::Sprite spriteNormal(textureNormal);
    sf::Sprite spriteClicked(textureClicked);

    spriteNormal.setPosition((widthpos)*resize/100 - 75,(heightpos)*resize/100-25 ); // Set position for the button
    spriteClicked.setPosition((widthpos) * resize / 100 - 75, (heightpos) * resize / 100-25); // Same position as the normal state

    if (event.type == sf::Event::MouseButtonPressed) {
        // Check if the mouse is clicked within the button area
        if (event.mouseButton.button == sf::Mouse::Left &&
            spriteNormal.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
            isButtonClicked = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        // Reset button state if mouse released
        isButtonClicked = false;
    }
    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            // Lấy vị trí khi thả chuột trái
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            std::cerr << "Release mouse location X = " << mousePos.x << ", Y = " << mousePos.y << std::endl;
        }
    }

    // Draw the appropriate button state based on click status
    if (isButtonClicked) {
        window.draw(spriteClicked);
    }
    else {
        window.draw(spriteNormal);
    }

    return;
}

void Draw_Text(sf::RenderWindow& window, std::string txt,sf::Color color, int widthpos, int heightpos)
{
    // Tạo một font
    sf::Font font;
    font.loadFromFile("Resources/Fonts/arial.ttf");

    // Tạo đối tượng văn bản và thiết lập thuộc tính
    sf::Text text;
    text.setFont(font); // Đặt font cho văn bản
    text.setString(txt); // Đặt nội dung của văn bản
    text.setCharacterSize(24); // Đặt kích thước ký tự
    text.setFillColor(color); // Đặt màu cho văn bản
    text.setPosition(widthpos, heightpos); // Đặt vị trí của văn bản

    window.draw(text);

    return;
}

void Server_Display_Screen(sf::RenderWindow &window, sf::Event event, std::string txt, bool &Server_On )
{
    window.clear();

    Draw_Background(window, "Resources/Image/background.png");

    Draw_Text(window, txt , sf::Color::Black, width*resize/100 / 2, height*resize/100 / 2 - 200);

    bool empy = 0;
    Draw_Button(window, event, empy, 0, 0);

    Draw_Text(window, "Back", sf::Color::Black, 0, 0);

    if (empy)
        Server_On = false;

    return;
}

void Client_Display_Screen(sf::RenderWindow& window, sf::Event event, bool& Client_On)
{
    Client_On = 1;
    sf::Font font;
    if (!font.loadFromFile("Resources/Fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
        return ;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setPosition(10, 10);
    text.setString("Enter IP Address: ");

    std::string ipAddress;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else
                if (event.text.unicode == 13)
                    window.clear();
                else
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode < 128 && event.text.unicode != 8) {
                            // Kiểm tra ký tự nhập vào, loại bỏ ký tự backspace (8)
                            ipAddress += static_cast<char>(event.text.unicode);
                        }
                        else if (event.text.unicode == 8 && !ipAddress.empty()) {
                            // Xóa ký tự cuối cùng nếu là backspace và xâu không rỗng
                            ipAddress.pop_back();
                        }
                    }
        }

        // Hiển thị IP đã nhập
        text.setString("Enter IP Address: " + ipAddress);

        window.clear();
        window.draw(text);
        window.display();
    }

    // Chuyển đổi std::string sang char*
    char* ipAddressChar = new char[ipAddress.size() + 1];
    std::strcpy(ipAddressChar, ipAddress.c_str());

    window.clear();

    Client(window, ipAddressChar);

    delete[] ipAddressChar;

    Client_On = 0;

    return;
}

void Main_Display_Screen()
{
    sf::RenderWindow window(sf::VideoMode(1920 * resize / 100, 1080 * resize / 100), "Remote Desktop Client"); // Resolution 1920x1080

    bool isButtonClicked_Server = false;
    bool isButtonClicked_Client = false;

    bool Server_On = false;
    bool Server_Connected = false;
    bool Client_On = false;
    bool Client_Connected = false;

    while (window.isOpen()) {

        window.clear();

        Draw_Background(window, "Resources/Image/background.png");

        sf::Event event = Get_Event(window);

        Draw_Button(window, event, isButtonClicked_Server, width / 2, height / 2 - 100);
        Draw_Button(window, event, isButtonClicked_Client, width / 2, height / 2 + 100);

        Draw_Text(window, "SERVER",sf::Color::White, 716, 338 );

        Draw_Text(window, "CLIENT",sf::Color::White, 725, 498 );

        if (Server_On && !Server_Connected)
            Server_Display_Screen(window, event, "Waitting For Connection!...", Server_On);
        if (Server_Connected)
            Server_Display_Screen(window, event, "Server Connected!", Server_On);

        if (isButtonClicked_Client)
            Client_Display_Screen(window, event, Client_On);

        if (isButtonClicked_Server && Server_On == 0)
        {
            std::thread THREAD_S(Server, std::ref(window), std::ref(Server_Connected));
            THREAD_S.detach(), Server_On = 1;
        }

        window.display();
    }
}