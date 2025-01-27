#ifndef QT_SEGMENT_H
#define QT_SEGMENT_H

#include <random>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <functional>
#include <opencv2/opencv.hpp>

#include <QCoreApplication>

#include "enum.h"

struct Segment {
    std::string path;
    double similarity;
    int w;
    int h;
    int x1;
    int y1;
    int x2;
    int y2;
    int xCenter;
    int yCenter;

    Segment(
            std::string p,
            double sim,
            int width,
            int height,
            int x, int y
    );

    void click(double wait = 0.1, int offsetX = 0, int offsetY = 0, Click position = Click::CENTER) const;

    [[nodiscard]] std::string on(const Segment &segment, const std::string &basis) const;

    [[nodiscard]] Segment copy() const;

    [[nodiscard]] virtual std::string toString() const;
};

template<typename T>
T choice(const std::vector<T> &vec) {
    if (vec.empty()) throw std::runtime_error("向量不能为空");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, vec.size() - 1);

    int index = dis(gen);
    return vec[index];
}

using Selector = std::function<Segment(const std::vector<Segment> &)>;

Segment similaritySelector(const std::vector<Segment> &segments);

Selector positionSelector(const std::string &attribute, const std::string &option);

Segment randomSelector(const std::vector<Segment> &segments);

Selector orderedRandomSelector(const std::string &attribute, const std::string &option, size_t top);

#endif // QT_SEGMENT_H