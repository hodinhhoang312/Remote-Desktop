
#pragma once
#include <SFML/Graphics.hpp>

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
    sprite_background_image.setScale((resize / 100), (resize / 100));
    window.draw(sprite_background_image);
    return;
}

void Draw_Button(sf::RenderWindow& window, sf::Event event, bool& isButtonClicked, int widthpos, int heightpos)
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

    spriteNormal.setPosition((widthpos), (heightpos)); // Set position for the button
    spriteClicked.setPosition((widthpos), (heightpos)); // Same position as the normal state

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
