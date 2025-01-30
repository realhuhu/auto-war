#include "ImageDialog.h"

ImageDialog::ImageDialog(
        const QImage &image,
        QWidget *parent
) : QDialog(parent), imageLabel(new QLabel(this)) {
    this->setWindowTitle("查看截屏");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

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