#include "runner.h"
#include <chrono>
#include <thread>
#include <utility>
#include <stdexcept>

#include "../state.h"
#include "cv.h"
#include "until.h"
#include "emitter.h"
#include "segment.h"

ImageClicker::ImageClicker(
        const std::string &imgPath,
        int wait,
        float threshold,
        int timeout,
        const std::string &mode
) : templatePath(imgPath),
    globalThreshold(threshold),
    globalTimeout(timeout) {

    if (wait) std::this_thread::sleep_for(std::chrono::duration<float>(wait));

    targetSegmentList = CV::findPositions(CV::getScreen(mode), imgPath, globalThreshold, mode);
}


ImageClicker::ImageClicker(
        const std::vector<std::string> &imgPathList,
        float threshold,
        int timeout,
        const std::string &mode
) : globalThreshold(threshold),
    globalTimeout(timeout) {
    for (const auto &imgPath: imgPathList) {
        auto ps = CV::findPositions(CV::getScreen(mode), imgPath, globalThreshold, mode);
        if (!ps.empty()) {
            templatePath = imgPath;
            targetSegmentList = ps;
        }
    }
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
    const Until *until;

    if (!runUntilList.empty()) {
        until = runUntilList.back().get();
    } else if (!clickUntilList.empty()) {
        until = clickUntilList.back().get();
    } else {
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
    if (startWait > 0) std::this_thread::sleep_for(std::chrono::duration<float>(startWait));

    for (const auto &startUntil: startUntilList) {
        emit Emitter::instance()->log(QString::fromStdString("START判断: " + startUntil->toString()));
        (*startUntil).loop(previousSegment, globalTimeout);
        emit Emitter::instance()->log(QString::fromStdString("START满足: " + startUntil->toString()));
    }
}

void ImageClicker::_click(
        const Selector &selector,
        const std::vector<std::unique_ptr<Until>> &clickUntilList,
        int offsetX,
        int offsetY,
        const std::string &position
) {
    auto start = std::chrono::high_resolution_clock::now();
    while (!state.stopFlag.load()) {
        auto segment = selector(targetSegmentList);

        emit Emitter::instance()->log(QString::fromStdString("开始循环点击: " + segment.toString()));

        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > globalTimeout) {
            throw std::runtime_error("超时，结束运行: " + this->toString());
        }

        segment.click(0, offsetX, offsetY, position);

        if (clickUntilList.empty()) break;

        bool allFulfilled = true;
        for (const auto &clickUntil: clickUntilList) {
            if (!clickUntil->fulfilled(previousSegment)) {
                allFulfilled = false;
                break;
            }
        }

        if (allFulfilled) {
            emit Emitter::instance()->log("所有CLICK满足，结束循环点击");
            break;
        }
        emit Emitter::instance()->log("CLICK不满足，继续循环点击");

        std::this_thread::sleep_for(std::chrono::duration<float>(1));
    }
}

void ImageClicker::_finish(
        float finishWait, const std::vector<std::unique_ptr<Until>> &runUntilList
) {
    if (finishWait > 0) std::this_thread::sleep_for(std::chrono::duration<float>(finishWait));

    for (const auto &runUntil: runUntilList) {
        emit Emitter::instance()->log(QString::fromStdString("RUN判断: " + runUntil->toString()));
        (*runUntil).loop(previousSegment, globalTimeout);
        emit Emitter::instance()->log(QString::fromStdString("RUN满足: " + runUntil->toString()));
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
    emit Emitter::instance()->log(QString::fromStdString(this->toString() + "开始" + name + "流程"), "green");

    _start(startWait, startUntilList);

    bool skipFinish = executor();

    if (!skipFinish) _finish(finishWait, runUntilList);

    auto clicker = _createChain(clickUntilList, runUntilList);

    emit Emitter::instance()->log(QString::fromStdString(this->toString() + "结束" + name + "流程"), "green");
    return clicker;
}

std::unique_ptr<ImageClicker> ImageClicker::click(
        const std::vector<std::unique_ptr<Until>> &startUntilList,
        const std::vector<std::unique_ptr<Until>> &clickUntilList,
        const std::vector<std::unique_ptr<Until>> &runUntilList,
        const Selector &selector,
        float startWait,
        float finishWait,
        int offsetX,
        int offsetY,
        const std::string &position
) {
    auto executor = [this, &selector, &clickUntilList, &offsetX, &offsetY, &position]() -> bool {
        if (targetSegmentList.empty()) throw std::runtime_error("未匹配到图像: " + this->templatePath);

        auto segment = selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        _click(
                selector,
                clickUntilList,
                offsetX,
                offsetY,
                position
        );

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
        float finishWait,
        int offsetX,
        int offsetY,
        const std::string &position
) {
    auto executor = [this, &selector, &clickUntilList, &offsetX, &offsetY, &position]() -> bool {
        if (targetSegmentList.empty()) {
            emit Emitter::instance()->log(
                    QString("未找到图片<img src='%2' alt='%3' height='14'>，开始下一流程").arg(
                            QCoreApplication::applicationDirPath() + QString::fromStdString("/res" + templatePath),
                            QString::fromStdString(templatePath)
                    )
            );
            return true;
        }

        auto segment = selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        _click(
                selector,
                clickUntilList,
                offsetX,
                offsetY,
                position
        );

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
        if (targetSegmentList.empty()) throw std::runtime_error("图片列表为空: " + this->templatePath);

        auto segment = selector(targetSegmentList);

        previousSegment = std::make_unique<Segment>(segment.copy());

        emit Emitter::instance()->log(QString::fromStdString(this->toString() + "定位成功: " + segment.toString()));

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
    if (templatePath.empty()) return "无";

    return "[" + std::filesystem::path(templatePath).stem().string() + "]";
}

bool ImageClicker::founded() const {
    return !targetSegmentList.empty();
}
