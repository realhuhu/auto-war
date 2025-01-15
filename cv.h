#ifndef QT_CV_H
#define QT_CV_H

#include <windows.h>

#include <QFile>
#include <QString>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#include "state.h"
#include "segment.h"

// 获取屏幕图像函数
cv::Mat getScreen(const std::string &mode = "gray") {
    SetProcessDPIAware();
    RECT rect;
    GetWindowRect(state.hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    HDC hwndDC = GetWindowDC(state.hwnd);
    HDC memDC = CreateCompatibleDC(hwndDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hwndDC, width, height);
    SelectObject(memDC, hBitmap);

    PrintWindow(state.hwnd, memDC, 3);

    BITMAP bmpInfo;
    GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);
    std::vector<BYTE> bmpData(bmpInfo.bmWidthBytes * bmpInfo.bmHeight);
    GetBitmapBits(hBitmap, bmpInfo.bmWidthBytes * bmpInfo.bmHeight, bmpData.data());

    cv::Mat capture(height, width, CV_8UC4, bmpData.data());
    cv::Mat screenshot;
    cv::cvtColor(capture, screenshot, cv::COLOR_BGRA2BGR);

    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(state.hwnd, hwndDC);

    if (mode == "rgb") {
        return screenshot;
    } else {
        cv::Mat gray;
        cv::cvtColor(screenshot, gray, cv::COLOR_BGRA2GRAY);
        return gray;
    }
}

// 查找模板位置函数
std::vector<Segment> findPositions(
        const cv::Mat &rawImg,
        const std::string &templatePath,
        double threshold = 0.9,
        const std::string &mode = "gray"
) {
    cv::Mat templateImg;

    cv::Mat Image1;
    QFile file(QString::fromStdString(templatePath));

    if (!file.open(QIODevice::ReadOnly)) {
    }

    QByteArray byteArray = file.readAll();
    std::vector<char> data(byteArray.data(), byteArray.data() + byteArray.size());

    if (mode == "rgb") {
        templateImg = cv::imdecode(cv::Mat(data), cv::IMREAD_COLOR);
        file.close();
        std::vector<cv::Mat> templateChannels, rawChannels;
        cv::split(templateImg, templateChannels);
        cv::split(rawImg, rawChannels);

        cv::Mat resultsB, resultsG, resultsR;
        cv::matchTemplate(rawChannels[0], templateChannels[0], resultsB, cv::TM_CCOEFF_NORMED);
        cv::matchTemplate(rawChannels[1], templateChannels[1], resultsG, cv::TM_CCOEFF_NORMED);
        cv::matchTemplate(rawChannels[2], templateChannels[2], resultsR, cv::TM_CCOEFF_NORMED);

        cv::Mat result = (resultsB + resultsG + resultsR) / 3.0;
        std::vector<cv::Point> locations;
        cv::findNonZero(result >= threshold, locations);

        std::vector<Segment> segments;
        int w = templateImg.cols;
        int h = templateImg.rows;
        for (const auto &loc: locations) {
            segments.emplace_back(templatePath, result.at<double>(loc.y, loc.x), w, h, loc.x, loc.y);
        }
        return segments;
    } else {
        templateImg = cv::imdecode(cv::Mat(data), cv::IMREAD_GRAYSCALE);
        file.close();
        cv::Mat result;
        cv::matchTemplate(rawImg, templateImg, result, cv::TM_CCOEFF_NORMED);
        std::vector<cv::Point> locations;
        cv::findNonZero(result >= threshold, locations);

        std::vector<Segment> segments;
        int w = templateImg.cols;
        int h = templateImg.rows;
        for (const auto &loc: locations) {
            segments.emplace_back(templatePath, result.at<double>(loc.y, loc.x), w, h, loc.x, loc.y);
        }
        return segments;
    }

}

#endif //QT_CV_H