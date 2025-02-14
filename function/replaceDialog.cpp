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
        emit screenshotCaptured(origin, current);
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

    // 第一栏布局
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

    // 第二栏布局
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

    // 第三栏布局
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
