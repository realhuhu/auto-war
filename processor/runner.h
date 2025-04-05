#ifndef RED_RUNNER_H
#define RED_RUNNER_H


#include <QString>
#include <QFileInfo>

#include "../util/tool.h"
#include "../util/enum.h"
#include "../util/state.h"
#include "../util/action.h"
#include "../util/emitter.h"
#include "cv.h"
#include "until.h"
#include "segment.h"

struct ClickerInitConfig {
    float threshold = 0.9;
    float timeout = 60;
    float wait = 0;
    Mode mode = Mode::GRAY;
};

struct ClickerRunConfig {
    float startWait = 0;
    const Selector selector = similaritySelector;
    const std::vector<Until *> startUntilList = {};
    const std::vector<Until *> runUntilList = {};
    const std::vector<Until *> finishUntilList = {};
    float finishWait = 0;
    bool homing = true;
};


class Clicker {
public:
    QString imgPath;
    float globalThreshold;
    float globalTimeout;
    std::vector<Segment> targetSegmentList;
    std::unique_ptr<Segment> previousSegment;

    explicit Clicker(QString templatePath, ClickerInitConfig config = {});

    explicit Clicker(QString templatePath, const Segment &segment, ClickerInitConfig config = {});

    explicit Clicker(QString templatePath, const std::vector<Segment> &segmentList, ClickerInitConfig config = {});

    [[nodiscard]]  std::unique_ptr<Clicker> _createNext(
            const std::vector<std::unique_ptr<Until>> &runUntilList,
            const std::vector<std::unique_ptr<Until>> &finishUntilList
    );

    void _start(float startWait, const std::vector<std::unique_ptr<Until>> &startUntilList);

    [[nodiscard]]  std::unique_ptr<Clicker> _run(
            const QString &name,
            const std::function<void()> &executor,
            float startWait,
            float finishWait,
            const std::vector<Until *> &startUntilList,
            const std::vector<Until *> &runUntilList,
            const std::vector<Until *> &finishUntilList,
            bool homing
    );

    void _finish(float finishWait, const std::vector<std::unique_ptr<Until>> &finishUntilList);

    [[nodiscard]] std::unique_ptr<Clicker> locate(ClickerRunConfig config = {});

    [[nodiscard]] std::unique_ptr<Clicker> click(
            ClickerRunConfig config = {},
            float interval = 1,
            int offsetX = 0,
            int offsetY = 0,
            Click position = Click::CENTER
    );

    [[nodiscard]] std::unique_ptr<Clicker> drag(
            ClickerRunConfig config = {},
            int step = 10,
            bool reverse = false
    );

    void end();

    [[nodiscard]] bool founded() const;

    [[nodiscard]] QString toString() const;
};

#endif //RED_RUNNER_H
