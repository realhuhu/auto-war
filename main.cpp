#include "main.h"

AutoWar::AutoWar(QWidget *parent) : QWidget(parent) {
    setWindowTitle("红警多开");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    resize(640, 480);

    auto *mainLayout = new QVBoxLayout(this);

    auto *topLayout = new QHBoxLayout();

    auto *openWindowButton = new QPushButton("全部打开", this);
    auto *qqLoginButton = new QPushButton("QQ账号", this);
    auto *setConfigButton = new QPushButton("红警账号", this);
    auto *batchRunButton = new QPushButton("全部执行", this);
    auto *batchStopButton = new QPushButton("全部停止", this);
    auto *clearLogButton = new QPushButton("清空日志", this);
    topLayout->addWidget(openWindowButton);
    topLayout->addWidget(qqLoginButton);
    topLayout->addWidget(setConfigButton);
    topLayout->addWidget(batchRunButton);
    topLayout->addWidget(batchStopButton);
    topLayout->addWidget(clearLogButton);

    mainLayout->addLayout(topLayout);

    auto *bodyLayout = new QHBoxLayout();
    logTextEdit->setReadOnly(true);
    bodyLayout->addWidget(panelTabWidget, 3);
    bodyLayout->addWidget(logTextEdit, 2);

    mainLayout->addLayout(bodyLayout);

    connect(openWindowButton, &QPushButton::clicked, this, &AutoWar::openBrowser);
    connect(qqLoginButton, &QPushButton::clicked, this, &AutoWar::openQQManager);
    connect(clearLogButton, &QPushButton::clicked, this, &AutoWar::clearLog);
}

void AutoWar::log(const QString &text, const QString &color) const {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTime currentTime = currentDateTime.time();
    QString timeString = currentTime.toString("hh:mm:ss");

    logTextEdit->append(QString(
            R"(
                <div>
                <span style="color:green;">[%1]</span>
                <span style="color:%2;">%3</span>
                </div>
                )"
    ).arg(timeString, color, text));
}


void AutoWar::openBrowser() {
    auto browser = new AutoWarBrowser(panelTabWidget);
    auto wid = browser->winId();

    browsers.insert(wid, browser);
    browser->show();

    panelTabWidget->addTabWithWId(browser->panel, WIdToQSting(wid), browser->winId());

    connect(browser, &AutoWarBrowser::closed, [this](WId closedWid) {
        panelTabWidget->removeTabByWId(closedWid);
        browsers.remove(closedWid);
        log(QString("已关闭游戏窗口(%1)").arg(WIdToQSting(closedWid)));
    });

    log(QString("已打开游戏窗口(%1)").arg(WIdToQSting(wid)));
}


void AutoWar::openQQManager() {
    auto dialog = new QQManger(this);
    dialog->exec();
}

void AutoWar::clearLog() const { logTextEdit->clear(); }


void AutoWar::closeEvent(QCloseEvent *event) {
    for (auto it = browsers.begin(); it != browsers.end(); ++it) {
        AutoWarBrowser *browser = it.value();
        if (browser) {
            browser->close();
        }
    }
    browsers.clear();

    QWidget::closeEvent(event);
}


int main(int argc, char *argv[]) {
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

    AutoWar autoWar;
    autoWar.show();

    int result = QApplication::exec();

    for (int i = 0; i < argc; ++i) {
        if (i >= argc - 1) {
            delete[] newArgvArray[i];
        }
    }
    delete[] newArgvArray;

    return result;
}
