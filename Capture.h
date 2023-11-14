#pragma once
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include "Header.h"

const int fps = 30;
const int reso = 30;
int slices = 500;

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
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, reso };
    cv::imencode("screen.jpg", image, buf, params);

    int totalSize = buf.size();
    int partSize = totalSize / slices;
    int remainder = totalSize % slices; // Phần dư khi chia

    int bytesSent = 0;

    // Gửi số lần gửi dữ liệu
    send(clientSocket, (char*)&slices, sizeof(slices), 0);

    for (int i = 0; i < slices; ++i) {
        int size = partSize + (i == slices - 1 ? remainder : 0); // Phần cuối cùng có thể cần cộng thêm phần dư

        int offset = i * partSize;

        std::cerr << "Sending slice " << i + 1 << " of size " << size << " bytes\n";

        int oke = 0;
        while (!oke)
        {
            std::cerr << "Cant!\n";
            // Gửi kích thước dữ liệu
            send(clientSocket, (char*)&size, sizeof(size), 0);

            // Gửi dữ liệu ảnh
            int bytes = send(clientSocket, reinterpret_cast<char*>(buf.data()) + offset, size, 0);
            bytesSent += bytes;

            //Phan hoi viec thieu du lieu
            int check_enough_data = 0;
            check_enough_data = recv(clientSocket, (char*)&check_enough_data, sizeof(check_enough_data), 0);
            std::cerr << check_enough_data << '\n';
            if (check_enough_data == 0)
                oke = 0;
            else
                oke = 1;
        }
    }

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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }
    return 0;
}

cv::Mat receiveMatFromSocket(SOCKET serverSocket) {
    bool notOKE = 0;
    int receivedSlices = 0;
    int slices = 0;

    receivedSlices = recv(serverSocket, (char*)&slices, sizeof(slices), 0);
    if (receivedSlices != sizeof(slices)) {
        std::cerr << "Error: Incomplete data received for the number of slices\n";
        // Xử lý lỗi khi không nhận được đúng số lượng byte mong đợi
        return cv::Mat(); // Trả về một Mat rỗng để biểu thị lỗi
    }

    buf.clear();

    for (int i = 0; i < slices; ++i) {
        int size = 0;
        int receivedSize = 0;
        int check_enough_data = 0;
        while (!check_enough_data)
        {
            check_enough_data = 1;
            receivedSize = recv(serverSocket, (char*)&size, sizeof(size), 0);
            if (receivedSize != sizeof(size)) {
                std::cerr << "Error: Incomplete data received for the image size\n";
                // Xử lý lỗi khi không nhận được đúng kích thước dữ liệu mong đợi
                bool notOKE = 1;
            }

            std::vector<uchar> tempBuf(size);

            int receivedImageData = recv(serverSocket, (char*)tempBuf.data(), size, 0);

            std::cerr << "Error: Incomplete image data received! Received size = " << receivedImageData << " ; Size = " << size << "\n";

            if (receivedImageData != size) {
                std::cerr << "Error: Incomplete image data received! Received size = " << receivedImageData << " ; Size = " << size << "\n";
                // Xử lý lỗi khi không nhận được đủ dữ liệu ảnh mong đợi
                notOKE = 1;
                check_enough_data = 0;
            }
            send(serverSocket, (char*)&check_enough_data, sizeof(check_enough_data), 0);
            if (!check_enough_data)
                continue;
            buf.insert(buf.end(), tempBuf.begin(), tempBuf.end()); // Nối dữ liệu từ từng phần vào vector buf
            std::cerr << "Slice number " << i << " received!\n";
        }
    }

    /*if (notOKE)
    {
        cv::Mat image = cv::imread("Image/Image1.jpg");
        return image;
    }*/

    std::cerr << "All slices received!\n";

    cv::Mat image = cv::imdecode(buf, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error: Failed to create Mat from received data\n";
        // Xử lý lỗi khi không thể tạo Mat từ dữ liệu nhận được
    }



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
