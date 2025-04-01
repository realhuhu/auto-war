#include "runner.h"

#include <utility>

Clicker::Clicker(
        QString templatePath,
        ClickerInitConfig config
) : globalThreshold(config.threshold), globalTimeout(config.timeout), imgPath(std::move(templatePath)) {
    sleep(env.stopFlag, config.wait);
    targetSegmentList = CV::findPositions(
            CV::getScreen(env.hwnd, config.mode),
            imgPath,
            globalThreshold,
            config.mode
    );
}

Clicker::Clicker(
        QString templatePath,
        const Segment &segment,
        ClickerInitConfig config
) : globalThreshold(config.threshold),
    globalTimeout(config.timeout),
    imgPath(std::move(templatePath)) {
    targetSegmentList.push_back(segment);
}

Clicker::Clicker(
        QString templatePath,
        const std::vector<Segment> &segmentList,
        ClickerInitConfig config
) : globalThreshold(config.threshold),
    globalTimeout(config.timeout),
    imgPath(std::move(templatePath)),
    targetSegmentList(segmentList) {}

std::unique_ptr<Clicker> Clicker::_createNext(
        const std::vector<Until *> &runUntilList,
        const std::vector<Until *> &finishUntilList
) {
    const Until *until;

    if (!empty(finishUntilList)) {
        until = finishUntilList.back();
    } else if (!empty(runUntilList)) {
        until = runUntilList.back();
    } else {
        return nullptr;
    }

    return std::make_unique<Clicker>(
            until->imgPath,
            until->targetSegmentList,
            ClickerInitConfig{.threshold=globalThreshold, .timeout=globalTimeout}
    );
}

void Clicker::_start(float startWait, const std::vector<Until *> &startUntilList) {
    sleep(env.stopFlag, startWait);

    for (const auto &startUntil: startUntilList) {
        emit env.emitter->log("等待开始: " + startUntil->toString());
        startUntil->loop(previousSegment, globalTimeout);
        emit env.emitter->log("已开始: " + startUntil->toString());
    }
}

std::unique_ptr<Clicker> Clicker::_run(
        const QString &name,
        const std::function<void()> &executor,
        float startWait,
        float finishWait,
        const std::vector<Until *> &startUntilList,
        const std::vector<Until *> &runUntilList,
        const std::vector<Until *> &finishUntilList,
        bool homing
) {
    if (homing) Mouse::moveTo(env.hwnd, 0, 0);

    emit env.emitter->log(toString() + "开始" + name + "流程", "green");

    _start(startWait, startUntilList);

    executor();

    _finish(finishWait, finishUntilList);

    auto clicker = _createNext(runUntilList, finishUntilList);

    emit env.emitter->log(toString() + "结束" + name + "流程", "green");

    if (homing) Mouse::moveTo(env.hwnd, 0, 0);

    return clicker;
}

void Clicker::_finish(float finishWait, const std::vector<Until *> &finishUntilList) {
    sleep(env.stopFlag, finishWait);

    for (const auto &finishUntil: finishUntilList) {
        emit env.emitter->log("等待结束: " + finishUntil->toString());
        finishUntil->loop(previousSegment, globalTimeout);
        emit env.emitter->log("已结束: " + finishUntil->toString());
    }
}

std::unique_ptr<Clicker> Clicker::locate(ClickerRunConfig config) {
    auto executor = [this, &config] {
        if (targetSegmentList.empty()) throw std::runtime_error("未匹配到图像: " + imgPath.toStdString());

        auto segment = config.selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        emit env.emitter->log(toString() + "定位成功: " + segment.toString());
    };

    return _run(
            "locate",
            executor,
            config.startWait,
            config.finishWait,
            config.startUntilList,
            {},
            config.finishUntilList,
            config.homing
    );
}

std::unique_ptr<Clicker> Clicker::click(
        ClickerRunConfig config,
        Click position,
        int offsetX,
        int offsetY,
        float interval
) {
    auto executor = [this, &config, &offsetX, &offsetY, &position, &interval] {
        if (targetSegmentList.empty()) throw std::runtime_error("未匹配到图像: " + imgPath.toStdString());

        auto segment = config.selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        auto startTime = std::chrono::high_resolution_clock::now();

        emit env.emitter->log("开始循环点击: " + segment.toString());

        segment.click(0, offsetX, offsetY, position);

        sleep(env.stopFlag, 0.5);

        while (!env.stopFlag->load()) {
            auto nowTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<float>(nowTime - startTime).count();

            if (elapsed > globalTimeout) throw std::runtime_error("超时，结束运行: " + toString().toStdString());

            if (empty(config.runUntilList)) break;

            bool allFulfilled = true;
            for (const auto &runUntil: config.runUntilList) {
                if (!runUntil->fulfilled(previousSegment)) {
                    allFulfilled = false;
                    break;
                }
            }

            if (allFulfilled) {
                emit env.emitter->log("所有CLICK满足，结束循环点击");
                break;
            }

            segment.click(0, offsetX, offsetY, position);

            emit env.emitter->log("CLICK不满足，继续循环点击");

            sleep(env.stopFlag, interval);
        }

        return false;
    };

    return _run(
            "click",
            executor,
            config.startWait,
            config.finishWait,
            config.startUntilList,
            config.runUntilList,
            config.finishUntilList,
            config.homing
    );
}

std::unique_ptr<Clicker> Clicker::drag(ClickerRunConfig config, int step, bool reverse) {
    auto executor = [this, &config, &reverse, &step]() -> bool {
        if (targetSegmentList.empty()) throw std::runtime_error("未匹配到图像: " + imgPath.toStdString());

        auto beforeSegment = config.selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(beforeSegment.copy());

        emit env.emitter->log("开始拖动: " + beforeSegment.toString());

        if (reverse) {
            while (!env.stopFlag->load()) {
                beforeSegment.drag(0.1, 100);
                auto afterSegment = config.selector(CV::findPositions(CV::getScreen(env.hwnd), imgPath));

                if (beforeSegment == afterSegment) {
                    emit env.emitter->log("已拖动到底，结束拖动");
                    break;
                }

                beforeSegment = afterSegment;
            }
        }

        while (!env.stopFlag->load()) {
            if (empty(config.runUntilList)) break;

            bool allFulfilled = true;
            for (const auto &runUntil: config.runUntilList) {
                if (!runUntil->fulfilled(previousSegment)) {
                    allFulfilled = false;
                    break;
                }
            }

            if (allFulfilled) {
                emit env.emitter->log("所有CLICK满足，结束拖动");
                break;
            }

            beforeSegment.drag(0.1, reverse ? -step : step);
            auto afterSegment = config.selector(CV::findPositions(CV::getScreen(env.hwnd), imgPath));

            if (beforeSegment == afterSegment) {
                emit env.emitter->log("已拖动到底，结束拖动");
                break;
            }

            emit env.emitter->log("CLICK不满足，继续拖动");

            beforeSegment = afterSegment;
        }

        return false;
    };

    return _run(
            "drag",
            executor,
            config.startWait,
            config.finishWait,
            config.startUntilList,
            config.runUntilList,
            {},
            config.homing
    );
}

bool Clicker::founded() const {
    return !targetSegmentList.empty();
}

void Clicker::end() {};

QString Clicker::toString() const {
    return QString("[%1]").arg(QFileInfo(imgPath).baseName());
}
