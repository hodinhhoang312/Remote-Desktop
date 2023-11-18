#pragma once
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include "Header.h"

const int reso = 80;
int resize = 80;

std::vector<uchar> buf;
std::vector<int> params;

cv::Mat Capture_Screen() {
    // Lấy handle của màn hình chính
    HWND hDesktop = GetDesktopWindow();

    // Lấy thông tin về màn hình
    RECT desktopRect;
    GetClientRect(hDesktop, &desktopRect);
    int width = desktopRect.right;
    int height = desktopRect.bottom;

    // Tạo DC (Device Context) cho màn hình
    HDC hDC = GetDC(hDesktop);

    // Tạo một bitmap để lưu dữ liệu màn hình
    HDC hCaptureDC = CreateCompatibleDC(hDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
    SelectObject(hCaptureDC, hBitmap);

    // Chụp màn hình và sao chép dữ liệu vào bitmap
    BitBlt(hCaptureDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);

    // Tạo đối tượng Mat từ dữ liệu bitmap
    BITMAPINFOHEADER bmpInfoHeader;
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biWidth = width;
    bmpInfoHeader.biHeight = -height; // Negative height to ensure correct order
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = 24; // 24-bit RGB
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biSizeImage = 0;

    // Tạo Mat để lưu trữ dữ liệu hình ảnh
    cv::Mat screenshot(height, width, CV_8UC3);
    GetDIBits(hDC, hBitmap, 0, height, screenshot.data, (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);

    // Giải phóng bộ nhớ
    DeleteObject(hBitmap);
    DeleteDC(hCaptureDC);
    ReleaseDC(hDesktop, hDC);

    return screenshot;
}

void reduceSize(cv::Mat& image, float percent) {
    // Kiểm tra xem hình ảnh có trống không
    if (image.empty()) {
        std::cout << "Hình ảnh trống!" << std::endl;
        return;
    }

    // Tính toán kích thước mới dựa trên tỷ lệ phần trăm
    int newWidth = static_cast<int>(image.cols * (percent / 100));
    int newHeight = static_cast<int>(image.rows * (percent / 100));

    // Thay đổi kích thước hình ảnh
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(newWidth, newHeight));

    // Gán hình ảnh đã thay đổi vào tham số đầu vào
    image = resizedImage;
}


int sendMatOverSocket(const cv::Mat& image, SOCKET clientSocket) {
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, reso };
    cv::imencode("screen.jpg", image, buf, params);

    int totalSize = buf.size();

    // Gửi kích thước dữ liệu
    if (send(clientSocket, (char*)&totalSize, sizeof(totalSize), 0) == SOCKET_ERROR) {
        return -1; // Xử lý lỗi khi gửi kích thước dữ liệu
    }

    // Gửi toàn bộ dữ liệu ảnh trong một gói
    int bytesSent = send(clientSocket, reinterpret_cast<char*>(buf.data()), totalSize, 0);

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

        reduceSize(screenshot, resize);

        if (!screenshot.empty()) {

            int bytesSend = sendMatOverSocket(screenshot, clientSocket);

            if (bytesSend > 0)
                std::cerr << "Send Successful! BytesSend : " << bytesSend << ".\n";
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
    return 0;
}

cv::Mat receiveMatFromSocket(SOCKET serverSocket) {
    int totalSize = 0;

    // Nhận kích thước dữ liệu
    if (recv(serverSocket, (char*)&totalSize, sizeof(totalSize), 0) == SOCKET_ERROR) {
        // Xử lý lỗi khi nhận kích thước dữ liệu
        return cv::Mat(); // Trả về một ma trận rỗng
    }

    std::vector<uchar> buffer;
    buffer.reserve(totalSize);

    int bytesReceived = 0;
    while (bytesReceived < totalSize) {
        int remaining = totalSize - bytesReceived;
        int chunkSize = remaining > 4096 ? 4096 : remaining; // Kích thước chunk mỗi lần nhận

        std::vector<uchar> tempBuffer(chunkSize);
        int received = recv(serverSocket, reinterpret_cast<char*>(tempBuffer.data()), chunkSize, 0);

        if (received <= 0) {
            // Xử lý lỗi hoặc kết thúc kết nối
            return cv::Mat(); // Trả về một ma trận rỗng
        }

        bytesReceived += received;
        buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.begin() + received);
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

int Recv_Screen(SOCKET serverSocket, sf::RenderWindow &window)
{
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::Escape)
                    return 1;
        }

        window.clear();

        receivedImage = receiveMatFromSocket(serverSocket);

        std::cerr << "Image Received!\n";

        sf::Image image = matToImage(receivedImage);
        sf::Texture texture = imageToTexture(image);
        sf::Sprite sprite(texture);

        window.draw(sprite);
        window.display();
    }

    return 1;
}