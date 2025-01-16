// until.h
#ifndef QT_UNTIL_H
#define QT_UNTIL_H

#include <vector>
#include <chrono>
#include <iostream>
#include <string>
#include <filesystem>
#include <memory>
#include <thread>

#include <QCoreApplication>

#include "segment.h"

class Until {
public:
    std::string imgPath;
    double threshold;
    std::string onPrevious;
    double interval;
    double timeout;
    double finishWait;
    bool reverse;
    std::string mode;
    std::vector<Segment> targetSegmentList;

    explicit Until(
            double threshold = 0.9,
            std::string onPrevious = "none",
            double interval = 0.1,
            double finishWait = 0,
            double timeout = -1,
            bool reverse = false,
            std::string mode = "gray"
    );

    void loop(std::unique_ptr<Segment> &previous, int globalTimeout);
    bool fulfilled(std::unique_ptr<Segment> &previous);
    [[nodiscard]] std::vector<Segment> filter(const std::vector<Segment> &positions, std::unique_ptr<Segment> &previous) const;
    Until operator~();
    virtual void pre_hook(std::unique_ptr<Segment> &previous);
    virtual bool flag(std::unique_ptr<Segment> &previous);
    [[nodiscard]] virtual std::string toString() const;
    [[nodiscard]] virtual std::string toRepr() const;
};

class UntilImage : public Until {
public:
    explicit UntilImage(
            const std::string &imgPath,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    bool flag(std::unique_ptr<Segment> &previous) override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string toRepr() const override;
};

class UntilAnyImage : public Until {
public:
    std::vector<std::string> imgPathList;

    UntilAnyImage(
            const std::initializer_list<const std::string> &imgList,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    bool flag(std::unique_ptr<Segment> &previous) override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string toRepr() const override;
};

class UntilImageStable : public Until {
public:
    std::vector<Segment> positions;

    explicit UntilImageStable(
            std::string imgPath,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.5,
            double timeout = -1
    );

    void pre_hook(std::unique_ptr<Segment> &previous) override;
    bool flag(std::unique_ptr<Segment> &previous) override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string toRepr() const override;
};

#endif // QT_UNTIL_H