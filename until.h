#ifndef QT_UNTIL_H
#define QT_UNTIL_H

#include <vector>
#include <chrono>
#include <iostream>

#include <QDebug>

#include "cv.h"
#include "state.h"
#include "runner.h"
#include "segment.h"


class Until {
public:
    std::string imgPath;

    double threshold;
    std::string onPrevious;
    double interval;
    double timeout;
    double finishWait;
    bool reverse;
    std::string mode;
    std::vector<Segment> targetSegmentList;

    explicit Until(
            double threshold = 0.9,
            std::string onPrevious = "none",
            double interval = 0.1,
            double finishWait = 0,
            double timeout = -1,
            bool reverse = false,
            std::string mode = "gray"
    ) : threshold(threshold),
        onPrevious(std::move(onPrevious)),
        interval(interval),
        timeout(timeout),
        finishWait(finishWait),
        reverse(reverse),
        mode(std::move(mode)) {}

    void loop(std::unique_ptr<Segment> &previous, int globalTimeout) {
        auto start = std::chrono::high_resolution_clock::now();
        auto maxTime = (timeout == -1) ? globalTimeout : timeout;

        while (!state.stopFlag.load()) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>(now - start).count();

            if (elapsed > maxTime) {
                throw std::runtime_error("timeout" + this->toString());
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


    bool fulfilled(std::unique_ptr<Segment> &previous) {
        pre_hook(previous);
        bool is_fulfilled = reverse == !flag(previous);
        qDebug() << QString::fromStdString((is_fulfilled ? "条件已满足: " : "条件未满足: ") + this->toString());
        return is_fulfilled;
    }

    [[nodiscard]] std::vector<Segment>
    filter(const std::vector<Segment> &positions, std::unique_ptr<Segment> &previous) const {
        if (onPrevious == "none") {
            return positions;
        }


        if (!previous) {
            throw std::runtime_error("No previous segment" + this->toString());
        }

        std::vector<Segment> result;

        qDebug() << QString::fromStdString("筛选: on " + previous->toString() + " " + onPrevious);

        for (const auto &position: positions) {
            if (onPrevious == "left" && position.on(*previous, "vertical") == "left") {
                result.push_back(position);
            } else if (onPrevious == "right" && position.on(*previous, "vertical") == "right") {
                result.push_back(position);
            } else if (onPrevious == "top" && position.on(*previous, "horizontal") == "top") {
                result.push_back(position);
            } else if (onPrevious == "top_center" && position.on(*previous, "horizontal") == "top" &&
                       position.on(*previous, "vertical") == "center") {
                result.push_back(position);
            } else if (onPrevious == "down" && position.on(*previous, "horizontal") == "down") {
                result.push_back(position);
            } else if (onPrevious == "down_center" && position.on(*previous, "horizontal") == "down" &&
                       position.on(*previous, "vertical") == "center") {
                result.push_back(position);
            } else if (onPrevious == "inner" && position.on(*previous, "horizontal") == "center" &&
                       position.on(*previous, "vertical") == "center") {
                result.push_back(position);
            }
        }
        return result;
    }

    Until operator~() {
        reverse = true;
        return *this;
    }

    virtual void pre_hook(std::unique_ptr<Segment> &previous) {}

    virtual bool flag(std::unique_ptr<Segment> &previous) {}

    [[nodiscard]] virtual std::string toString() const {
        return "[Until" + std::filesystem::path(imgPath).stem().string() + "]";
    }

    [[nodiscard]] virtual std::string toRepr() const {
        return toString();
    }
};

class UntilImage : public Until {
public:
    explicit UntilImage(
            const std::string &imgPath,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    ) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
        this->imgPath = imgPath;
    }

    bool flag(std::unique_ptr<Segment> &previous) override {
        std::vector<Segment> matchedPositions = findPositions(
                getScreen(mode),
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

    [[nodiscard]] std::string toString() const override {
        return "[等待图片 " + std::filesystem::path(imgPath).stem().string() + "]";

    }

    [[nodiscard]] std::string toRepr() const override {
        return toString();
    }
};

class UntilAnyImage : public Until {
public:
    std::vector<std::string> imgPathList;

    UntilAnyImage(
            const std::initializer_list<const std::string> &imgList,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    ) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
        for (const auto &i: imgList) {
            imgPathList.push_back(i);
        }
    }

    bool flag(std::unique_ptr<Segment> &previous) override {
        cv::Mat screen = getScreen(mode);

        for (const auto &currentImgPath: imgPathList) {
            std::vector<Segment> matchedPositions = findPositions(screen, currentImgPath, threshold, mode);

            std::vector<Segment> filteredPositions = filter(matchedPositions, previous);

            if (!filteredPositions.empty()) {
                imgPath = currentImgPath;
                targetSegmentList = filteredPositions;
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::string toString() const override {
        std::string img_stems;
        for (const auto &currentImgPath: imgPathList) {
            img_stems += std::filesystem::path(currentImgPath).stem().string() + "|";
        }
        if (!img_stems.empty()) {
            img_stems.pop_back();
        }
        return "[等待任意图片 " + img_stems + "]";

    }

    [[nodiscard]] std::string toRepr() const override {
        return toString();
    }
};

class UntilImageStable : public Until {
public:
    std::vector<Segment> positions;

    explicit UntilImageStable(
            std::string imgPath,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.5,
            double timeout = -1
    ) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, std::move(mode)) {
        this->imgPath = std::move(imgPath);
    }

    void pre_hook(std::unique_ptr<Segment> &previous) override {
        std::vector<Segment> matchedPositions = findPositions(
                getScreen(mode),
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

    bool flag(std::unique_ptr<Segment> &previous) override {
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

    [[nodiscard]] std::string toString() const override {
        return "[等待图片稳定 " + std::filesystem::path(imgPath).stem().string() + "]";
    }

    [[nodiscard]] std::string toRepr() const override {
        return toString();
    }

};

#endif // QT_UNTIL_H