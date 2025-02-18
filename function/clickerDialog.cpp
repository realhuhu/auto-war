#include "clickerDialog.h"

ClickThread::ClickThread(
        const std::vector<POINT> &points,
        HWND hwnd,
        int interval,
        int rounds,
        QObject *parent
) : QThread(parent), pointList(points), hwnd(hwnd), interval(interval), rounds(rounds) {}

void ClickThread::stop() { stopFlag.store(true); }

void ClickThread::run() {
    auto current = rounds;
    while (!stopFlag.load()) {
        if (rounds) emit logText(QString("轮次: %1/%2").arg(
                    QString::number(rounds - current + 1),
                    QString::number(rounds))
            );

        for (auto &point: pointList) {
            Mouse::leftDown(hwnd, point.x, point.y);
            Mouse::leftUp(hwnd, point.x, point.y);

            auto sleepDuration = std::chrono::milliseconds(interval);
            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < sleepDuration) {
                if (stopFlag.load()) return;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        current--;

        if (current == 0) {
            emit logText("运行完成");
            return;
        }
    }
}

ClickerDialog::ClickerDialog(QWidget *parent) : QDialog(parent) {
    instance = this;
    this->setWindowTitle("设置连点器");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *mainLayout = new QVBoxLayout(this);

    auto *buttonLayout = new QHBoxLayout(this);

    auto startRecordButton = new QPushButton("开始录制", this);
    auto endRecordButton = new QPushButton("结束录制", this);
    auto startClickButton = new QPushButton("循环点击", this);
    auto endClickButton = new QPushButton("结束点击", this);

    connect(startRecordButton, &QPushButton::clicked, this, &ClickerDialog::startRecord);
    connect(endRecordButton, &QPushButton::clicked, this, &ClickerDialog::endRecord);
    connect(startClickButton, &QPushButton::clicked, this, &ClickerDialog::startClick);
    connect(endClickButton, &QPushButton::clicked, this, &ClickerDialog::endClick);

    buttonLayout->addWidget(startRecordButton);
    buttonLayout->addWidget(endRecordButton);
    buttonLayout->addWidget(startClickButton);
    buttonLayout->addWidget(endClickButton);

    mainLayout->addLayout(buttonLayout);

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setMinimumWidth(360);

    mainLayout->addWidget(textEdit);

    auto *footLayout = new QHBoxLayout();

    auto *intervalLabel = new QLabel("间隔", this);
    intervalSpinBox = new QSpinBox(this);
    intervalSpinBox->setRange(1, 10000);
    intervalSpinBox->setValue(200);
    auto *millisecondLabel = new QLabel("毫秒", this);

    auto *roundsLabel = new QLabel("重复", this);
    roundsSpinBox = new QSpinBox(this);
    roundsSpinBox->setRange(0, 100000);
    roundsSpinBox->setValue(0);
    auto *roundsUnitLabel = new QLabel("轮", this);

    footLayout->addWidget(intervalLabel);
    footLayout->addWidget(intervalSpinBox);
    footLayout->addWidget(millisecondLabel);
    footLayout->addStretch();
    footLayout->addWidget(roundsLabel);
    footLayout->addWidget(roundsSpinBox);
    footLayout->addWidget(roundsUnitLabel);

    mainLayout->addLayout(footLayout);

    this->setLayout(mainLayout);
}

void ClickerDialog::updateTextEdit(const QString &text) { textEdit->append(text); }

[[maybe_unused]] void ClickerDialog::appendCoordinate(int x, int y) {
    RECT clientRect = {0};
    POINT topLeft = {0, 0};

    if (GetClientRect(state.hwnd, &clientRect)) {
        ClientToScreen(state.hwnd, &topLeft);

        int left = topLeft.x;
        int top = topLeft.y;

        textEdit->append(QString("绝对坐标: (%1, %2), 窗口内坐标: (%4, %5)").arg(
                QString::number(x),
                QString::number(y),
                QString::number(x - left),
                QString::number(y - top)
        ));

        pointList.push_back({x - left, y - top});
    }
}

void ClickerDialog::startRecord() {
    if (isWaiting) {
        textEdit->append("正在录制中");
        return;
    }

    pointList.clear();
    textEdit->clear();

    textEdit->append("请按顺序点击");
    isWaiting = true;

    if (hook) {
        UnhookWindowsHookEx(hook);
        hook = nullptr;
    }
    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);
    if (!hook) textEdit->append("钩子安装失败");
}

void ClickerDialog::endRecord() {
    if (!isWaiting) {
        textEdit->append("还未开始录制");
        return;
    }

    isWaiting = false;
    if (hook) {
        UnhookWindowsHookEx(hook);
        hook = nullptr;
    }
    textEdit->append("已结束录制");
}

void ClickerDialog::startClick() {
    if (clickThread) {
        textEdit->append("正在连点中");
        return;
    }

    if (isWaiting) {
        textEdit->append("请先完成录制");
        return;
    }

    if (pointList.empty()) {
        textEdit->append("请先录制");
        return;
    }

    textEdit->clear();

    auto text = QString("循环点击(间隔%1毫秒, %2轮): ").arg(
            QString::number(intervalSpinBox->value()),
            roundsSpinBox->value() > 0 ? QString::number(roundsSpinBox->value()) : "无限"
    );

    for (auto &point: pointList) {
        text += QString("(%1, %2)").arg(point.x).arg(point.y);
    }

    textEdit->append(text);

    clickThread = new ClickThread(
            pointList, state.hwnd,
            intervalSpinBox->value(),
            roundsSpinBox->value(),
            this
    );
    connect(clickThread, &QThread::finished, clickThread, &QThread::deleteLater);
    connect(clickThread, &QThread::destroyed, this, [this]() { clickThread = nullptr; });
    connect(clickThread, &ClickThread::logText, this, &ClickerDialog::updateTextEdit);
    clickThread->start();
}

void ClickerDialog::endClick() {
    if (!clickThread) {
        textEdit->append("当前未开始连点");
        return;
    }

    textEdit->append("正在终止");
    clickThread->stop();
    clickThread->quit();
    clickThread->wait();
    textEdit->append("已结束点击");
}

void ClickerDialog::closeEvent(QCloseEvent *) {
    if (hook) UnhookWindowsHookEx(hook);

    if (clickThread) {
        clickThread->stop();
        clickThread->quit();
        clickThread->wait();
    }

    instance = nullptr;

    parentWidget()->showNormal();
}

LRESULT CALLBACK ClickerDialog::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_LBUTTONDOWN && instance && instance->isWaiting) {
        auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
        HWND hwnd = WindowFromPoint(pMouseStruct->pt);

        if (hwnd == state.hwnd) {
            QMetaObject::invokeMethod(
                    instance,
                    "appendCoordinate",
                    Qt::QueuedConnection,
                    Q_ARG(int, pMouseStruct->pt.x),
                    Q_ARG(int, pMouseStruct->pt.y)
            );
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

ClickerDialog *ClickerDialog::instance = nullptr;

