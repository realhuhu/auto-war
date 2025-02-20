#include "replaceDialog.h"


ScreenshotArea::ScreenshotArea(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);
    grabKeyboard();

    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, [this]() {
        updateScreenCache();
        update();
    });
    refreshTimer->start(33);
    frameTimer.start();
}

ScreenshotArea::~ScreenshotArea() {
    refreshTimer->stop();
}

void ScreenshotArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        origin = event->pos();
        current = origin;
        selecting = true;
    }
}


void ScreenshotArea::mouseMoveEvent(QMouseEvent *event) {
    mousePos = event->pos();
    if (selecting) {
        current = mousePos;
    }
    update();
}

void ScreenshotArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && selecting) {
        selecting = false;
        emit screenshotCaptured(origin, current);
        close();
    }
}

void ScreenshotArea::updateScreenCache() {
    if (frameTimer.elapsed() > 50) {
        cachedScreen = QGuiApplication::primaryScreen()->grabWindow(
                0,
                mousePos.x() - 25,
                mousePos.y() - 25,
                50,
                50
        );
        frameTimer.restart();
    }
}


void ScreenshotArea::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QColor(0, 0, 0, 16));
    painter.drawRect(rect());

    const int zoomSize = 25;
    const int zoomScale = 4;
    const int magnifierSize = zoomSize * 2 * zoomScale;

    if (!cachedScreen.isNull() && !mousePos.isNull()) {
        QPixmap magnified = cachedScreen.scaled(
                magnifierSize,
                magnifierSize,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
        );

        static QPoint lastDrawPos;
        QPoint drawPos = mousePos + QPoint(25, 25);
        if ((drawPos - lastDrawPos).manhattanLength() > 5) {
            if (drawPos.x() + magnifierSize > width())
                drawPos.setX(mousePos.x() - 25 - magnifierSize);
            if (drawPos.y() + magnifierSize > height())
                drawPos.setY(mousePos.y() - 25 - magnifierSize);
            lastDrawPos = drawPos;
        }

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(drawPos, magnified);

        painter.setPen(QPen(Qt::blue, 1));
        painter.drawRect(QRect(drawPos, magnified.size()));

        QPoint center = drawPos + QPoint(magnified.width() / 2, magnified.height() / 2);
        painter.setPen(QPen(Qt::cyan, 1, Qt::DotLine));
        painter.drawLine(center - QPoint(zoomSize * zoomScale, 0),
                         center + QPoint(zoomSize * zoomScale, 0));
        painter.drawLine(center - QPoint(0, zoomSize * zoomScale),
                         center + QPoint(0, zoomSize * zoomScale));
    }


    if (selecting) {
        QRect selectRect = QRect(origin, current);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.drawRect(selectRect);

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(QPen(Qt::red, 1));
        painter.drawRect(selectRect);
    }
}

void ScreenshotArea::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void ScreenshotArea::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    QApplication::setOverrideCursor(Qt::CrossCursor);
}

void ScreenshotArea::closeEvent(QCloseEvent *event) {
    QApplication::restoreOverrideCursor();
    QWidget::closeEvent(event);
}

ReplaceDialog::ReplaceDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("替换图片");
    auto mainLayout = new QVBoxLayout(this);
    this->setMinimumHeight(300);

    auto style = R"(
        QLabel {
            border: 1px solid #aaa;
            border-radius: 2px; /* 设置圆角 */
            padding-top: 5px;
            padding-bottom: 5px;
        }
    )";

    auto topLayout = new QHBoxLayout();

    auto openFileButton = new QPushButton("选择图片", this);
    openFileButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(openFileButton, &QPushButton::clicked, this, &ReplaceDialog::openFile);

    fileLabel = new QLabel(this);

    auto captureButton = new QPushButton("截图", this);
    captureButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(captureButton, &QPushButton::clicked, this, &ReplaceDialog::captureScreenshot);

    topLayout->addWidget(openFileButton);
    topLayout->addStretch();
    topLayout->addWidget(fileLabel);
    topLayout->addStretch();
    topLayout->addWidget(captureButton);

    mainLayout->addLayout(topLayout);

    auto middleLayout = new QHBoxLayout();

    selectedImageLabel = new QLabel(this);
    selectedImageLabel->setStyleSheet(style);
    selectedImageLabel->setMinimumWidth(200);
    selectedImageLabel->setMinimumHeight(200);

    auto replaceLabel = new QLabel("替换为", this);
    replaceLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    croppedImageLabel = new QLabel(this);
    croppedImageLabel->setStyleSheet(style);
    croppedImageLabel->setMinimumWidth(200);
    croppedImageLabel->setMinimumHeight(200);

    middleLayout->addWidget(selectedImageLabel);
    middleLayout->addWidget(replaceLabel);
    middleLayout->addWidget(croppedImageLabel);

    mainLayout->addLayout(middleLayout);

    auto bottomLayout = new QHBoxLayout();

    tipLabel = new QLabel(this);
    tipLabel->setStyleSheet("color: red;");

    auto okButton = new QPushButton("确定", this);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(okButton, &QPushButton::clicked, this, &ReplaceDialog::saveFile);

    bottomLayout->addWidget(tipLabel);
    bottomLayout->addWidget(okButton);

    mainLayout->addLayout(bottomLayout);
}

void ReplaceDialog::openFile() {
    auto resDir = QCoreApplication::applicationDirPath() + "/res/";
    QFileDialog fileDialog(this);
    fileDialog.setDirectory(resDir);
    fileDialog.setNameFilters(QStringList() << "PNG Images (*.png)");

    if (fileDialog.exec() == QFileDialog::Accepted) {
        QString filePath = fileDialog.selectedFiles().first();

        if (!filePath.startsWith(resDir)) {
            tipLabel->setText("只能替换/res文件夹下的图片");
            return;
        }

        if (filePath.endsWith("1.png")) {
            filePath.replace("1.png", ".png");
            tipLabel->setText("xxx1.png无需替换，已换为同名图片");
        }
        selectedPath = filePath;

        QPixmap pixmap(filePath);
        fileLabel->setText(filePath.replace(resDir, ""));
        if (!pixmap.isNull()) {
            selectedImageLabel->setPixmap(pixmap.scaled(selectedImageLabel->size(), Qt::KeepAspectRatio));
        }
    }
}

void ReplaceDialog::captureScreenshot() const {
    auto *area = new ScreenshotArea();
    connect(area, &ScreenshotArea::screenshotCaptured, this, &ReplaceDialog::showScreenshot);
    area->showFullScreen();
}

void ReplaceDialog::showScreenshot(const QPoint p1, const QPoint p2) {
    RECT rect;
    POINT topLeft = {0, 0};

    if (!GetClientRect(state.hwnd, &rect)) {
        tipLabel->setText("获取窗口识别");
        return;
    }

    ClientToScreen(state.hwnd, &topLeft);
    int left = topLeft.x;
    int top = topLeft.y;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int x1 = (p1.x() - left) / state.scale;
    int y1 = (p1.y() - top) / state.scale;
    int x2 = (p2.x() - left) / state.scale;
    int y2 = (p2.y() - top) / state.scale;

    if (x1 < 0 || x1 > width || x2 < 0 || x2 > width || y1 < 0 || y1 > height || y2 < 0 || y2 > height) {
        tipLabel->setText("截图区域必须在窗口内!");
        return;
    }

    int x_start = std::min(x1, x2);
    int x_end = std::max(x1, x2);
    int y_start = std::min(y1, y2);
    int y_end = std::max(y1, y2);

    auto screen = CV::getScreen(Mode::RGB);
    cv::cvtColor(screen, screen, cv::COLOR_BGR2RGB);
    croppedImage = screen(cv::Rect(x_start, y_start, x_end - x_start, y_end - y_start));

    auto image = QImage(
            croppedImage.data,
            croppedImage.cols,
            croppedImage.rows,
            static_cast<int>(croppedImage.step),
            QImage::Format_RGB888
    );

    auto scaledImage = image.scaled(croppedImageLabel->size(), Qt::KeepAspectRatio);

    croppedImageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    tipLabel->clear();
}


void ReplaceDialog::saveFile() {
    if (selectedPath.isEmpty() || croppedImage.empty()) return;

    auto resDir = QCoreApplication::applicationDirPath() + "/res/";
    auto customDir = QCoreApplication::applicationDirPath() + "/自定义图片/";
    auto fileName = selectedPath.replace(resDir, "").replace("/", "-");
    tipLabel->setText("已保存到自定义图片文件夹: " + fileName);

    auto image = QImage(
            croppedImage.data,
            croppedImage.cols,
            croppedImage.rows,
            static_cast<int>(croppedImage.step),
            QImage::Format_RGB888
    );

    image.save(customDir + fileName);
}


void ReplaceDialog::closeEvent(QCloseEvent *) {
    parentWidget()->showNormal();
}
