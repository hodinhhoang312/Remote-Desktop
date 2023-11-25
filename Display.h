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
    sprite_background_image.setScale((resize/100), (resize/100));
    window.draw(sprite_background_image);
    return;
}

void Draw_Button(sf::RenderWindow& window,sf::Event event, bool &isButtonClicked, int widthpos, int heightpos)
{
    widthpos = widthpos * resize / 100;
    heightpos = heightpos * resize / 100;


    bool isButtonClicking = false;
    // Load textures for button states (normal and clicked)
    sf::Texture textureNormal, textureClicked;
    textureNormal.loadFromFile("Resources/Image/button_normal.png");
    textureClicked.loadFromFile("Resources/Image/button_clicked.png");

    // Create sprites for button states
    sf::Sprite spriteNormal(textureNormal);
    sf::Sprite spriteClicked(textureClicked);

    spriteNormal.setPosition((widthpos),(heightpos) ); // Set position for the button
    spriteClicked.setPosition((widthpos), (heightpos) ); // Same position as the normal state

    if (event.type == sf::Event::MouseButtonPressed) {
        // Check if the mouse is clicked within the button area
        if (event.mouseButton.button == sf::Mouse::Left &&
            spriteNormal.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
            isButtonClicking = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        // Reset button state if mouse released

        isButtonClicking = false;

    }
    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {        
            sf::FloatRect spriteBounds = spriteClicked.getGlobalBounds();

            // In thông tin về sprite
            std::cerr << "Sprite Bounds - Left: " << spriteBounds.left
                << ", Top: " << spriteBounds.top
                << ", Width: " << spriteBounds.width
                << ", Height: " << spriteBounds.height
                << std::endl;

            // Lấy vị trí khi thả chuột trái
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            std::cerr << "Release mouse location X = " << mousePos.x << ", Y = " << mousePos.y << std::endl;

            // Kiểm tra xem tọa độ chuột có nằm trong sprite hay không
            if (spriteBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                isButtonClicked = true;
                std::cerr << "Mouse Released on Sprite!" << std::endl;
            }
            else {
                isButtonClicked = false;
            }
        }
    }

    // Draw the appropriate button state based on click status
    if (isButtonClicking) {
        window.draw(spriteClicked);
    }
    else {
        window.draw(spriteNormal);
    }

    return;
}

void Draw_Text(sf::RenderWindow& window, std::string txt, int Fsize, sf::Color color, int widthpos, int heightpos)
{
    // Tạo một font
    sf::Font font;
    font.loadFromFile("Resources/Fonts/arial.ttf");

    widthpos = widthpos * 80 / 100;
    heightpos = heightpos * 80 / 100;

    // Tạo đối tượng văn bản và thiết lập thuộc tính
    sf::Text text;
    text.setFont(font); // Đặt font cho văn bản
    text.setString(txt); // Đặt nội dung của văn bản
    text.setCharacterSize(Fsize); // Đặt kích thước ký tự
    text.setFillColor(color); // Đặt màu cho văn bản
    text.setPosition(widthpos, heightpos); // Đặt vị trí của văn bản

    window.draw(text);

    return;
}

void Server_Display_Screen(sf::RenderWindow &window, sf::Event event, std::string txt, bool &Server_On )
{
    window.clear();

    Draw_Background(window, "Resources/Image/background.png");

    Draw_Text(window, txt , 30, sf::Color::Black, width/ 2-100, height/ 2 - 200);

    bool empy = 0;
    Draw_Button(window, event, empy, 0, 0);

    Draw_Text(window, "Back", 30, sf::Color::Black, 0, 0);

    if (empy)
        Server_On = false;

    return;
}

void Client_Display_Screen(sf::RenderWindow& window, sf::Event event, bool& Client_On)
{
    sf::Font font;
    if (!font.loadFromFile("Resources/Fonts/arial.ttf")) {
        std::cerr << "Error loading font\n";
        return ;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setPosition(10, 50);
    text.setFillColor(sf::Color::Red); // Đặt màu cho văn bản
    text.setString("Enter IP Address: ");

    std::string ipAddress;

    bool is_Done = 0;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.text.unicode == 13)
            {
                window.clear();
                is_Done = 1;
                break;
            }
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

            window.clear();

            std::cerr << is_Done << '\n';
            Draw_Background(window, "Resources/Image/background.png");

            bool empy = 0;
            Draw_Button(window, event, empy, 0, 0);

            Draw_Text(window, "Back", 30, sf::Color::Black, 0, 0);

            if (empy)
            {
                Client_On = 0;
                std::cerr << "Button Clicked Exit\n";
                window.clear();
                return;
            }

            // Hiển thị IP đã nhập
            text.setString("Enter IP Address: " + ipAddress);

            window.draw(text);
            window.display();

            if (is_Done)
                break;
        }
        if (is_Done)
            break;
    }

    // Chuyển đổi std::string sang char*
    char* ipAddressChar = new char[16];
    for (int i = 0; i < 16; ++i)ipAddressChar[i] = '\0';
    std::strcpy(ipAddressChar, ipAddress.c_str());

    window.clear();

    printf("%s\n", ipAddressChar);

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
    bool Have_Port = false;

    while (window.isOpen()) {
     //   std::cerr << "Start!\n";
      //  std::cerr << Client_On << ' ' << isButtonClicked_Client << '\n';

        window.clear();

        Draw_Background(window, "Resources/Image/background.png");

        sf::Event event = Get_Event(window);

        Draw_Button(window, event, isButtonClicked_Server, 768 + 50, 432 - 100 - 10);
        Draw_Button(window, event, isButtonClicked_Client, 768 + 50, 432 + 100 - 10);

        Draw_Text(window, "SERVER", 24, sf::Color::White, 768 + 75, 432-100 );

        Draw_Text(window, "CLIENT", 24, sf::Color::White, 768 + 90, 432+100 );

        if (isButtonClicked_Server)
            Server_On = true;

        if (Server_On)
        {
            if (Server_Connected)
                Server_Display_Screen(window, event, "Server Connected!", Server_On);
            else
                Server_Display_Screen(window, event, "Waitting For Connection!...", Server_On);
        }
        else
            Have_Port = 0;

        if ( Server_On && !Server_Connected && !Have_Port )
        {
            std::thread THREAD_S(Server, std::ref(window), std::ref(Server_Connected));
            THREAD_S.detach();
           // Server(window, Server_Connected);
            Server_On = 1;
            Have_Port = 1;
        }

        if (isButtonClicked_Client)
            Client_On = true;

      //  std::cerr << Client_On << ' ' << isButtonClicked_Client << '\n';

        if (Client_On)
        {
            Client_Display_Screen(window, event, Client_On);
        }


        isButtonClicked_Server = false;
        isButtonClicked_Client = false;

        window.display();
    }
}
