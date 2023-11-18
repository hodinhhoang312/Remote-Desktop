#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#include "Header.h"
#include "Capture.h"
#include "Display.h"
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include <thread>

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

int main() {
  Main_Display_Screen();
}