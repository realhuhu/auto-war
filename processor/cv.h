#ifndef RED_CV_H
#define RED_CV_H

#include <windows.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QFile>
#include <QString>
#include <QCoreApplication>

#include "../util/enum.h"
#include "../util/state.h"
#include "segment.h"

class CV {
public:
    static cv::Mat getScreen(HWND hwnd, Mode mode = Mode::GRAY);

    static std::vector<Segment> findPositions(
            const cv::Mat &rawImg,
            const std::string &templatePath,
            float threshold = 0.9,
            Mode mode = Mode::GRAY
    );
};


#endif //RED_CV_H
