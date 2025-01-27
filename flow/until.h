#ifndef QT_UNTIL_H
#define QT_UNTIL_H

#include <vector>
#include <chrono>
#include <string>
#include <memory>
#include <thread>
#include <iostream>

#include <QCoreApplication>

#include "enum.h"
#include "segment.h"

class Until {
public:
    std::string imgPath;
    double threshold;
    Previous onPrevious;
    double interval;
    double timeout;
    double finishWait;
    bool reverse;
    Mode mode;
    std::vector<Segment> targetSegmentList;

    explicit Until(
            double threshold = 0.9,
            Previous onPrevious = Previous::NONE,
            double interval = 0.1,
            double finishWait = 0,
            double timeout = -1,
            bool reverse = false,
            Mode mode = Mode::GRAY
    );

    virtual void loop(std::unique_ptr<Segment> &previous, int globalTimeout);

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
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
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
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    explicit UntilAnyImage(
            const std::vector<std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
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
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.2,
            double timeout = -1
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
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    [[nodiscard]] std::string toString() const override;

    void loop(std::unique_ptr<Segment> &previous, int globalTimeout) override;
};

class UntilIfAnyImage : public UntilAnyImage {
public:
    UntilIfAnyImage(
            const std::initializer_list<const std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    explicit UntilIfAnyImage(
            const std::vector<std::string> &imgList,
            Previous onPrevious = Previous::NONE,
            bool reverse = false,
            Mode mode = Mode::GRAY,
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    [[nodiscard]] std::string toString() const override;

    void loop(std::unique_ptr<Segment> &previous, int globalTimeout) override;
};


void clearUntil(
        std::vector<std::unique_ptr<Until>> &startUntil,
        std::vector<std::unique_ptr<Until>> &clickUntil,
        std::vector<std::unique_ptr<Until>> &runUntil
);

#endif // QT_UNTIL_H