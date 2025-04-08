#include "segment.h"

Segment::Segment(
        QString p,
        float sim,
        int width,
        int height,
        int x1,
        int y1
) : path(std::move(p)), similarity(sim), w(width), h(height), x1(x1), y1(y1) {
    x2 = x1 + w;
    y2 = y1 + h;
    xCenter = x1 + w / 2;
    yCenter = y1 + h / 2;
}

void Segment::click(float wait, int offsetX, int offsetY, Click position) const {
    if (env.stopFlag->load()) return;

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

    x += static_cast<int>(static_cast<float>(offsetX));
    y += static_cast<int>(static_cast<float>(offsetY));

    Mouse::leftDown(env.hwnd, x, y);
    Mouse::leftUp(env.hwnd, x, y);

    emit env.emitter->log(QString::fromStdString("点击: (%1,%2)").arg(QString::number(x), QString::number(y)));

    sleep(env.stopFlag, wait);
}

void Segment::drag(float wait, int distance) const {
    if (env.stopFlag->load()) return;

    int xStart = xCenter;
    int yStart = yCenter;

    int xEnd = xStart;
    int yEnd = yStart + static_cast<int>(static_cast<float>(distance));

    Mouse::drag(env.hwnd, xStart, yStart, xEnd, yEnd);

    emit env.emitter->log(
            QString::fromStdString("拖动: 从(%1,%2)到(%3,%4)").arg(
                    QString::number(xStart),
                    QString::number(yStart),
                    QString::number(xEnd),
                    QString::number(yEnd)
            )
    );
    sleep(env.stopFlag, wait);
}

QString Segment::on(const Segment &segment, const QString &basis) const {
    if (basis == "vertical") {
        if (xCenter <= segment.x1) {
            return "left";
        } else if (xCenter >= segment.x2) {
            return "right";
        } else if (segment.x2 >= xCenter && xCenter >= segment.x1) {
            return "center";
        } else {
            return "none";
        }
    } else if (basis == "horizontal") {
        if (yCenter <= segment.y1) {
            return "top";
        } else if (yCenter >= segment.y2) {
            return "down";
        } else if (segment.y2 >= yCenter && yCenter >= segment.y1) {
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

bool operator==(const Segment &a, const Segment &b) {
    return (a.xCenter == b.xCenter) && (a.yCenter == b.yCenter);
}

bool operator!=(const Segment &a, const Segment &b) {
    return !(a == b);
}

QString Segment::toString() const {
    return QString("(%2, %3)<img src='%1' height='14'>").arg(
            path,
            QString::number(xCenter),
            QString::number(yCenter)
    );
}

Segment similaritySelector(const std::vector<Segment> &segments) {
    if (segments.empty()) throw std::runtime_error("similaritySelector: 列表为空");

    auto result = segments;
    std::sort(result.begin(), result.end(), [](const Segment &a, const Segment &b) {
        return a.similarity > b.similarity;
    });

    return result[0];
}

Selector positionSelector(const QString &attribute, const QString &option) {
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
        const QString &attribute,
        const QString &option,
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
                sortedSegments.begin() + static_cast<long long>((std::min)(top, sortedSegments.size()))
        );

        return choice(new_vector);
    };

    return selectorFunc;
}