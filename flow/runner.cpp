// runner.cpp
#include "runner.h"
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <utility>

#include "../state.h"
#include "cv.h"
#include "until.h"
#include "emitter.h"
#include "segment.h"

ImageClicker::ImageClicker(
        const std::string &imgPath,
        float threshold,
        int timeout,
        const std::string &mode
) : templatePath(imgPath),
    globalThreshold(threshold),
    globalTimeout(timeout) {

    targetSegmentList = CV::find_positions(CV::get_screen(mode), imgPath, globalThreshold, mode);
}

ImageClicker::ImageClicker(
        std::string imgPath,
        const Segment &segment,
        float threshold,
        int timeout
) : templatePath(std::move(imgPath)),
    globalThreshold(threshold),
    globalTimeout(timeout) {

    targetSegmentList.push_back(segment);
}

ImageClicker::ImageClicker(
        std::string imgPath,
        const std::vector<Segment> &targetSegmentList,
        float threshold,
        int timeout
) : templatePath(std::move(imgPath)),
    globalThreshold(threshold),
    globalTimeout(timeout),
    targetSegmentList(targetSegmentList) {}

std::unique_ptr<ImageClicker> ImageClicker::_createChain(
        const std::vector<std::unique_ptr<Until>> &clickUntilList,
        const std::vector<std::unique_ptr<Until>> &runUntilList
) {
    const Until *until = nullptr;

    if (!runUntilList.empty()) {
        until = runUntilList.back().get();
    } else if (!clickUntilList.empty()) {
        until = clickUntilList.back().get();
    }

    if (!until) {
        return nullptr;
    }

    return std::make_unique<ImageClicker>(
            until->imgPath,
            until->targetSegmentList,
            globalThreshold,
            globalTimeout
    );
}

void ImageClicker::_start(
        float startWait,
        const std::vector<std::unique_ptr<Until>> &startUntilList
) {
    if (startWait > 0) {
        std::this_thread::sleep_for(std::chrono::duration<float>(startWait));
    }

    for (const auto &startUntil: startUntilList) {

        emit SignalEmitter::instance()->logMessage(QString::fromStdString("start条件判断: " + startUntil->toString()));
        (*startUntil).loop(previousSegment, globalTimeout);
        emit SignalEmitter::instance()->logMessage(QString::fromStdString("start条件满足: " + startUntil->toString()));
    }
}

void ImageClicker::_click(
        std::unique_ptr<Segment> position,
        const std::vector<std::unique_ptr<Until>> &clickUntilList
) {
    auto start = std::chrono::high_resolution_clock::now();

    while (!state.stopFlag.load()) {
        emit SignalEmitter::instance()->logMessage(QString::fromStdString("开始循环点击: " + position->toString()));

        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > globalTimeout) {
            throw std::runtime_error("超时，结束运行: " + this->toString());
        }

        position->click();


        if (clickUntilList.empty()) {
            break;
        }

        bool allFulfilled = true;
        for (const auto &clickUntil: clickUntilList) {
            if (!clickUntil->fulfilled(previousSegment)) {
                allFulfilled = false;
                break;
            }
        }

        if (allFulfilled) {
            emit SignalEmitter::instance()->logMessage("所有click条件已满足，结束循环点击");
            break;
        }
        emit SignalEmitter::instance()->logMessage("click条件不满足，继续循环点击");

        std::this_thread::sleep_for(std::chrono::duration<float>(0.5));
    }
}

void ImageClicker::_finish(
        float finishWait, const std::vector<std::unique_ptr<Until>> &runUntilList
) {
    if (finishWait > 0) {
        std::this_thread::sleep_for(std::chrono::duration<float>(finishWait));
    }

    for (const auto &runUntil: runUntilList) {
        emit SignalEmitter::instance()->logMessage(QString::fromStdString("run条件判断: " + runUntil->toString()));
        (*runUntil).loop(previousSegment, globalTimeout);
        emit SignalEmitter::instance()->logMessage(QString::fromStdString("run条件满足: " + runUntil->toString()));
    }
}

std::unique_ptr<ImageClicker> ImageClicker::_execute(
        const std::string &name,
        const std::function<bool()> &executor,
        float startWait,
        float finishWait,
        const std::vector<std::unique_ptr<Until>> &startUntilList,
        const std::vector<std::unique_ptr<Until>> &clickUntilList,
        const std::vector<std::unique_ptr<Until>> &runUntilList
) {
    emit SignalEmitter::instance()->logMessage(QString::fromStdString(this->toString() + "开始" + name + "流程"));

    _start(startWait, startUntilList);

    bool skipFinish = executor();

    if (!skipFinish) {
        _finish(finishWait, runUntilList);
    }

    auto clicker = _createChain(clickUntilList, runUntilList);

    emit SignalEmitter::instance()->logMessage(QString::fromStdString(this->toString() + "结束" + name + "流程"));
    emit SignalEmitter::instance()->logMessage(
            QString::fromStdString("下一调用链: " + (clicker ? clicker->toString() : "无")));

    return clicker;
}

std::unique_ptr<ImageClicker> ImageClicker::click(
        const std::vector<std::unique_ptr<Until>> &startUntilList,
        const std::vector<std::unique_ptr<Until>> &clickUntilList,
        const std::vector<std::unique_ptr<Until>> &runUntilList,
        const Selector &selector,
        float startWait,
        float finishWait
) {
    auto executor = [this, &selector, &clickUntilList]() -> bool {
        if (targetSegmentList.empty()) {
            throw std::runtime_error("图片列表为空: " + this->templatePath);
        }

        auto segment = selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        _click(std::make_unique<Segment>(segment.copy()), clickUntilList);

        return false;
    };

    return _execute(
            "click",
            executor,
            startWait,
            finishWait,
            startUntilList,
            clickUntilList,
            runUntilList
    );
}

std::unique_ptr<ImageClicker> ImageClicker::clickIfFound(
        const std::vector<std::unique_ptr<Until>> &startUntilList,
        const std::vector<std::unique_ptr<Until>> &clickUntilList,
        const std::vector<std::unique_ptr<Until>> &runUntilList,
        const Selector &selector,
        float startWait,
        float finishWait
) {
    auto executor = [this, &selector, &clickUntilList]() -> bool {
        if (targetSegmentList.empty()) {
            return true;
        }

        auto segment = selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        _click(std::make_unique<Segment>(segment.copy()), clickUntilList);

        return false;
    };

    return _execute(
            "click_if_found",
            executor,
            startWait,
            finishWait,
            startUntilList,
            clickUntilList,
            runUntilList
    );
}

std::unique_ptr<ImageClicker> ImageClicker::locate(
        const std::vector<std::unique_ptr<Until>> &startUntilList,
        const std::vector<std::unique_ptr<Until>> &runUntilList,
        const Selector &selector,
        float startWait,
        float finishWait
) {
    auto executor = [this, &selector]() -> bool {
        if (targetSegmentList.empty()) {
            throw std::runtime_error("图片列表为空: " + this->templatePath);
        }

        auto segment = selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        emit SignalEmitter::instance()->logMessage(
            QString::fromStdString(this->toString() + "定位成功: " + segment.toString()
            ));

        return false;
    };

    return _execute(
            "locate",
            executor,
            startWait,
            finishWait,
            startUntilList,
            {},
            runUntilList
    );
}

std::string ImageClicker::toString() const {
    if (templatePath.empty()) {
        return "无";
    }
    return "[" + std::filesystem::path(templatePath).stem().string() + "]";
}

bool ImageClicker::founded() const {
    return !targetSegmentList.empty();
}
