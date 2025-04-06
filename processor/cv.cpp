#include "cv.h"

cv::Mat CV::getScreen(HWND hwnd, Mode mode) {
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    HDC hwndDC = GetWindowDC(hwnd);
    HDC memDC = CreateCompatibleDC(hwndDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hwndDC, width, height);
    SelectObject(memDC, hBitmap);

    PrintWindow(hwnd, memDC, 3);

    BITMAP bmpInfo;
    GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);
    std::vector<BYTE> bmpData(bmpInfo.bmWidthBytes * bmpInfo.bmHeight);
    GetBitmapBits(hBitmap, bmpInfo.bmWidthBytes * bmpInfo.bmHeight, bmpData.data());

    cv::Mat capture(height, width, CV_8UC4, bmpData.data());
    cv::Mat screenshot;
    cv::cvtColor(capture, screenshot, cv::COLOR_BGRA2BGR);

    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(hwnd, hwndDC);

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

    std::vector<Segment> segments;
    std::vector<cv::Point> locations;

    cv::Mat result;
    if (mode == Mode::RGB) {
        templateImg = cv::imdecode(cv::Mat(data), cv::IMREAD_COLOR);

        cv::Mat rawImgHsv, templateImgHsv;
        cvtColor(rawImg, rawImgHsv, cv::COLOR_BGR2HSV);
        cvtColor(templateImg, templateImgHsv, cv::COLOR_BGR2HSV);

        std::vector<cv::Mat> templateChannels, rawChannels, templateHsvChannels, rawHsvChannels;
        cv::split(templateImg, templateChannels);
        cv::split(rawImg, rawChannels);
        split(templateImgHsv, templateHsvChannels);
        split(rawImgHsv, rawHsvChannels);

        cv::Mat resultsB, resultsG, resultsR, resultH, resultS, resultV;
        cv::matchTemplate(rawChannels[0], templateChannels[0], resultsB, cv::TM_CCOEFF_NORMED);
        cv::matchTemplate(rawChannels[1], templateChannels[1], resultsG, cv::TM_CCOEFF_NORMED);
        cv::matchTemplate(rawChannels[2], templateChannels[2], resultsR, cv::TM_CCOEFF_NORMED);
        matchTemplate(rawHsvChannels[0], templateHsvChannels[0], resultH, cv::TM_CCOEFF_NORMED);
        matchTemplate(rawHsvChannels[1], templateHsvChannels[1], resultS, cv::TM_CCOEFF_NORMED);
        matchTemplate(rawHsvChannels[2], templateHsvChannels[2], resultV, cv::TM_CCOEFF_NORMED);

        result = (resultsB + resultsG + resultsR + resultH + resultS + resultV) / 6;
    } else {
        templateImg = cv::imdecode(cv::Mat(data), cv::IMREAD_GRAYSCALE);
        cv::matchTemplate(rawImg, templateImg, result, cv::TM_CCOEFF_NORMED);
    }

    cv::findNonZero(result >= threshold, locations);
    segments.reserve(locations.size());
    for (const auto &loc: locations) {
        segments.emplace_back(
                imgFile->fileName(),
                result.at<float>(loc.y, loc.x),
                static_cast<float >(templateImg.cols),
                static_cast<float >(templateImg.rows),
                static_cast<float >(loc.x),
                static_cast<float >(loc.y)
        );
    }
    return segments;
}

std::vector<Segment> CV::findPositions(
        const cv::Mat &rawImg,
        const QString &templatePath,
        float threshold,
        Mode mode
) {
    QFile imgFile(res(templatePath));
    qDebug() << "3" << imgFile.fileName();
    if (!imgFile.open(QIODevice::ReadOnly)) throw std::runtime_error("文件不存在: " + imgFile.fileName().toStdString());

    auto ps = singleFindPositions(rawImg, &imgFile, threshold, mode);
    if (!ps.empty()) return ps;

    QFile customImgFile(res(QString(templatePath).replace("/", "-"), "自定义图片"));
    customImgFile.open(QIODevice::ReadOnly);
    if (customImgFile.isOpen()) {
        ps = singleFindPositions(rawImg, &customImgFile, threshold, mode);
        if (!ps.empty()) return ps;
    }

    return {};
}