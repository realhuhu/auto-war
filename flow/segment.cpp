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
    xCenter = x1 + w / 2;
    yCenter = y1 + h / 2;
}

void Segment::click(double wait, int offsetX, int offsetY, Click position) const {
    if (state.stopFlag.load()) return;

    int x, y;

    switch (position) {
        case Click::CENTER:
            x = xCenter;
            y = yCenter;
            break;
        case Click::LEFT:
            x = x1;
            y = yCenter;
            break;
        case Click::TOP:
            x = xCenter;
            y = y1;
            break;
        case Click::RIGHT:
            x = x2;
            y = yCenter;
            break;
        case Click::DOWN:
            x = xCenter;
            y = y2;
            break;
    }

    x += offsetX * state.scale;
    y += offsetY * state.scale;
    LPARAM lparam = (y << 16) | x;
    PostMessageW(state.hwnd, WM_LBUTTONDOWN, 0, lparam);
    PostMessageW(state.hwnd, WM_LBUTTONUP, 0, lparam);

    emit Emitter::instance()->log(QString::fromStdString("点击: (%1,%2)").arg(QString::number(x), QString::number(y)));

    if (wait > 0) std::this_thread::sleep_for(std::chrono::duration<float>(wait));
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
    return QString(
            "(%3, %4)<img src='%1' height='14'>"
    ).arg(
            QString::fromStdString(this->path),
            QString::number(this->xCenter),
            QString::number(this->yCenter)
    ).toStdString();
}

Segment similaritySelector(const std::vector<Segment> &segments) {
    if (segments.empty()) throw std::runtime_error("similaritySelector: 列表为空");

    auto result = segments;
    std::sort(result.begin(), result.end(), [](const Segment &a, const Segment &b) {
        return a.similarity > b.similarity;
    });

    return result[0];
}

Selector positionSelector(const std::string &attribute, const std::string &option) {
    auto selectorFunc = [attribute, option](const std::vector<Segment> &segments) {
        if (segments.empty()) throw std::runtime_error("positionSelector: 列表为空");

        std::vector<Segment> sortedSegments = segments;

        auto compare = [&](const Segment &a, const Segment &b) {
            if (attribute == "x1") return a.x1 < b.x1;
            if (attribute == "y1") return a.y1 < b.y1;
            if (attribute == "x2") return a.x2 < b.x2;
            if (attribute == "y2") return a.y2 < b.y2;
            if (attribute == "xCenter") return a.xCenter < b.xCenter;
            if (attribute == "yCenter") return a.yCenter < b.yCenter;
            throw std::invalid_argument("Unknown attribute");
        };

        std::sort(sortedSegments.begin(), sortedSegments.end(), compare);

        if (option == "max") std::reverse(sortedSegments.begin(), sortedSegments.end());

        return sortedSegments[0];
    };
    return selectorFunc;
}

Segment randomSelector(const std::vector<Segment> &segments) {
    if (segments.empty()) throw std::runtime_error("randomSelector: 列表为空");

    return choice(segments);
}

Selector orderedRandomSelector(
        const std::string &attribute,
        const std::string &option,
        size_t top
) {
    auto selectorFunc = [attribute, option, top](const std::vector<Segment> &segments) {
        if (segments.empty()) throw std::runtime_error("randomSelector: 列表为空");

        auto compare = [&](const Segment &a, const Segment &b) {
            if (attribute == "x1") return a.x1 < b.x1;
            if (attribute == "y1") return a.y1 < b.y1;
            if (attribute == "x2") return a.x2 < b.x2;
            if (attribute == "y2") return a.y2 < b.y2;
            if (attribute == "xCenter") return a.xCenter < b.xCenter;
            if (attribute == "yCenter") return a.yCenter < b.yCenter;
            throw std::invalid_argument("Unknown attribute");
        };

        std::vector<Segment> sortedSegments = segments;

        std::sort(sortedSegments.begin(), sortedSegments.end(), compare);

        if (option == "max") std::reverse(sortedSegments.begin(), sortedSegments.end());

        std::vector<Segment> new_vector(
                sortedSegments.begin(),
                sortedSegments.begin() + std::min(top, sortedSegments.size())
        );

        return choice(new_vector);
    };

    return selectorFunc;
}