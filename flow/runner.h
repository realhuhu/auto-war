#ifndef QT_RUNNER_H
#define QT_RUNNER_H

#include <QString>
#include <QFileInfo>

#include "../util/state.h"
#include "../util/enum.h"
#include "../util/sleep.h"
#include "../util/action.h"
#include "../util/emitter.h"
#include "cv.h"
#include "until.h"
#include "segment.h"

class ImageClicker {
public:
    std::string templatePath;
    float globalThreshold;
    float globalTimeout;
    std::vector<Segment> targetSegmentList;
    std::unique_ptr<Segment> previousSegment;

    explicit ImageClicker(
            const std::string &imgPath,
            float wait = 0,
            float threshold = 0.9,
            float timeout = 60,
            Mode mode = Mode::GRAY
    );

    explicit ImageClicker(
            const std::vector<std::string> &imgPathList,
            float threshold = 0.9,
            float timeout = 60,
            Mode mode = Mode::GRAY
    );

    explicit ImageClicker(
            std::string imgPath,
            const Segment &segment,
            float threshold = 0.9,
            float timeout = 60
    );

    explicit ImageClicker(
            std::string imgPath,
            const std::vector<Segment> &targetSegmentList,
            float threshold = 0.9,
            float timeout = 60
    );

    [[nodiscard]] bool founded() const;

    std::unique_ptr<ImageClicker> _createChain(
            const std::vector<std::unique_ptr<Until>> &clickUntilList,
            const std::vector<std::unique_ptr<Until>> &runUntilList
    );

    void _start(float startWait, const std::vector<std::unique_ptr<Until>> &startUntilList);

    void _click(
            const Selector &selector,
            const std::vector<std::unique_ptr<Until>> &clickUntilList,
            int offsetX = 0,
            int offsetY = 0,
            Click position = Click::CENTER,
            float interval = 1
    );

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
            const Selector &selector = similaritySelector,
            float startWait = 0,
            float finishWait = 0,
            int offsetX = 0,
            int offsetY = 0,
            Click position = Click::CENTER,
            float interval = 1
    );

    std::unique_ptr<ImageClicker> clickIfFound(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &clickUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similaritySelector,
            float startWait = 0,
            float finishWait = 0,
            int offsetX = 0,
            int offsetY = 0,
            Click position = Click::CENTER,
            float interval = 1
    );


    std::unique_ptr<ImageClicker> drag(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &clickUntilList = {},
            const Selector &selector = similaritySelector,
            float startWait = 0,
            float finishWait = 0
    );

    std::unique_ptr<ImageClicker> locate(
            const std::vector<std::unique_ptr<Until>> &startUntilList = {},
            const std::vector<std::unique_ptr<Until>> &runUntilList = {},
            const Selector &selector = similaritySelector,
            float startWait = 0,
            float finishWait = 0
    );


    [[nodiscard]] std::string toString() const;
};

#endif // QT_RUNNER_H