// segment.h
#ifndef QT_SEGMENT_H
#define QT_SEGMENT_H

#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <functional>
#include <opencv2/opencv.hpp>

#include <QCoreApplication>

struct Segment {
    std::string path;
    double similarity;
    int w;
    int h;
    int x1;
    int y1;
    int x2;
    int y2;
    int x_center;
    int y_center;

    Segment(
            std::string p,
            double sim,
            int width,
            int height,
            int x, int y
    );

    void click(double wait = 0.1) const;

    [[nodiscard]] std::string on(const Segment &segment, const std::string &basis) const;

    [[nodiscard]] Segment copy() const;

    [[nodiscard]] virtual std::string toString() const;
    [[nodiscard]] virtual std::string toRepr() const;
};

template<typename T>
T choice(const std::vector<T> &vec) {
    if (vec.empty()) {
        throw std::runtime_error("向量不能为空");
    }

    std::random_device rd;  // 非确定性随机数生成器
    std::mt19937 gen(rd()); // 以随机设备作为种子的Mersenne Twister伪随机数生成器
    std::uniform_int_distribution<> dis(0, vec.size() - 1); // 均匀分布

    int index = dis(gen); // 生成随机索引
    return vec[index]; // 返回随机选择的元素
}

using Selector = std::function<Segment(const std::vector<Segment> &)>;

Segment similarity_selector(const std::vector<Segment> &segments);

Selector position_selector(const std::string &attribute, const std::string &option);

Segment random_selector(const std::vector<Segment> &segments);

#endif // QT_SEGMENT_H