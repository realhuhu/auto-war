#ifndef QT_CV_H
#define QT_CV_H

#include <windows.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QFile>
#include <QString>
#include <QCoreApplication>

#include "../util/state.h"
#include "../util/enum.h"
#include "segment.h"

class CV {
public:
    static cv::Mat getScreen(Mode mode = Mode::GRAY);

    static std::vector<Segment> findPositions(
            const cv::Mat &rawImg,
            const std::string &templatePath,
            float threshold = 0.9,
            Mode mode = Mode::GRAY
    );
};

#endif // QT_CV_H