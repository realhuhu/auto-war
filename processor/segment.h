#ifndef RED_SEGMENT_H
#define RED_SEGMENT_H

#include <random>
#include <vector>

#include "env.h"
#include "../util/tool.h"
#include "../util/enum.h"
#include "../util/state.h"
#include "../util/action.h"

class Segment {
public:
    std::string path;
    float similarity;
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
            float sim,
            int width,
            int height,
            int x, int y
    );

    void click(float wait = 0.1, int offsetX = 0, int offsetY = 0, Click position = Click::CENTER) const;

    void drag(float wait = 0.1, int distance = 0) const;

    [[nodiscard]] std::string on(const Segment &segment, const std::string &basis) const;

    [[nodiscard]] Segment copy() const;

    friend bool operator==(const Segment &a, const Segment &b);

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

#endif //RED_SEGMENT_H
