#ifndef QT_UNTIL_H
#define QT_UNTIL_H

#include <vector>
#include <chrono>
#include <iostream>

#include <QCoreApplication>

#include "../util/enum.h"
#include "../util/state.h"
#include "../util/sleep.h"
#include "../util/emitter.h"
#include "cv.h"
#include "segment.h"

class Until {
public:
    std::string imgPath;
    float threshold;
    Previous onPrevious;
    float interval;
    float timeout;
    float finishWait;
    bool reverse;
    Mode mode;
    std::vector<Segment> targetSegmentList;

    explicit Until(
            float threshold = 0.9,
            Previous onPrevious = Previous::NONE,
            float interval = 0.1,
            float finishWait = 0,
            float timeout = -1,
            bool reverse = false,
            Mode mode = Mode::GRAY
    );

    virtual void loop(std::unique_ptr<Segment> &previous, float globalTimeout);

    bool fulfilled(std::unique_ptr<Segment> &previous);

    [[nodiscard]] std::vector<Segment>
    filter(const std::vector<Segment> &positions, std::unique_ptr<Segment> &previous) const;

    Until operator~();

    virtual void preHook(std::unique_ptr<Segment> &previous);

    virtual bool flag(std::unique_ptr<Segment> &previous);

    [[nodiscard]] virtual std::string toString() const;
};

class UntilImage : public Until {
public:
    explicit UntilImage(
            const std::string &imgPath,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    bool flag(std::unique_ptr<Segment> &previous) override;

    [[nodiscard]] std::string toString() const override;
};

class UntilAnyImage : public Until {
public:
    std::vector<std::string> imgPathList;

    UntilAnyImage(
            const std::initializer_list<const std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    explicit UntilAnyImage(
            const std::vector<std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    bool flag(std::unique_ptr<Segment> &previous) override;

    [[nodiscard]] std::string toString() const override;
};

class UntilImageStable : public Until {
public:
    std::vector<Segment> positions;

    explicit UntilImageStable(
            std::string imgPath,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.2,
            float timeout = -1
    );

    void preHook(std::unique_ptr<Segment> &previous) override;

    bool flag(std::unique_ptr<Segment> &previous) override;

    [[nodiscard]] std::string toString() const override;
};

class UntilIfImage : public UntilImage {
public:
    explicit UntilIfImage(
            const std::string &imgPath,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    [[nodiscard]] std::string toString() const override;

    void loop(std::unique_ptr<Segment> &previous, float globalTimeout) override;
};

class UntilIfAnyImage : public UntilAnyImage {
public:
    UntilIfAnyImage(
            const std::initializer_list<const std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    explicit UntilIfAnyImage(
            const std::vector<std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    [[nodiscard]] std::string toString() const override;

    void loop(std::unique_ptr<Segment> &previous, float globalTimeout) override;
};

using UntilCustomFunc = std::function<std::vector<Segment>(std::unique_ptr<Segment> &previous, Mode mode,float threshold)>;

class UntilCustom : public Until {
public:
    UntilCustomFunc func;

    explicit UntilCustom(
            UntilCustomFunc func,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            float finishWait = 0,
            float threshold = 0.9,
            float interval = 0.1,
            float timeout = -1
    );

    [[nodiscard]] std::string toString() const override;

    bool flag(std::unique_ptr<Segment> &previous) override;
};

#endif // QT_UNTIL_H