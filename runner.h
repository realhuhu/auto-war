#ifndef QT_RUNNER_H
#define QT_RUNNER_H

#include <vector>
#include <chrono>
#include <iostream>
#include <filesystem>

#include <QDebug>
#include <QString>

#include "cv.h"
#include "until.h"
#include "segment.h"

class ImageClicker {
public:
    std::string templatePath;
    float globalThreshold;
    int globalTimeout;
    std::vector<Segment> targetSegmentList;
    std::unique_ptr<Segment> previousSegment;

    explicit ImageClicker(
            std::string templatePath,
            float threshold = 0.9,
            int timeout = 60,
            const std::vector<Segment> &targetSegmentList = {}
    ) : templatePath(std::move(templatePath)),
        globalThreshold(threshold),
        globalTimeout(timeout),
        targetSegmentList(targetSegmentList) {
        previousSegment = nullptr;
    }

    std::unique_ptr<ImageClicker> _createChain(
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
                globalThreshold,
                globalTimeout,
                until->targetSegmentList
        );
    }


    void _start(float startWait, const std::vector<std::unique_ptr<Until>> &startUntilList) {
        if (startWait > 0) {
            std::this_thread::sleep_for(std::chrono::duration<float>(startWait));
        }

        for (const auto &startUntil: startUntilList) {
            qDebug() << QString::fromStdString("start条件判断: " + startUntil->toString());
            (*startUntil).loop(previousSegment, globalTimeout);
            qDebug() << QString::fromStdString("start条件满足: " + startUntil->toString());
        }
    }

    void _click(std::unique_ptr<Segment> position, const std::vector<std::unique_ptr<Until>> &clickUntilList) {
        auto start = std::chrono::high_resolution_clock::now();

        if (!previousSegment) {
            previousSegment = std::make_unique<Segment>(position->copy());
        }

        while (!state.stopFlag.load()) {
            qDebug() << QString::fromStdString("开始循环点击: " + position->toString());


            auto now = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > globalTimeout) {
                throw std::runtime_error("Timed out");
            }

            // 假设Segment类有click方法
            position->click();

            std::this_thread::sleep_for(std::chrono::duration<float>(0.5));

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
                qDebug() << "所有click条件已满足，结束循环点击";
                break;
            }
            qDebug() << "click条件不满足，继续循环点击";
        }

    }

    void _finish(float finishWait, const std::vector<std::unique_ptr<Until>> &runUntilList) {
        if (finishWait > 0) {
            std::this_thread::sleep_for(std::chrono::duration<float>(finishWait));
        }

        for (const auto &runUntil: runUntilList) {
            qDebug() << QString::fromStdString("run条件判断: " + runUntil->toString());
            (*runUntil).loop(previousSegment, globalTimeout);
            qDebug() << QString::fromStdString("run条件满足: " + runUntil->toString());
        }
    }

    std::unique_ptr<ImageClicker> _execute(
            const std::string &name,
            const std::function<bool(std::unique_ptr<Segment>)> &executor,
            float startWait,
            float finishWait,
            const Selector &selector,
            const std::vector<std::unique_ptr<Until>> &startUntilList,
            const std::vector<std::unique_ptr<Until>> &clickUntilList,
            const std::vector<std::unique_ptr<Until>> &runUntilList
    ) {
        qDebug() << QString::fromStdString(this->toString() + "开始" + name + "流程");

        std::unique_ptr<Segment> position;

        if (!targetSegmentList.empty()) {
            auto segment = selector(targetSegmentList);

            previousSegment = std::make_unique<Segment>(segment.copy());
            position = std::make_unique<Segment>(segment.copy());
        }

        _start(startWait, startUntilList);

        bool skipFinish = executor(std::move(position));

        if (!skipFinish) {
            _finish(finishWait, runUntilList);
        }

        auto clicker = _createChain(clickUntilList, runUntilList);

        qDebug() << QString::fromStdString(this->toString() + "结束" + name + "流程");
        qDebug() << QString::fromStdString("下一调用链: " + (clicker ? clicker->toString() : "无"));

        return clicker;
    }

    std::unique_ptr<ImageClicker> click(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &clickUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similarity_selector,
            float threshold = -1,
            float startWait = 0,
            float finishWait = 0
    ) {
        auto executor = [this, &threshold, &selector, &clickUntilList](std::unique_ptr<Segment> position) -> bool {
            if (!position) {
                std::vector<Segment> positions;
                auto start = std::chrono::high_resolution_clock::now();

                while (positions.empty()) {
                    auto now = std::chrono::high_resolution_clock::now();

                    if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > globalTimeout) {
                        throw std::runtime_error("Timed out");
                    }

                    positions = findPositions(
                            getScreen(),
                            templatePath,
                            threshold > 0 ? threshold : this->globalThreshold
                    );

                    if (positions.empty()) {
                        qDebug() << QString::fromStdString(this->toString() + "匹配失败，再次尝试");
                    }
                }

                position = std::make_unique<Segment>(selector(positions));
            }

            if (!position) {
                throw std::runtime_error("No position found");
            }

            _click(std::move(position), clickUntilList);

            return false;
        };

        return _execute(
                "click",
                executor,
                startWait,
                finishWait,
                selector,
                startUntilList,
                clickUntilList,
                runUntilList
        );
    }

    std::unique_ptr<ImageClicker> clickIfFound(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &clickUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similarity_selector,
            float threshold = -1,
            float startWait = 0,
            float finishWait = 0
    ) {
        auto executor = [this, &threshold, &selector, &clickUntilList](std::unique_ptr<Segment> position) -> bool {
            std::vector<Segment> positions;

            if (!position) {
                positions = findPositions(
                        getScreen(),
                        templatePath,
                        threshold > 0 ? threshold : this->globalThreshold
                );

                if (!positions.empty()) {
                    position = std::make_unique<Segment>(selector(positions));
                }
            }

            if (position) {
                _click(std::move(position), clickUntilList);
                return false;
            } else {
                return true;
            }
        };

        return _execute(
                "click_if_found",
                executor,
                startWait,
                finishWait,
                selector,
                startUntilList,
                clickUntilList,
                runUntilList
        );
    }

    std::unique_ptr<ImageClicker> locate(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similarity_selector,
            float threshold = -1,
            float startWait = 0,
            float finishWait = 0
    ) {
        auto executor = [this, &selector, &threshold](std::unique_ptr<Segment> position) -> bool {
            if (!position) {
                // 假设findPositions和getScreen函数已定义
                std::vector<Segment> positions = findPositions(
                        getScreen(),
                        templatePath,
                        threshold > 0 ? threshold : this->globalThreshold
                );

                if (!positions.empty()) {
                    position = std::make_unique<Segment>(selector(positions));
                }
            }

            if (!position) {
                throw std::runtime_error("No position found");
            }

            qDebug() << QString::fromStdString(this->toString() + "定位成功: " + position->toString());

            if (!previousSegment) {
                previousSegment = std::move(position);
            }
            return false;
        };

        return _execute(
                "locate",
                executor,
                startWait,
                finishWait,
                selector,
                startUntilList,
                {},
                runUntilList
        );
    }

    [[nodiscard]] std::string toString() const {
        return "[" + std::filesystem::path(templatePath).stem().string() + "]";
    }

    [[nodiscard]] std::string toRepr() const {
        return toString();
    }
};


#endif //QT_RUNNER_H
