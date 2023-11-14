#pragma once
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include "Header.h"

const int fps = 12;
const int reso = 2;
int slices = 5;

std::vector<uchar> buf;
std::vector<int> params;

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
    // Chuyển đổi ma trận hình ảnh thành mảng byte để gửi đi
    std::vector<uchar> buffer;
    cv::imencode(".jpg", image, buffer);

    // Gửi kích thước của dữ liệu hình ảnh
    int imgSize = buffer.size();
    if (send(clientSocket, reinterpret_cast<char*>(&imgSize), sizeof(imgSize), 0) == SOCKET_ERROR) {
        // Xử lý lỗi khi gửi kích thước dữ liệu
        return -1;
    }

    // Gửi dữ liệu hình ảnh qua socket
    int bytesSent = send(clientSocket, reinterpret_cast<char*>(buffer.data()), imgSize, 0);
    if (bytesSent == SOCKET_ERROR || bytesSent != imgSize) {
        // Xử lý lỗi khi gửi dữ liệu hình ảnh
        return -1;
    }

    return 0; // Gửi thành công
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }
    return 0;
}

cv::Mat receiveMatFromSocket(SOCKET serverSocket) {
    int imgSize = 0;
    if (recv(serverSocket, reinterpret_cast<char*>(&imgSize), sizeof(imgSize), 0) == SOCKET_ERROR) {
        // Xử lý lỗi khi nhận kích thước dữ liệu hình ảnh
        return cv::Mat(); // Trả về một ma trận rỗng
    }

    std::vector<uchar> buffer(imgSize);
    int bytesReceived = recv(serverSocket, reinterpret_cast<char*>(buffer.data()), imgSize, 0);
    if (bytesReceived == SOCKET_ERROR || bytesReceived != imgSize) {
        // Xử lý lỗi khi nhận dữ liệu hình ảnh
        return cv::Mat(); // Trả về một ma trận rỗng
    }

    cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR); // Tạo ma trận hình ảnh từ dữ liệu nhận được

    return image; // Trả về ma trận hình ảnh đã nhận
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

cv::Mat receivedImage;

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

        receivedImage = receiveMatFromSocket(serverSocket);

        std::cerr << "Image Received!\n";
        cv::waitKey(1000 / fps); // Đợi 1/24 giây (của 24 fps)

        sf::Image image = matToImage(receivedImage);        std::cerr << 2 << '\n';
        sf::Texture texture = imageToTexture(image);        std::cerr << 3 << '\n';
        sf::Sprite sprite(texture);                         std::cerr << 4 << '\n';

        window.draw(sprite);                                std::cerr << 5 << '\n';
        window.display();
    }

    return 1;
}
