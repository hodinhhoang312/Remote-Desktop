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


WORD mapSfmlKeyToVirtualKey(sf::Keyboard::Key sfmlKeyCode) {
    switch (sfmlKeyCode) {
        // Bảng chữ cái
    case sf::Keyboard::A: return 'A';
    case sf::Keyboard::B: return 'B';
    case sf::Keyboard::C: return 'C';
    case sf::Keyboard::D: return 'D';
    case sf::Keyboard::E: return 'E';
    case sf::Keyboard::F: return 'F';
    case sf::Keyboard::G: return 'G';
    case sf::Keyboard::H: return 'H';
    case sf::Keyboard::I: return 'I';
    case sf::Keyboard::J: return 'J';
    case sf::Keyboard::K: return 'K';
    case sf::Keyboard::L: return 'L';
    case sf::Keyboard::M: return 'M';
    case sf::Keyboard::N: return 'N';
    case sf::Keyboard::O: return 'O';
    case sf::Keyboard::P: return 'P';
    case sf::Keyboard::Q: return 'Q';
    case sf::Keyboard::R: return 'R';
    case sf::Keyboard::S: return 'S';
    case sf::Keyboard::T: return 'T';
    case sf::Keyboard::U: return 'U';
    case sf::Keyboard::V: return 'V';
    case sf::Keyboard::W: return 'W';
    case sf::Keyboard::X: return 'X';
    case sf::Keyboard::Y: return 'Y';
    case sf::Keyboard::Z: return 'Z';

        // Số và ký tự đặc biệt
    case sf::Keyboard::Num0: return '0';
    case sf::Keyboard::Num1: return '1';
    case sf::Keyboard::Num2: return '2';
    case sf::Keyboard::Num3: return '3';
    case sf::Keyboard::Num4: return '4';
    case sf::Keyboard::Num5: return '5';
    case sf::Keyboard::Num6: return '6';
    case sf::Keyboard::Num7: return '7';
    case sf::Keyboard::Num8: return '8';
    case sf::Keyboard::Num9: return '9';
    case sf::Keyboard::Space: return VK_SPACE;
    case sf::Keyboard::Enter: return VK_RETURN;
    case sf::Keyboard::Backspace: return VK_BACK;
    case sf::Keyboard::Tab: return VK_TAB;
    case sf::Keyboard::Escape: return VK_ESCAPE;
    case sf::Keyboard::LControl: return VK_LCONTROL;
    case sf::Keyboard::LShift: return VK_LSHIFT;
    case sf::Keyboard::LAlt: return VK_LMENU;
    case sf::Keyboard::LSystem: return VK_LWIN;
    case sf::Keyboard::RControl: return VK_RCONTROL;
    case sf::Keyboard::RShift: return VK_RSHIFT;
    case sf::Keyboard::RAlt: return VK_RMENU;
    case sf::Keyboard::RSystem: return VK_RWIN;

        // Các phím điều hướng
    case sf::Keyboard::Up: return VK_UP;
    case sf::Keyboard::Down: return VK_DOWN;
    case sf::Keyboard::Left: return VK_LEFT;
    case sf::Keyboard::Right: return VK_RIGHT;

        // Các phím chức năng
    case sf::Keyboard::F1: return VK_F1;
    case sf::Keyboard::F2: return VK_F2;
    case sf::Keyboard::F3: return VK_F3;
    case sf::Keyboard::F4: return VK_F4;
    case sf::Keyboard::F5: return VK_F5;
    case sf::Keyboard::F6: return VK_F6;
    case sf::Keyboard::F7: return VK_F7;
    case sf::Keyboard::F8: return VK_F8;
    case sf::Keyboard::F9: return VK_F9;
    case sf::Keyboard::F10: return VK_F10;
    case sf::Keyboard::F11: return VK_F11;
    case sf::Keyboard::F12: return VK_F12;

        // Các phím số
    case sf::Keyboard::Numpad0: return VK_NUMPAD0;
    case sf::Keyboard::Numpad1: return VK_NUMPAD1;
    case sf::Keyboard::Numpad2: return VK_NUMPAD2;
    case sf::Keyboard::Numpad3: return VK_NUMPAD3;
    case sf::Keyboard::Numpad4: return VK_NUMPAD4;
    case sf::Keyboard::Numpad5: return VK_NUMPAD5;
    case sf::Keyboard::Numpad6: return VK_NUMPAD6;
    case sf::Keyboard::Numpad7: return VK_NUMPAD7;
    case sf::Keyboard::Numpad8: return VK_NUMPAD8;
    case sf::Keyboard::Numpad9: return VK_NUMPAD9;

        // Các phím mũi tên và các phím khác
    case sf::Keyboard::Insert: return VK_INSERT;
    case sf::Keyboard::Delete: return VK_DELETE;
    case sf::Keyboard::Home: return VK_HOME;
    case sf::Keyboard::End: return VK_END;
    case sf::Keyboard::PageUp: return VK_PRIOR;
    case sf::Keyboard::PageDown: return VK_NEXT;
    case sf::Keyboard::Add: return VK_ADD;
    case sf::Keyboard::Subtract: return VK_SUBTRACT;
    case sf::Keyboard::Multiply: return VK_MULTIPLY;
    case sf::Keyboard::Divide: return VK_DIVIDE;

        // Thêm các phím khác nếu cần thiết

    default: return 0; // Giá trị mặc định nếu không có ánh xạ
    }
}
