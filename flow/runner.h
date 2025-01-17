// runner.h
#ifndef QT_RUNNER_H
#define QT_RUNNER_H

#include <vector>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>
#include <thread>
#include <functional>
#include <QString>

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
            const std::string &imgPath,
            float threshold = 0.9,
            int timeout = 60,
            const std::string &mode = "gray"
    );

    explicit ImageClicker(
            std::string imgPath,
            const Segment &segment,
            float threshold = 0.9,
            int timeout = 60
    );

    explicit ImageClicker(
            std::string imgPath,
            const std::vector<Segment> &targetSegmentList,
            float threshold = 0.9,
            int timeout = 60
    );

    [[nodiscard]] bool founded() const;

    std::unique_ptr<ImageClicker> _createChain(
            const std::vector<std::unique_ptr<Until>> &clickUntilList,
            const std::vector<std::unique_ptr<Until>> &runUntilList
    );

    void _start(float startWait, const std::vector<std::unique_ptr<Until>> &startUntilList);

    void _click(std::unique_ptr<Segment> position, const std::vector<std::unique_ptr<Until>> &clickUntilList);

    void _finish(float finishWait, const std::vector<std::unique_ptr<Until>> &runUntilList);

    std::unique_ptr<ImageClicker> _execute(
            const std::string &name,
            const std::function<bool()> &executor,
            float startWait,
            float finishWait,
            const std::vector<std::unique_ptr<Until>> &startUntilList,
            const std::vector<std::unique_ptr<Until>> &clickUntilList,
            const std::vector<std::unique_ptr<Until>> &runUntilList
    );

    std::unique_ptr<ImageClicker> click(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &clickUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similarity_selector,
            float startWait = 0,
            float finishWait = 0
    );

    std::unique_ptr<ImageClicker> clickIfFound(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &clickUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similarity_selector,
            float startWait = 0,
            float finishWait = 0
    );

    std::unique_ptr<ImageClicker> locate(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similarity_selector,
            float startWait = 0,
            float finishWait = 0
    );

    [[nodiscard]] std::string toString() const;
};

#endif // QT_RUNNER_H