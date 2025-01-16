#ifndef QT_SEGMENT_H
#define QT_SEGMENT_H

#include <random>
#include <vector>
#include <thread>
#include <iostream>

#include <QDebug>
#include <opencv2/opencv.hpp>

#include "state.h"

// 定义Segment结构体
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
    ) : path(std::move(p)), similarity(sim), w(width), h(height), x1(x), y1(y) {
        x2 = x1 + w;
        y2 = y1 + h;
        x_center = x1 + w / 2;
        y_center = y1 + h / 2;
    }

    // 点击函数
    void click(double wait = 0.1) const {
        int x = x_center;
        int y = y_center;
        LPARAM lparam = (y << 16) | x;
        PostMessageW(state.hwnd, WM_LBUTTONDOWN, 0, lparam);
        std::this_thread::sleep_for(std::chrono::duration<float>(0.1));
        PostMessageW(state.hwnd, WM_LBUTTONUP, 0, lparam);

        qDebug() << QString::fromStdString("点击: " + this->toString());

        if (wait > 0) {
            std::this_thread::sleep_for(std::chrono::duration<float>(wait));
        }
    }

    // 判断位置关系函数
    [[nodiscard]] std::string on(const Segment &segment, const std::string &basis) const {
        if (basis == "vertical") {
            if (x2 <= segment.x1) {
                return "left";
            } else if (x1 >= segment.x2) {
                return "right";
            } else if (x1 >= segment.x1 && x2 <= segment.x2) {
                return "center";
            } else {
                return "none";
            }
        } else if (basis == "horizontal") {
            if (y2 <= segment.y1) {
                return "top";
            } else if (y1 >= segment.y2) {
                return "down";
            } else if (y1 >= segment.y1 && y2 <= segment.y2) {
                return "center";
            } else {
                return "none";
            }
        }
        return "none";
    }

    [[nodiscard]] Segment copy() const {
        return {path, similarity, w, h, x1, y1};
    }

    [[nodiscard]] virtual std::string toString() const {
        std::filesystem::path p(this->path);
        return "[" + p.stem().string() +
               "(" + std::to_string(this->x_center) + ", " + std::to_string(this->y_center) +
               ")]";
    }

    [[nodiscard]] virtual std::string toRepr() const {
        return toString();
    }
};


template<typename T>
T choice(const std::vector<T>& vec) {
    if (vec.empty()) {
        throw std::runtime_error("向量不能为空");
    }

    std::random_device rd;  // 非确定性随机数生成器
    std::mt19937 gen(rd()); // 以随机设备作为种子的Mersenne Twister伪随机数生成器
    std::uniform_int_distribution<> dis(0, vec.size() - 1); // 均匀分布

    int index = dis(gen); // 生成随机索引
    return vec[index]; // 返回随机选择的元素
}

// 定义Selector类型，使用std::function模拟Python中的Callable
using Selector = std::function<Segment(const std::vector<Segment> &)>;

// 相似度选择器
Segment similarity_selector(const std::vector<Segment> &segments) {
    if (segments.empty()) {
        throw std::runtime_error("similarity_selector::No segments");
    };


    auto result = segments;
    std::sort(result.begin(), result.end(), [](const Segment &a, const Segment &b) {
        return a.similarity > b.similarity;
    });

    return result[0];
}

// 属性选择器
Selector position_selector(const std::string &attribute, const std::string &option) {
    auto selector_func = [attribute, option](const std::vector<Segment> &segments) {
        if (segments.empty()) {
            throw std::runtime_error("position_selector::No segments");
        };

        std::vector<Segment> sorted_segments = segments;

        auto compare = [&](const Segment &a, const Segment &b) {
            if (attribute == "x1") return a.x1 < b.x1;
            if (attribute == "y1") return a.y1 < b.y1;
            if (attribute == "x2") return a.x2 < b.x2;
            if (attribute == "y2") return a.y2 < b.y2;
            if (attribute == "x_center") return a.x_center < b.x_center;
            if (attribute == "y_center") return a.y_center < b.y_center;
            throw std::invalid_argument("Unknown attribute");
        };

        std::sort(sorted_segments.begin(), sorted_segments.end(), compare);

        if (option == "max") {
            std::reverse(sorted_segments.begin(), sorted_segments.end());
        }
        return sorted_segments[0];
    };
    return selector_func;
}

std::random_device rd;

// 随机选择器
Segment random_selector(const std::vector<Segment> &segments) {
    if (segments.empty()) {
        throw std::runtime_error("random_selector::No segments");
    };

    return choice(segments);
}

#endif //QT_SEGMENT_H
