// qt_cv.h
#ifndef QT_CV_H
#define QT_CV_H

#include <windows.h>
#include <QDir>
#include <QFile>
#include <QString>
#include <QCoreApplication>
#include <opencv2/opencv.hpp>

#include "until.h"
#include "segment.h"

class CV {
public:
    static cv::Mat get_screen(const std::string &mode = "gray");
    static std::vector<Segment> find_positions(
            const cv::Mat &rawImg,
            const std::string &templatePath,
            double threshold = 0.9,
            const std::string &mode = "gray"
    );
};

#endif // QT_CV_H