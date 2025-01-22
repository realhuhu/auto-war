#ifndef QT_UNTIL_H
#define QT_UNTIL_H

#include <vector>
#include <chrono>
#include <string>
#include <memory>
#include <thread>
#include <iostream>
#include <filesystem>

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

    explicit UntilAnyImage(
            const std::vector<std::string>& imgList,
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
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
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
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
            double finishWait = 0,
            double threshold = 0.9,
            double interval = 0.1,
            double timeout = -1
    );

    explicit UntilIfAnyImage(
            const std::vector<std::string>& imgList,
            const std::string &onPrevious = "none",
            bool reverse = false,
            std::string mode = "gray",
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