#include "clicker.h"
#include <thread>
#include <chrono>

#include <QLabel>
#include <QMessageBox>
#include <QApplication>

# include "../state.h"

ClickThread::ClickThread(
        const std::vector<POINT> &points,
        HWND hwnd,
        int interval,
        QObject *parent
) : QThread(parent), pointList(points), hwnd(hwnd), interval(interval) {}

void ClickThread::stop() { stopFlag.store(true); }

void ClickThread::run() {
    while (!stopFlag.load()) {
        for (auto &point: pointList) {
            LPARAM lparam = (point.y << 16) | point.x;
            PostMessageW(hwnd, WM_LBUTTONDOWN, 0, lparam);
            PostMessageW(hwnd, WM_LBUTTONUP, 0, lparam);
            // 将 sleep_for 拆分为多个小周期
            auto sleepDuration = std::chrono::milliseconds(interval);
            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < sleepDuration) {
                if (stopFlag.load()) {
                    return; // 如果 stopFlag 为 true，立即退出
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 小周期睡眠
            }
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

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setMinimumWidth(360);

    connect(startRecordButton, &QPushButton::clicked, this, &ClickerDialog::startRecord);
    connect(endRecordButton, &QPushButton::clicked, this, &ClickerDialog::endRecord);
    connect(startClickButton, &QPushButton::clicked, this, &ClickerDialog::startClick);
    connect(endClickButton, &QPushButton::clicked, this, &ClickerDialog::endClick);

    buttonLayout->addWidget(startRecordButton);
    buttonLayout->addWidget(endRecordButton);
    buttonLayout->addWidget(startClickButton);
    buttonLayout->addWidget(endClickButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(textEdit);

    // 添加间隔设置行
    auto *intervalLayout = new QHBoxLayout();
    auto *intervalLabel = new QLabel("间隔", this);
    intervalSpinBox = new QSpinBox(this);
    intervalSpinBox->setRange(1, 10000); // 设置最小值和最大值
    intervalSpinBox->setValue(100); // 设置默认值为100
    auto *millisecondLabel = new QLabel("毫秒", this);

    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(intervalSpinBox);
    intervalLayout->addWidget(millisecondLabel);
    intervalLayout->addStretch(); // 将内容靠右对齐

    mainLayout->addLayout(intervalLayout);

    this->setLayout(mainLayout);
}

void ClickerDialog::closeEvent(QCloseEvent *) {
    if (hook) UnhookWindowsHookEx(hook);

    clickThread->stop();
    clickThread = nullptr;
    instance = nullptr;

    parentWidget()->showNormal();
}


void ClickerDialog::appendCoordinate(int x, int y) {
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

    auto text = QString("循环点击(间隔%1毫秒): ").arg(intervalSpinBox->value());
    for (auto &point: pointList) {
        text += QString("(%1, %2)").arg(point.x).arg(point.y);
    }

    textEdit->append(text);

    clickThread = new ClickThread(pointList, state.hwnd, intervalSpinBox->value(), this);
    connect(clickThread, &ClickThread::finished, clickThread, &QObject::deleteLater);
    clickThread->start();
}

void ClickerDialog::endClick() {
    if (!clickThread) {
        textEdit->append("当前未开始连点");
        return;
    }

    if (isWaiting) {
        textEdit->append("请先完成录制");
        return;
    }

    clickThread->stop();
    clickThread->wait();
    clickThread = nullptr;

    textEdit->append("已结束点击");
}


LRESULT CALLBACK ClickerDialog::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_LBUTTONDOWN && instance && instance->isWaiting) {
        auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
        HWND hwnd = WindowFromPoint(pMouseStruct->pt);

        if (hwnd == state.hwnd) {
            // 跨线程安全更新 UI
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