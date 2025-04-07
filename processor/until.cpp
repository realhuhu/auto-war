#include "until.h"

#include <utility>

Until::Until(UntilConfig untilConfig) : config(untilConfig) {}

void Until::loop(std::unique_ptr<Segment> &previous, float globalTimeout) {
    sleep(env.stopFlag, config.startWait);

    auto start = std::chrono::high_resolution_clock::now();
    auto maxTime = (config.timeout == -1) ? globalTimeout : config.timeout;

    while (!env.stopFlag->load()) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<float>(now - start).count();

        if (elapsed > maxTime) throw std::runtime_error("超时，结束运行: " + toString().toStdString());

        if (fulfilled(previous)) {
            sleep(env.stopFlag, config.finishWait);
            break;
        }

        sleep(env.stopFlag, config.interval);
    }
}

bool Until::fulfilled(std::unique_ptr<Segment> &previous) {
    preHook(previous);
    bool isFulfilled = config.reverse == !flag(previous);

    if (isFulfilled) {
        emit env.emitter->log("条件满足: " + toString());
        env.logFlag.remove(QString("%1-until-not-fulfilled").arg(toString()));
        return true;
    }

    if (!env.logFlag.contains(QString("%1-until-not-fulfilled").arg(toString()))) {
        env.logFlag[QString("%1-until-not-fulfilled").arg(toString())] = true;
        emit env.emitter->log("条件未满足: " + toString());
    }

    return false;
}

std::vector<Segment> Until::filter(const std::vector<Segment> &positions, std::unique_ptr<Segment> &previous) const {
    if (config.onPrevious == Previous::NONE) return positions;

    if (!previous) throw std::runtime_error("Previous segment为空: " + toString().toStdString());

    std::vector<Segment> result = {};

    emit env.emitter->log("筛选: 在" + previous->toString() + "" + PreviousToString(config.onPrevious));

    switch (config.onPrevious) {
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
            for (const auto &position: positions) {
                result.push_back(position);
            }
    }

    emit env.emitter->log(
            QString("筛选前: %1, 筛选后: %2").arg(
                    QString::number(positions.size()),
                    QString::number(result.size())
            )
    );

    return result;
}

void Until::preHook(std::unique_ptr<Segment> &previous) {}

Image::Image(
        QString templatePath,
        UntilConfig untilConfig
) : Until(untilConfig) {
    imgPath = std::move(templatePath);
}

bool Image::flag(std::unique_ptr<Segment> &previous) {
    std::vector<Segment> matchedPositions = CV::findPositions(
            CV::getScreen(env.hwnd, config.mode),
            imgPath,
            config.threshold,
            config.mode
    );

    std::vector<Segment> filteredPositions = filter(matchedPositions, previous);

    if (filteredPositions.empty()) return false;

    targetSegmentList = filteredPositions;
    return true;
}

QString Image::toString() const { return QString("等待%1<img src='%2' height='14'>").arg(QString(config.reverse ? "消失" : ""), res(imgPath)); }

AnyImage::AnyImage(
        const std::vector<QString> &templatePathList,
        UntilConfig untilConfig
) : Until(untilConfig), imgPathList(templatePathList) {}

bool AnyImage::flag(std::unique_ptr<Segment> &previous) {
    cv::Mat screen = CV::getScreen(env.hwnd, config.mode);

    for (auto &currentImgPath: imgPathList) {
        auto matched = CV::findPositions(screen, currentImgPath, config.threshold, config.mode);

        auto filtered = filter(matched, previous);

        if (filtered.empty()) continue;

        imgPath = currentImgPath;
        targetSegmentList = filtered;
        return true;
    }

    return false;
}

QString AnyImage::toString() const {
    auto text = QString("等待任意|");

    for (const auto &currentImgPath: imgPathList) {
        text += QString("<img src='%1' height='14'>|").arg(res(currentImgPath));
    }

    return text;
}


ImageStable::ImageStable(
        QString templatePath,
        UntilConfig untilConfig
) : Until(untilConfig) {
    imgPath = std::move(templatePath);
}

void ImageStable::preHook(std::unique_ptr<Segment> &previous) {
    auto matched = CV::findPositions(
            CV::getScreen(env.hwnd, config.mode),
            imgPath,
            config.threshold,
            config.mode
    );

    auto filtered = filter(matched, previous);

    if (filtered.empty()) return;

    Segment position = similaritySelector(filtered);
    positions.push_back(position);

    if (positions.size() > 3) positions.erase(positions.begin());
}

bool ImageStable::flag(std::unique_ptr<Segment> &previous) {
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

QString ImageStable::toString() const { return QString("等待稳定<img src='%1' height='14'>").arg(res(imgPath)); }


IfImage::IfImage(
        const QString &imgPath,
        UntilConfig untilConfig
) : Image(imgPath, untilConfig) {}

void IfImage::loop(std::unique_ptr<Segment> &previous, float globalTimeout) { fulfilled(previous); }

QString IfImage::toString() const { return QString("尝试等待<img src='%1' height='14'>").arg(res(imgPath)); }

IfAnyImage::IfAnyImage(
        const std::vector<QString> &templatePathList,
        UntilConfig untilConfig
) : AnyImage(templatePathList, untilConfig) {}

void IfAnyImage::loop(std::unique_ptr<Segment> &previous, float globalTimeout) { fulfilled(previous); }

QString IfAnyImage::toString() const {
    auto text = QString("尝试等待任意|");

    for (const auto &currentImgPath: imgPathList) {
        text += QString("<img src='%1' height='14'>|").arg(res(currentImgPath));
    }

    return text;
}
