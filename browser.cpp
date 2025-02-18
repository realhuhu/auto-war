#include "browser.h"


AutoWarBrowser::AutoWarBrowser(QWidget *parent) {
    setWindowTitle("红警浏览器");

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

    mainLayout->addWidget(browser);
    mainLayout->addLayout(panelLayout);
}

void AutoWarBrowser::closeEvent(QCloseEvent *event) {
    if (browser) {
        browser->deleteLater();
        browser = nullptr;
    }

    PanelWidget::closeEvent(event);
}

void AutoWarBrowser::refresh() {
    browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));
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

