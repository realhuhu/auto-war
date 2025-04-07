#ifndef RED_UNTIL_H
#define RED_UNTIL_H

#include <QString>

#include "cv.h"
#include "env.h"
#include "segment.h"
#include "../util/enum.h"
#include "../util/state.h"
#include "../util/tool.h"

struct UntilConfig {
    Previous onPrevious = Previous::NONE;
    Mode mode = Mode::GRAY;
    float threshold = 0.9;
    float interval = 0.1;
    float startWait = 0;
    float finishWait = 0;
    float timeout = -1;
    bool reverse = false;
};

class Until {
public:
    QString imgPath;
    UntilConfig config;
    std::vector<Segment> targetSegmentList;

    explicit Until(UntilConfig untilConfig = {});

    virtual void loop(std::unique_ptr<Segment> &previous, float globalTimeout);

    bool fulfilled(std::unique_ptr<Segment> &previous);

    [[nodiscard]] std::vector<Segment> filter(
            const std::vector<Segment> &positions,
            std::unique_ptr<Segment> &previous
    ) const;

    virtual void preHook(std::unique_ptr<Segment> &previous);

    [[nodiscard]] virtual bool flag(std::unique_ptr<Segment> &previous) = 0;

    [[nodiscard]] virtual QString toString() const = 0;
};

class Image : public Until {
public:
    explicit Image(
            QString templatePath,
            UntilConfig config = {}
    );

    bool flag(std::unique_ptr<Segment> &previous) override;

    [[nodiscard]] QString toString() const override;
};

class AnyImage : public Until {
public:
    std::vector<QString> imgPathList;

    explicit AnyImage(
            const std::vector<QString> &templatePathList,
            UntilConfig config = {}
    );

    bool flag(std::unique_ptr<Segment> &previous) override;

    [[nodiscard]] QString toString() const override;
};

class ImageStable : public Until {
public:
    std::vector<Segment> positions;

    explicit ImageStable(
            QString templatePath,
            UntilConfig config = {}
    );

    void preHook(std::unique_ptr<Segment> &previous) override;

    bool flag(std::unique_ptr<Segment> &previous) override;

    [[nodiscard]] QString toString() const override;
};

class IfImage : public Image {
public:
    explicit IfImage(
            const QString &imgPath,
            UntilConfig config = {}
    );

    [[nodiscard]] QString toString() const override;

    void loop(std::unique_ptr<Segment> &previous, float globalTimeout) override;
};

class IfAnyImage : public AnyImage {
public:
    explicit IfAnyImage(
            const std::vector<QString> &templatePathList,
            UntilConfig config = {}
    );

    [[nodiscard]] QString toString() const override;

    void loop(std::unique_ptr<Segment> &previous, float globalTimeout) override;
};

#endif //RED_UNTIL_H
