# include "simple.h"

float getScale() {
    MONITORINFOEX info = {};
    POINT ptZero = {0, 0};
    info.cbSize = sizeof(info);
    GetMonitorInfo(MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY), &info);
    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    return static_cast<float>(devMode.dmPelsWidth) / static_cast<float>(info.rcMonitor.right - info.rcMonitor.left);
}

AutoWarSimple::AutoWarSimple(QWidget *parent) : PanelWidget(parent) {
    instance = this;
    setWindowTitle("红警自动");
    resize(480, 320);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    auto *mainLayout = new QVBoxLayout(this);
    auto *controlLayout = new QHBoxLayout();

    auto *captureButton = new QPushButton("获取窗口");
    connect(captureButton, &QPushButton::clicked, this, &AutoWarSimple::startCapture);

    auto *executeButton = new QPushButton("执行命令");
    connect(executeButton, &QPushButton::clicked, this, &AutoWarSimple::selectCommand);

    controlLayout->addWidget(captureButton);
    controlLayout->addWidget(executeButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(clearButton);

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(outputText);

    log("使用方法: 先获取窗口，再执行命令", "blue");
    log(QString("当前缩放率:%1").arg(QString::number(state.scale)));
    log("QQ游戏支持任意缩放率，其他客户端(如浏览器、360游戏)可能需要调整为100%缩放再打开辅助!");
}

void AutoWarSimple::startCapture() {
    log("请用鼠标点击游戏窗口");

    isWaiting = true;
    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);

    if (hook == nullptr) log("Failed to set hook");
}


void AutoWarSimple::selectCommand() {
    QDialog selectDialog(this);
    selectDialog.setWindowTitle("选择命令");
    selectDialog.setWindowFlags(selectDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    selectDialog.setLayout(createCommandLayout());
    selectDialog.exec();
}

[[maybe_unused]] void AutoWarSimple::getHwnd(int x, int y) {
    HWND hwnd = WindowFromPoint(POINT{x, y});

    if (!hwnd) {
        log("获取窗口句柄失败", "red");
        return;
    }

    wchar_t buffer[256] = {0};
    GetWindowTextW(hwnd, buffer, 256);

    log(
            QString("获取到窗口: ") + QString::fromWCharArray(buffer) + "(0x" +
            QString::number(reinterpret_cast<qulonglong>(hwnd), 16) + ")", "blue"
    );
    log("请不要最小化游戏窗口！但可以放在其它窗口后面", "red");
    state.hwnd = hwnd;

    UnhookWindowsHookEx(hook);
    hook = nullptr;
    isWaiting = false;
}

void AutoWarSimple::closeEvent(QCloseEvent *event) {
    if (hook) UnhookWindowsHookEx(hook);

    instance = nullptr;

    PanelWidget::closeEvent(event);

}

LRESULT CALLBACK AutoWarSimple::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_LBUTTONDOWN && instance && instance->isWaiting) {
        auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
        QMetaObject::invokeMethod(
                instance,
                "getHwnd",
                Qt::QueuedConnection,
                Q_ARG(int, pMouseStruct->pt.x),
                Q_ARG(int, pMouseStruct->pt.y)
        );
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

AutoWarSimple *AutoWarSimple::instance = nullptr;

int main(int argc, char *argv[]) {
    state.scale = getScale();

    QApplication app(argc, argv);
    AutoWarSimple window;
    window.show();
    return QApplication::exec();
}

