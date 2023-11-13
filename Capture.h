#pragma once
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include "Header.h"

#pragma comment(linker, "/STACK:100000000") // Đặt kích thước stack là 10MB


cv::Mat Capture_Screen() {
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreen = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, width, height, hScreen, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  // negative height to ensure top-down DIB
    bi.biPlanes = 1;
    bi.biBitCount = 32;     // 32-bit color depth (8 bits for each of B,G,R,A)
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;

    cv::Mat screenshot(height, width, CV_8UC4); // 8-bit per channel (32-bit)
    GetDIBits(hMemoryDC, hBitmap, 0, height, screenshot.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Release resources
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreen);

    return screenshot;
}

int sendMatOverSocket(const cv::Mat& image, SOCKET clientSocket) {
    // Chuyển đổi cv::Mat thành chuỗi byte
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 5 }; // Định dạng và chất lượng ảnh
    cv::imencode(".jpg", image, buf, params);

    // Gửi kích thước dữ liệu trước
    int size = buf.size();
    send(clientSocket, (char*)&size, sizeof(size), 0);

    // Gửi dữ liệu ảnh
    int bytesSent = send(clientSocket, (char*)buf.data(), size, 0);

    return bytesSent;
}

std::string matToString(const cv::Mat& image) {
    std::stringstream ss;
    ss << image; // Chuyển đổi Mat thành chuỗi

    return ss.str();
}

int Send_Screen(SOCKET clientSocket)
{
    while (1)
    {
        cv::Mat screenshot = Capture_Screen(); // Chup man hinh

        if (!screenshot.empty()) {

            int bytesSend = sendMatOverSocket(screenshot, clientSocket);

            if (bytesSend > 0)
                std::cerr << "Send Successful! BytesSend : " << bytesSend << ".\n";
            else
            {
                std::cerr << "Cannot Send Image!\n";
            }
        }
        else {
            std::cerr << "Error: Could not read the image." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 10));
    }
    return 0;
}

cv::Mat receiveMatFromSocket(SOCKET serverSocket) {
    int size = 0;
    recv(serverSocket, (char*)&size, sizeof(size), 0); // Nhận kích thước dữ liệu

    uchar* data = new uchar[size];
    recv(serverSocket, (char*)data, size, 0); // Nhận dữ liệu ảnh

    cv::Mat image = cv::imdecode(cv::Mat(1, size, CV_8U, data), cv::IMREAD_COLOR); // Chuyển đổi dữ liệu thành cv::Mat

    delete[] data; // Giải phóng bộ nhớ đã cấp phát

    return image;
}

sf::Image matToImage(const cv::Mat& mat) {
    sf::Image image;
    image.create(mat.cols, mat.rows, sf::Color::Black);

    for (int y = 0; y < mat.rows; y++) {
        for (int x = 0; x < mat.cols; x++) {
            cv::Vec3b color = mat.at<cv::Vec3b>(y, x);
            image.setPixel(x, y, sf::Color(color[2], color[1], color[0]));
        }
    }

    return image;
}

sf::Texture imageToTexture(const sf::Image& image) {
    sf::Texture texture;
    texture.loadFromImage(image);
    return texture;
}

int Recv_Screen(SOCKET serverSocket)
{
    sf::RenderWindow window(sf::VideoMode(1920, 1030), "Remote Desktop Client"); // Resolution 1920x1080
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        cv::Mat receivedImage = receiveMatFromSocket(serverSocket);

        std::cerr << "Image Received!\n";
        cv::waitKey(1000 / 12); // Đợi 1/24 giây (của 24 fps)

        sf::Image image = matToImage(receivedImage);
        sf::Texture texture = imageToTexture(image);
        sf::Sprite sprite(texture);

        window.draw(sprite);
        window.display();
    }

    return 1;
}
