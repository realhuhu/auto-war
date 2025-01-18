// until.cpp
#include "until.h"

#include <chrono>
#include <thread>
#include <stdexcept>
#include <algorithm>
#include <utility>


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

        if (elapsed > maxTime) {
            throw std::runtime_error("超时，结束运行: " + this->toString());
        }

        bool fulfilled = this->fulfilled(previous);
        if (fulfilled) {
            if (finishWait > 0) {
                std::this_thread::sleep_for(std::chrono::duration<float>(finishWait));
            }
            break;
        }

        std::this_thread::sleep_for(std::chrono::duration<float>(interval));
    }
}

bool Until::fulfilled(std::unique_ptr<Segment> &previous) {
    pre_hook(previous);
    bool is_fulfilled = reverse == !flag(previous);
    emit Emitter::instance()->log(
            QString::fromStdString((is_fulfilled ? "条件已满足: " : "条件未满足: ") + this->toString()));
    return is_fulfilled;
}

std::vector<Segment> Until::filter(const std::vector<Segment> &positions, std::unique_ptr<Segment> &previous) const {
    if (onPrevious == "none") {
        return positions;
    }

    if (!previous) {
        throw std::runtime_error("Previous segment为空" + this->toString());
    }

    std::vector<Segment> result;

    emit Emitter::instance()->log(QString::fromStdString("筛选: on " + previous->toString() + " " + onPrevious));

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

    return result;
}

Until Until::operator~() {
    reverse = true;
    return *this;
}

void Until::pre_hook(std::unique_ptr<Segment> &previous) {}

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
    std::vector<Segment> matchedPositions = CV::find_positions(
            CV::get_screen(mode),
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
    return "[等待图片 " + std::filesystem::path(imgPath).stem().string() + "]";
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

bool UntilAnyImage::flag(std::unique_ptr<Segment> &previous) {
    cv::Mat screen = CV::get_screen(mode);

    for (const auto &currentImgPath: imgPathList) {
        std::vector<Segment> matchedPositions = CV::find_positions(screen, currentImgPath, threshold, mode);

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
    std::string img_stems;
    for (const auto &currentImgPath: imgPathList) {
        img_stems += std::filesystem::path(currentImgPath).stem().string() + "|";
    }
    if (!img_stems.empty()) {
        img_stems.pop_back();
    }
    return "[等待任意图片 " + img_stems + "]";
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

void UntilImageStable::pre_hook(std::unique_ptr<Segment> &previous) {
    std::vector<Segment> matchedPositions = CV::find_positions(
            CV::get_screen(mode),
            imgPath,
            threshold,
            mode
    );
    std::vector<Segment> filteredPositions = filter(matchedPositions, previous);

    if (filteredPositions.empty()) {
        return;
    }

    Segment position = similarity_selector(filteredPositions);
    positions.push_back(position);

    if (positions.size() > 3) {
        positions.erase(positions.begin());
    }
}

bool UntilImageStable::flag(std::unique_ptr<Segment> &previous) {
    if (positions.size() != 3) {
        return false;
    }

    std::vector<int> x;
    std::vector<int> y;
    for (const auto &pos: positions) {
        x.push_back(pos.x_center);
        y.push_back(pos.y_center);
    }

    if (*std::max_element(x.begin(), x.end()) != *std::min_element(x.begin(), x.end()) ||
        *std::max_element(y.begin(), y.end()) != *std::min_element(y.begin(), y.end())) {
        return false;
    }

    targetSegmentList = {positions[0]};
    return true;
}

std::string UntilImageStable::toString() const {
    return "[等待图片稳定 " + std::filesystem::path(imgPath).stem().string() + "]";
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
    return "[尝试等待图片 " + std::filesystem::path(imgPath).stem().string() + "]";
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

void UntilIfAnyImage::loop(std::unique_ptr<Segment> &previous, int globalTimeout) {
    this->fulfilled(previous);
}

std::string UntilIfAnyImage::toString() const {
    std::string img_stems;
    for (const auto &currentImgPath: imgPathList) {
        img_stems += std::filesystem::path(currentImgPath).stem().string() + "|";
    }
    if (!img_stems.empty()) {
        img_stems.pop_back();
    }
    return "[尝试等待任意图片 " + img_stems + "]";
}


void clear_until(
        std::vector<std::unique_ptr<Until>> &start_until,
        std::vector<std::unique_ptr<Until>> &click_until,
        std::vector<std::unique_ptr<Until>> &run_until
) {
    start_until.clear();
    click_until.clear();
    run_until.clear();
}