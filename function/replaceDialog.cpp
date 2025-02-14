#include "replaceDialog.h"

ScreenshotArea::ScreenshotArea(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);
    grabKeyboard();
}

void ScreenshotArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        origin = event->pos();
        current = origin;
        selecting = true;
    }
}


void ScreenshotArea::mouseMoveEvent(QMouseEvent *event) {
// 始终更新鼠标位置
    mousePos = event->pos();
    if (selecting) {
        current = mousePos;
    }
    update();
}

void ScreenshotArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && selecting) {
        selecting = false;
        QRect selection = QRect(origin, current).normalized();

        QScreen *screen = QGuiApplication::primaryScreen();
        QPixmap fullPixmap = screen->grabWindow(0);
        QPixmap cropped = fullPixmap.copy(selection);

        emit screenshotCaptured(cropped);
        close();
    }
}

void ScreenshotArea::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

// 绘制全屏半透明遮罩
    painter.setBrush(QColor(0, 0, 0, 100));
    painter.drawRect(rect());

// 绘制十字线
    painter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    painter.drawLine(QPoint(0, mousePos.y()), QPoint(width(), mousePos.y()));
    painter.drawLine(QPoint(mousePos.x(), 0), QPoint(mousePos.x(), height()));

    if (selecting) {
// 绘制选择区域
        QRect selectRect = QRect(origin, current);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.drawRect(selectRect);

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(QPen(Qt::red, 2));
        painter.drawRect(selectRect);
    }
}

void ScreenshotArea::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}


ReplaceDialog::ReplaceDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("替换图片");
    auto layout = new QVBoxLayout(this);
    auto *btn = new QPushButton("screen", this);

    connect(btn, &QPushButton::clicked, this, &ReplaceDialog::captureScreenshot);

    label = new QLabel(this);
    label->setMinimumWidth(100);

    layout->addWidget(btn);
    layout->addWidget(label);
}

void ReplaceDialog::captureScreenshot() const {
    auto *area = new ScreenshotArea();
    connect(area, &ScreenshotArea::screenshotCaptured, this, &ReplaceDialog::showScreenshot);
    area->showFullScreen();
}

void ReplaceDialog::showScreenshot(const QPixmap &pixmap) {
    label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio));
}

void ReplaceDialog::closeEvent(QCloseEvent *) {
    parentWidget()->showNormal();
}
