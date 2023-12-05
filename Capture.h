#pragma once
#include <SFML/Graphics.hpp>
#include <thread>
#include "Header.h"
#include "Capture.h"
#include "Client_Display.h"
#include "Server_Display.h"
#include "Tools.h"

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
