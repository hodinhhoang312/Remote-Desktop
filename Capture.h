#pragma once
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "Header.h"
#include <cwchar>

const int reso = 30;
float resize = 80;

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
