#ifndef QT_CV_H
#define QT_CV_H

#include <windows.h>

#include <opencv2/opencv.hpp>

#include <QDir>
#include <QFile>
#include <QString>
#include <QCoreApplication>

#include "enum.h"
#include "until.h"
#include "segment.h"

class CV {
public:
    static cv::Mat getScreen(Mode mode = Mode::GRAY);

    static std::vector<Segment> findPositions(
            const cv::Mat &rawImg,
            const std::string &templatePath,
            double threshold = 0.9,
            Mode mode = Mode::GRAY
    );
};

#endif // QT_CV_H