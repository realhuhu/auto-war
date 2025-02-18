#include "browser.h"


AutoWarBrowser::AutoWarBrowser(QWidget *parent) : PanelWidget(parent) {
    setWindowTitle("红警浏览器");
    resize(this->width(), 965);
    autoHide = false;

    auto *mainLayout = new QHBoxLayout(this);
    auto *panelLayout = new QVBoxLayout(this);
    auto *controlLayout = new QHBoxLayout();

    auto *refreshButton = new QPushButton("刷新游戏");
    connect(refreshButton, &QPushButton::clicked, this, &AutoWarBrowser::refresh);

    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(executeButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(clearButton);

    panelLayout->addLayout(controlLayout);
    panelLayout->addWidget(outputText);

    browser = new QWebEngineView(this);

    QWebEngineProfile *engineProfile = browser->page()->profile();
    engineProfile->setCachePath("cache");
    engineProfile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
    engineProfile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);

    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));
    browser->setFixedWidth(985);
    browser->setMinimumHeight(700);
    browser->setMaximumHeight(935);

    state.hwnd = reinterpret_cast<HWND>(browser->winId());
    wchar_t buffer[256] = {0};
    GetWindowTextW(state.hwnd, buffer, 256);

    log(
            QString("获取到窗口: ") + QString::fromWCharArray(buffer) + "(0x" +
            QString::number(reinterpret_cast<qulonglong>(state.hwnd), 16) + ")", "blue"
    );

    log("运行命令时由于在频繁点击浏览器，按钮可能需要连续快速点击才能触发!");
    log("按快捷键Ctrl Q也可以停止命令", "blue");

    mainLayout->addWidget(browser);
    mainLayout->addLayout(panelLayout);

    auto *quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &AutoWarBrowser::stopCommand);
    addAction(quitAction);
}

void AutoWarBrowser::refresh() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "刷新游戏", "确认刷新网页？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));
}

void AutoWarBrowser::closeEvent(QCloseEvent *event) {
    if (browser) {
        browser->deleteLater();
        browser = nullptr;
    }

    PanelWidget::closeEvent(event);
}

AutoWarBrowser::~AutoWarBrowser() {
    if (browser) {
        browser->page()->deleteLater();
        browser->close();
    }
}

int main(int argc, char *argv[]) {
    state.scale = 1;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    std::vector<char *> newArgv;
    newArgv.reserve(argc);
    for (int i = 0; i < argc; ++i) newArgv.push_back(argv[i]);

    const char *newParam = "--register-pepper-plugins='pepflashplayer.dll;application/x-demo'";
    size_t newParamLen = strlen(newParam) + 1;
    char *newParamCopy = new char[newParamLen];
    errno_t err = strcpy_s(newParamCopy, newParamLen, newParam);
    if (err != 0) {
        for (auto ptr: newArgv) {
            delete[] ptr;
        }
        delete[] newParamCopy;
        return 1;
    }
    newArgv.push_back(newParamCopy);
    argc = newArgv.size();
    char **newArgvArray = new char *[argc];
    for (int i = 0; i < argc; ++i) newArgvArray[i] = newArgv[i];

    QApplication app(argc, newArgvArray);
    AutoWarBrowser window;
    window.show();
    int result = QApplication::exec();

    // 释放内存
    for (int i = 0; i < argc; ++i) {
        if (i >= argc - 1) {
            delete[] newArgvArray[i];
        }
    }
    delete[] newArgvArray;

    return result;
}