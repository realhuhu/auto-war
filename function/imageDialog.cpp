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

    auto *layout = new QVBoxLayout(this);
    imageLabel->setFixedWidth(400);
    imageLabel->setFixedHeight(300);
    layout->addWidget(imageLabel);

    QImage scaledImage = image.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
}