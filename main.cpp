#include "main.h"

auto configFile = "/config.json";

AutoWar::AutoWar(QWidget *parent) : QWidget(parent) {
    setWindowTitle("红警多开");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    resize(640, 480);

    auto *mainLayout = new QVBoxLayout(this);

    auto *topLayout = new QHBoxLayout();

    auto *openAllButton = new QPushButton("全部打开", this);
    auto *qqConfigButton = new QPushButton("QQ账号", this);
    auto *redConfigButton = new QPushButton("红警账号", this);
    auto *batchRunButton = new QPushButton("全部执行", this);
    auto *batchStopButton = new QPushButton("全部停止", this);
    auto *clearLogButton = new QPushButton("清空日志", this);
    topLayout->addWidget(openAllButton);
    topLayout->addWidget(qqConfigButton);
    topLayout->addWidget(redConfigButton);
    topLayout->addWidget(batchRunButton);
    topLayout->addWidget(batchStopButton);
    topLayout->addWidget(clearLogButton);

    mainLayout->addLayout(topLayout);

    auto *bodyLayout = new QHBoxLayout();
    logTextEdit->setReadOnly(true);
    bodyLayout->addWidget(panelTabWidget, 3);
    bodyLayout->addWidget(logTextEdit, 2);

    mainLayout->addLayout(bodyLayout);

    connect(openAllButton, &QPushButton::clicked, this, &AutoWar::openBrowser);
    connect(qqConfigButton, &QPushButton::clicked, this, &AutoWar::openQQManager);
    connect(redConfigButton, &QPushButton::clicked, this, &AutoWar::openRedManager);
    connect(clearLogButton, &QPushButton::clicked, this, &AutoWar::clearLog);

    loadConfig();
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

void AutoWar::loadConfig() const {
    QString configPath = QCoreApplication::applicationDirPath() + configFile;
    QFileInfo fileInfo(configPath);

    if (!fileInfo.exists()) {
        QFile newFile(configPath);
        if (!newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            log(QString("无法创建配置文件: %1").arg(configPath));
            return;
        }
        QTextStream stream(&newFile);
        stream << "{\"account\": []}";
        newFile.close();
        log("已创建默认配置文件");
    }

    QFile file(configPath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        log(QString("配置文件存在但无法打开: %1").arg(file.errorString()));
        return;
    }

    QByteArray jsonData = file.readAll();
    QJsonParseError parseError{};
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        log(QString("配置文件解析错误: %1").arg(parseError.errorString()));

        if (jsonData.trimmed().isEmpty()) {
            log("检测到空文件，重新初始化配置");
            file.resize(0); // 清空文件内容
            QTextStream stream(&file);
            stream << "{\"account\": []}";
        }
        file.close();
        return;
    }

    state.config = doc.object();
    file.close();
}

void AutoWar::openBrowser() {
    const auto accounts = state.config["account"].toArray();
    for (const auto &accountVal: accounts) {
        const auto account = accountVal.toObject();
        const auto qqRemark = account["remark"].toString();  // QQ账号备注
        const auto link = account["link"].toString();  // QQ账号备注

        // 提取红警子账号配置
        const auto redAccounts = account["red"].toArray();
        for (const auto &redVal: redAccounts) {
            const auto red = redVal.toObject();

            // 创建表格行数据
            const auto redRemark = red["remark"].toString();
            const auto region = red["region"].toInt();


            auto browser = new RedBrowser(
                    panelTabWidget,
                    qqRemark,
                    redRemark,
                    link,
                    region
            );

            auto wid = browser->winId();

            browsers.insert(wid, browser);
            browser->show();

            panelTabWidget->addTabWithWId(browser->panel, WIdToQSting(wid), browser->winId());

            connect(browser, &RedBrowser::closed, [this](WId closedWid) {
                panelTabWidget->removeTabByWId(closedWid);
                browsers.remove(closedWid);
                log(QString("已关闭游戏窗口(%1)").arg(WIdToQSting(closedWid)));
            });

            log(QString("已打开游戏窗口(%1)").arg(WIdToQSting(wid)));
        }
    }
}

void AutoWar::openQQManager() {
    auto dialog = new QQManger(this);
    connect(dialog, &QQManger::configChanged, this, &AutoWar::saveConfig);
    dialog->exec();
}

void AutoWar::openRedManager() {
    if (state.config["account"].toArray().isEmpty()) {
        log("请先配置QQ账号", "red");
        return;
    }

    auto dialog = new RedManger(this);
    connect(dialog, &RedManger::configChanged, this, &AutoWar::saveConfig);
    dialog->exec();
}

void AutoWar::saveConfig() const {
    QString configPath = QCoreApplication::applicationDirPath() + configFile;

    QFile file(configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(state.config);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }

    log("配置已保存");
}

void AutoWar::clearLog() const { logTextEdit->clear(); }

void AutoWar::closeEvent(QCloseEvent *event) {
    for (auto it = browsers.begin(); it != browsers.end(); ++it) {
        RedBrowser *browser = it.value();
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
