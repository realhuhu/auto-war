#include "until.h"

#include <chrono>
#include <thread>
#include <utility>
#include <stdexcept>
#include <algorithm>


#include "../state.h"
#include "cv.h"
#include "segment.h"
#include "emitter.h"

Until::Until(
        double threshold,
        std::string onPrevious,
        double interval,
        double finishWait,
        double timeout,
        bool reverse,
        std::string mode
) : threshold(threshold),
    onPrevious(std::move(onPrevious)),
    interval(interval),
    timeout(timeout),
    finishWait(finishWait),
    reverse(reverse),
    mode(std::move(mode)) {}

void Until::loop(std::unique_ptr<Segment> &previous, int globalTimeout) {
    auto start = std::chrono::high_resolution_clock::now();
    auto maxTime = (timeout == -1) ? globalTimeout : timeout;

    while (!state.stopFlag.load()) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(now - start).count();

        if (elapsed > maxTime) throw std::runtime_error("超时，结束运行: " + this->toString());

        bool fulfilled = this->fulfilled(previous);
        if (fulfilled) {
            if (finishWait > 0) std::this_thread::sleep_for(std::chrono::duration<float>(finishWait));
            break;
        }

        std::this_thread::sleep_for(std::chrono::duration<float>(interval));
    }
}

bool Until::fulfilled(std::unique_ptr<Segment> &previous) {
    preHook(previous);
    bool isFulfilled = reverse == !flag(previous);
    emit Emitter::instance()->log(
            QString::fromStdString((isFulfilled ? "条件满足: " : "条件未满足: ") + this->toString()));
    return isFulfilled;
}

std::vector<Segment> Until::filter(const std::vector<Segment> &positions, std::unique_ptr<Segment> &previous) const {
    if (onPrevious == "none") return positions;

    if (!previous) throw std::runtime_error("Previous segment为空" + this->toString());

    std::vector<Segment> result;

    emit Emitter::instance()->log(QString::fromStdString("筛选: 在 " + previous->toString() + " " + onPrevious));

    if (onPrevious == "left") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "vertical") == "left")) continue;
            result.push_back(position);
        }
    } else if (onPrevious == "right") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "vertical") == "right")) continue;
            result.push_back(position);
        }
    }
    if (onPrevious == "top") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "horizontal") == "top")) continue;
            result.push_back(position);
        }
    }
    if (onPrevious == "down") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "horizontal") == "down")) continue;
            result.push_back(position);
        }
    }
    if (onPrevious == "top_center") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "horizontal") == "top")) continue;
            if (!(position.on(*previous, "vertical") == "center")) continue;
            result.push_back(position);
        }
    }
    if (onPrevious == "down_center") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "horizontal") == "down")) continue;
            if (!(position.on(*previous, "vertical") == "center")) continue;
            result.push_back(position);
        }
    }
    if (onPrevious == "inner") {
        for (const auto &position: positions) {
            if (!(position.on(*previous, "horizontal") == "center")) continue;
            if (!(position.on(*previous, "vertical") == "center")) continue;
            result.push_back(position);
        }
    }

    emit Emitter::instance()->log(
            "筛选前: " + QString::number(positions.size()) +
            ", 筛选后: " + QString::number(result.size())
    );

    return result;
}

Until Until::operator~() {
    reverse = true;
    return *this;
}

void Until::preHook(std::unique_ptr<Segment> &previous) {}

bool Until::flag(std::unique_ptr<Segment> &previous) {
    return false;
}

std::string Until::toString() const {
    return "[Until" + std::filesystem::path(imgPath).stem().string() + "]";
}

UntilImage::UntilImage(
        const std::string &imgPath,
        const std::string &onPrevious,
        bool reverse,
        std::string mode,
        double finishWait,
        double threshold,
        double interval,
        double timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
    this->imgPath = imgPath;
}

bool UntilImage::flag(std::unique_ptr<Segment> &previous) {
    std::vector<Segment> matchedPositions = CV::findPositions(
            CV::getScreen(mode),
            imgPath,
            threshold,
            mode
    );

    std::vector<Segment> filteredPositions = filter(matchedPositions, previous);

    if (!filteredPositions.empty()) {
        targetSegmentList = filteredPositions;
        return true;
    }


    return false;
}

std::string UntilImage::toString() const {
    return QString(
            "等待%1<img src='%2' alt='%3' height='14'>"
    ).arg(
            QString(reverse ? "消失" : ""),
            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + imgPath),
            QString::fromStdString(std::filesystem::path(imgPath).stem().string())
    ).toStdString();
}


UntilAnyImage::UntilAnyImage(
        const std::initializer_list<const std::string> &imgList,
        const std::string &onPrevious,
        bool reverse,
        std::string mode,
        double finishWait,
        double threshold,
        double interval,
        double timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
    for (const auto &i: imgList) {
        imgPathList.push_back(i);
    }
}

UntilAnyImage::UntilAnyImage(
        const std::vector<std::string> &imgList,
        const std::string &onPrevious,
        bool reverse,
        std::string mode,
        double finishWait,
        double threshold,
        double interval,
        double timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
    for (const auto &i: imgList) {
        imgPathList.push_back(i);
    }
}

bool UntilAnyImage::flag(std::unique_ptr<Segment> &previous) {
    cv::Mat screen = CV::getScreen(mode);

    for (const auto &currentImgPath: imgPathList) {
        std::vector<Segment> matchedPositions = CV::findPositions(screen, currentImgPath, threshold, mode);

        std::vector<Segment> filteredPositions = filter(matchedPositions, previous);

        if (!filteredPositions.empty()) {
            imgPath = currentImgPath;
            targetSegmentList = filteredPositions;
            return true;
        }
    }
    return false;
}

std::string UntilAnyImage::toString() const {
    auto start = QString("等待|");

    for (const auto &currentImgPath: imgPathList) {
        start += QString("<img src='%1' alt='%2' height='14'>|").arg(
                QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + currentImgPath),
                QString::fromStdString(std::filesystem::path(currentImgPath).stem().string())
        );
    }

    return start.toStdString();
}


UntilImageStable::UntilImageStable(
        std::string imgPath,
        const std::string &onPrevious,
        bool reverse,
        std::string mode,
        double finishWait,
        double threshold,
        double interval,
        double timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
    this->imgPath = std::move(imgPath);
}

void UntilImageStable::preHook(std::unique_ptr<Segment> &previous) {
    std::vector<Segment> matchedPositions = CV::findPositions(
            CV::getScreen(mode),
            imgPath,
            threshold,
            mode
    );
    std::vector<Segment> filteredPositions = filter(matchedPositions, previous);

    if (filteredPositions.empty()) return;

    Segment position = similaritySelector(filteredPositions);
    positions.push_back(position);

    if (positions.size() > 3) positions.erase(positions.begin());
}

bool UntilImageStable::flag(std::unique_ptr<Segment> &previous) {
    if (positions.size() != 3) return false;

    std::vector<int> x;
    std::vector<int> y;
    for (const auto &pos: positions) {
        x.push_back(pos.xCenter);
        y.push_back(pos.yCenter);
    }

    if (*std::max_element(x.begin(), x.end()) != *std::min_element(x.begin(), x.end()) ||
        *std::max_element(y.begin(), y.end()) != *std::min_element(y.begin(), y.end())) {
        return false;
    }

    targetSegmentList = {positions[0]};
    return true;
}

std::string UntilImageStable::toString() const {
    return QString(
            "等待稳定<img src='%1' alt='%2' height='14'>"
    ).arg(
            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + imgPath),
            QString::fromStdString(std::filesystem::path(imgPath).stem().string())
    ).toStdString();
}


UntilIfImage::UntilIfImage(
        const std::string &imgPath,
        const std::string &onPrevious,
        bool reverse, std::string mode,
        double finishWait,
        double threshold,
        double interval,
        double timeout
) : UntilImage(imgPath, onPrevious, reverse, std::move(mode), finishWait, threshold, interval, timeout) {}

void UntilIfImage::loop(std::unique_ptr<Segment> &previous, int globalTimeout) {
    this->fulfilled(previous);
}

std::string UntilIfImage::toString() const {
    return QString(
            "尝试等待<img src='%1' alt='%2' height='14'>"
    ).arg(
            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + imgPath),
            QString::fromStdString(std::filesystem::path(imgPath).stem().string())
    ).toStdString();
}


UntilIfAnyImage::UntilIfAnyImage(
        const std::initializer_list<const std::string> &imgList,
        const std::string &onPrevious,
        bool reverse,
        std::string mode,
        double finishWait,
        double threshold,
        double interval,
        double timeout
) : UntilAnyImage(imgList, onPrevious, reverse, std::move(mode), finishWait, threshold, interval, timeout) {}

UntilIfAnyImage::UntilIfAnyImage(
        const std::vector<std::string> &imgList,
        const std::string &onPrevious,
        bool reverse,
        std::string mode,
        double finishWait,
        double threshold,
        double interval, double
        timeout
) : UntilAnyImage(imgList, onPrevious, reverse, std::move(mode), finishWait, threshold, interval, timeout) {}

void UntilIfAnyImage::loop(std::unique_ptr<Segment> &previous, int globalTimeout) {
    this->fulfilled(previous);
}

std::string UntilIfAnyImage::toString() const {
    auto start = QString("尝试等待|");

    for (const auto &currentImgPath: imgPathList) {
        start += QString("<img src='%1' alt='%2' height='14'>|").arg(
                QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + currentImgPath),
                QString::fromStdString(std::filesystem::path(currentImgPath).stem().string())
        );
    }

    return start.toStdString();
}


void clearUntil(
        std::vector<std::unique_ptr<Until>> &startUntil,
        std::vector<std::unique_ptr<Until>> &clickUntil,
        std::vector<std::unique_ptr<Until>> &runUntil
) {
    startUntil.clear();
    clickUntil.clear();
    runUntil.clear();
}