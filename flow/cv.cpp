#include "cv.h"

cv::Mat CV::getScreen(Mode mode) {
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

    if (mode == Mode::RGB) return screenshot;

    cv::Mat gray;
    cv::cvtColor(screenshot, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

std::vector<Segment> singleFindPositions(
        const cv::Mat &rawImg,
        QFile *imgFile,
        float threshold,
        Mode mode
) {
    cv::Mat templateImg;
    QByteArray byteArray = imgFile->readAll();
    imgFile->close();
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
                    result.at<float>(loc.y, loc.x),
                    static_cast<float >(w) * state.scale,
                    static_cast<float >(h) * state.scale,
                    static_cast<float >(loc.x) * state.scale,
                    static_cast<float >(loc.y) * state.scale
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
                    result.at<float>(loc.y, loc.x),
                    static_cast<float >(w) * state.scale,
                    static_cast<float >(h) * state.scale,
                    static_cast<float >(loc.x) * state.scale,
                    static_cast<float >(loc.y) * state.scale
            );
        }
        return segments;
    }
}

std::vector<Segment> CV::findPositions(
        const cv::Mat &rawImg,
        const std::string &templatePath,
        float threshold,
        Mode mode
) {
    QFile imgFile(QCoreApplication::applicationDirPath() + "/res" + QString::fromStdString(templatePath));
    if (!imgFile.open(QIODevice::ReadOnly)) throw std::runtime_error("文件不存在: " + imgFile.fileName().toStdString());

    auto ps = singleFindPositions(rawImg, &imgFile, threshold, mode);
    if (!ps.empty()) return ps;

    auto tryPath = QString::fromStdString(templatePath).replace(".png", "1.png");
    auto customPath = QString::fromStdString(templatePath).mid(1).replace("/", "-");
    QFile tryImgFile(QCoreApplication::applicationDirPath() + "/res" + tryPath);
    QFile customImgFile(QCoreApplication::applicationDirPath() + "/自定义图片/" + customPath);

    if (!tryImgFile.open(QIODevice::ReadOnly) && !customImgFile.open(QIODevice::ReadOnly)) return ps;

    if (tryImgFile.isOpen()) {
        ps = singleFindPositions(rawImg, &tryImgFile, threshold, mode);
        if (!ps.empty()) return ps;
    }

    if (customImgFile.isOpen()) {
        ps = singleFindPositions(rawImg, &customImgFile, threshold, mode);
        if (!ps.empty()) return ps;
    }

    return {};
}

bool CV::hasBlack(const cv::Mat &binaryImage) {
    int rows = binaryImage.rows;
    int cols = binaryImage.cols;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (binaryImage.at<uchar>(y, x) == 0) return true;
        }
    }

    return false;
}

bool CV::identical(const cv::Mat &binaryImage1, const cv::Mat &binaryImage2) {
    CV_Assert(binaryImage1.size() == binaryImage2.size() && binaryImage1.type() == binaryImage2.type());

    for (int y = 0; y < binaryImage1.rows; ++y) {
        for (int x = 0; x < binaryImage1.cols; ++x) {
            if (binaryImage1.at<uchar>(y, x) != binaryImage2.at<uchar>(y, x)) return false;
        }
    }

    return true;
}
