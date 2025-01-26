#include "cv.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QFile>
#include <QCoreApplication>

#include "../state.h"
#include "segment.h"

cv::Mat CV::getScreen(Mode mode) {
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


    if (mode == Mode::RGB) {
        return screenshot;
    } else {
        cv::Mat gray;
        cv::cvtColor(screenshot, gray, cv::COLOR_BGR2GRAY);
        return gray;
    }
}

std::vector<Segment> singleFindPositions(
        const cv::Mat &rawImg,
        QFile *imgFile,
        double threshold,
        Mode mode
) {
    cv::Mat templateImg;
    QByteArray byteArray = imgFile->readAll();
    std::vector<char> data(byteArray.data(), byteArray.data() + byteArray.size());

    if (mode == Mode::RGB) {
        templateImg = cv::imdecode(cv::Mat(data), cv::IMREAD_COLOR);
        imgFile->close();
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
            segments.emplace_back(
                    imgFile->fileName().toStdString(),
                    result.at<double>(loc.y, loc.x),
                    w * state.scale,
                    h * state.scale,
                    loc.x * state.scale,
                    loc.y * state.scale
            );
        }
        return segments;
    } else {
        templateImg = cv::imdecode(cv::Mat(data), cv::IMREAD_GRAYSCALE);
        imgFile->close();
        cv::Mat result;
        cv::matchTemplate(rawImg, templateImg, result, cv::TM_CCOEFF_NORMED);
        std::vector<cv::Point> locations;
        cv::findNonZero(result >= threshold, locations);

        std::vector<Segment> segments;
        int w = templateImg.cols;
        int h = templateImg.rows;
        for (const auto &loc: locations) {
            segments.emplace_back(
                    imgFile->fileName().toStdString(),
                    result.at<double>(loc.y, loc.x),
                    w * state.scale,
                    h * state.scale,
                    loc.x * state.scale,
                    loc.y * state.scale
            );
        }
        return segments;
    }
}

std::vector<Segment> CV::findPositions(
        const cv::Mat &rawImg,
        const std::string &templatePath,
        double threshold,
        Mode mode
) {
    auto absolutePath = QCoreApplication::applicationDirPath() + "/res" + QString::fromStdString(templatePath);

    QFile imgFile(absolutePath);

    if (!imgFile.open(QIODevice::ReadOnly)) throw std::runtime_error("文件不存在: " + absolutePath.toStdString());

    auto ps = singleFindPositions(rawImg, &imgFile, threshold, mode);

    if (!ps.empty()) return ps;

    QFile tryImgFile(absolutePath.replace(".png", "1.png"));

    if (!tryImgFile.open(QIODevice::ReadOnly)) return ps;

    return singleFindPositions(rawImg, &tryImgFile, threshold, mode);
}