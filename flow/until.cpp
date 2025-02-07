#include "until.h"

Until::Until(
        float threshold,
        Previous onPrevious,
        float interval,
        float finishWait,
        float timeout,
        bool reverse,
        Mode mode
) : threshold(threshold),
    onPrevious(onPrevious),
    interval(interval),
    timeout(timeout),
    finishWait(finishWait),
    reverse(reverse),
    mode(mode) {}

void Until::loop(std::unique_ptr<Segment> &previous, float globalTimeout) {
    auto start = std::chrono::high_resolution_clock::now();
    auto maxTime = (timeout == -1) ? globalTimeout : timeout;

    while (!state.stopFlag.load()) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<float>(now - start).count();

        if (elapsed > maxTime) throw std::runtime_error("超时，结束运行: " + this->toString());

        bool fulfilled = this->fulfilled(previous);
        if (fulfilled) {
            sleep(finishWait);
            break;
        }

        sleep(interval);
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
    if (onPrevious == Previous::NONE) return positions;

    if (!previous) throw std::runtime_error("Previous segment为空" + this->toString());

    std::vector<Segment> result;

    emit Emitter::instance()->log(
            QString::fromStdString("筛选: 在" + previous->toString() + "" + PreviousToString(onPrevious))
    );

    switch (onPrevious) {
        case Previous::LEFT:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "vertical") == "left")) continue;
                result.push_back(position);
            }
            break;
        case Previous::RIGHT:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "vertical") == "right")) continue;
                result.push_back(position);
            }
            break;
        case Previous::TOP:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "top")) continue;
                result.push_back(position);
            }
            break;
        case Previous::DOWN:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "down")) continue;
                result.push_back(position);
            }
            break;
        case Previous::LEFT_CENTER:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "center")) continue;
                if (!(position.on(*previous, "vertical") == "left")) continue;
                result.push_back(position);
            }
            break;
        case Previous::TOP_CENTER:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "top")) continue;
                if (!(position.on(*previous, "vertical") == "center")) continue;
                result.push_back(position);
            }
            break;
        case Previous::RIGHT_CENTER:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "center")) continue;
                if (!(position.on(*previous, "vertical") == "right")) continue;
                result.push_back(position);
            }
            break;
        case Previous::DOWN_CENTER:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "down")) continue;
                if (!(position.on(*previous, "vertical") == "center")) continue;
                result.push_back(position);
            }
            break;
        case Previous::INNER:
            for (const auto &position: positions) {
                if (!(position.on(*previous, "horizontal") == "center")) continue;
                if (!(position.on(*previous, "vertical") == "center")) continue;
                result.push_back(position);
            }
            break;
        case Previous::NONE:
            return positions;
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
    return "[Until" + imgPath + "]";
}

UntilImage::UntilImage(
        const std::string &imgPath,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, mode) {
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
            "等待%1<img src='%2' height='14'>"
    ).arg(
            QString(reverse ? "消失" : ""),
            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + imgPath)
    ).toStdString();
}


UntilAnyImage::UntilAnyImage(
        const std::initializer_list<const std::string> &imgList,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, mode) {
    for (const auto &i: imgList) {
        imgPathList.push_back(i);
    }
}

UntilAnyImage::UntilAnyImage(
        const std::vector<std::string> &imgList,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, mode) {
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
        start += QString("<img src='%1' height='14'>|").arg(
                QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + currentImgPath)
        );
    }

    return start.toStdString();
}


UntilImageStable::UntilImageStable(
        std::string imgPath,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, mode) {
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
            "等待稳定<img src='%1' height='14'>"
    ).arg(
            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + imgPath)
    ).toStdString();
}


UntilIfImage::UntilIfImage(
        const std::string &imgPath,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : UntilImage(imgPath, onPrevious, reverse, mode, finishWait, threshold, interval, timeout) {}

void UntilIfImage::loop(std::unique_ptr<Segment> &previous, float globalTimeout) {
    this->fulfilled(previous);
}

std::string UntilIfImage::toString() const {
    return QString(
            "尝试等待<img src='%1' height='14'>"
    ).arg(
            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + imgPath)
    ).toStdString();
}


UntilIfAnyImage::UntilIfAnyImage(
        const std::initializer_list<const std::string> &imgList,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : UntilAnyImage(imgList, onPrevious, reverse, mode, finishWait, threshold, interval, timeout) {}

UntilIfAnyImage::UntilIfAnyImage(
        const std::vector<std::string> &imgList,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : UntilAnyImage(imgList, onPrevious, reverse, mode, finishWait, threshold, interval, timeout) {}

void UntilIfAnyImage::loop(std::unique_ptr<Segment> &previous, float globalTimeout) {
    this->fulfilled(previous);
}

std::string UntilIfAnyImage::toString() const {
    auto start = QString("尝试等待|");

    for (const auto &currentImgPath: imgPathList) {
        start += QString("<img src='%1' height='14'>|").arg(
                QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + currentImgPath)
        );
    }

    return start.toStdString();
}

UntilCustom::UntilCustom(
        UntilCustomFunc func,
        Previous onPrevious,
        bool reverse,
        Mode mode,
        float finishWait,
        float threshold,
        float interval,
        float timeout
) : Until(threshold, onPrevious, interval, finishWait, timeout, reverse, mode), func(std::move(func)) {
    imgPath = "";
}

std::string UntilCustom::toString() const { return "自定义Until"; }

bool UntilCustom::flag(std::unique_ptr<Segment> &previous) {
    auto foundedSegments = func(previous, mode, threshold);

    if (foundedSegments.empty()) return false;

    targetSegmentList = foundedSegments;
    return true;
}

