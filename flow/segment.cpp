// segment.cpp
#include "segment.h"
#include <random>
#include <algorithm>
#include <stdexcept>
#include <windows.h>

#include "../state.h"
#include "emitter.h"

Segment::Segment(
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

void Segment::click(double wait, int offset_x, int offset_y) const {
    int x = x_center + offset_x * state.scale;
    int y = y_center + offset_y * state.scale;
    LPARAM lparam = (y << 16) | x;
    PostMessageW(state.hwnd, WM_LBUTTONDOWN, 0, lparam);
    PostMessageW(state.hwnd, WM_LBUTTONUP, 0, lparam);

    emit Emitter::instance()->log(QString::fromStdString("点击: (%1,%2)").arg(QString::number(x), QString::number(y)));

    if (wait > 0) {
        std::this_thread::sleep_for(std::chrono::duration<float>(wait));
    }
}

std::string Segment::on(const Segment &segment, const std::string &basis) const {
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

Segment Segment::copy() const {
    return {path, similarity, w, h, x1, y1};
}

std::string Segment::toString() const {
    std::filesystem::path p(this->path);
    return "[" + p.stem().string() +
           "(" + std::to_string(this->x_center) + ", " + std::to_string(this->y_center) +
           ")]";
}


Segment similarity_selector(const std::vector<Segment> &segments) {
    if (segments.empty()) {
        throw std::runtime_error("similarity_selector: 列表为空");
    }

    auto result = segments;
    std::sort(result.begin(), result.end(), [](const Segment &a, const Segment &b) {
        return a.similarity > b.similarity;
    });

    return result[0];
}

Selector position_selector(const std::string &attribute, const std::string &option) {
    auto selector_func = [attribute, option](const std::vector<Segment> &segments) {
        if (segments.empty()) {
            throw std::runtime_error("position_selector: 列表为空");
        }

        std::vector<Segment> sorted_segments = segments;

        for (const auto &i: segments) {
            emit Emitter::instance()->log(QString::fromStdString(i.toString()));
        }

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

        emit Emitter::instance()->log(QString::fromStdString(sorted_segments[0].toString()) + "selected");

        return sorted_segments[0];
    };
    return selector_func;
}

Segment random_selector(const std::vector<Segment> &segments) {
    if (segments.empty()) {
        throw std::runtime_error("random_selector: 列表为空");
    }

    return choice(segments);
}