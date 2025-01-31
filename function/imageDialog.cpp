#include "imageDialog.h"

ImageDialog::ImageDialog(QWidget *parent) : QDialog(parent), imageLabel(new QLabel(this)) {
    this->setWindowTitle("查看截屏");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto screenshot = CV::getScreen(Mode::RGB);
    cv::cvtColor(screenshot, screenshot, cv::COLOR_BGR2RGB);
    auto image = QImage(
            screenshot.data,
            screenshot.cols,
            screenshot.rows,
            static_cast<int>(screenshot.step),
            QImage::Format_RGB888
    );

    int maxWidth = 400;
    int maxHeight = 400;

    auto *layout = new QVBoxLayout(this);
    imageLabel->setMaximumSize(maxWidth, maxHeight);
    layout->addWidget(imageLabel);

    int originalWidth = image.width();
    int originalHeight = image.height();

    float scaleWidth = static_cast<float>(maxWidth) / originalWidth;
    float scaleHeight = static_cast<float>(maxHeight) / originalHeight;

    float scale = qMin(scaleWidth, scaleHeight);
    int scaledWidth = static_cast<int>(originalWidth * scale);
    int scaledHeight = static_cast<int>(originalHeight * scale);

    QImage scaledImage = image.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
}