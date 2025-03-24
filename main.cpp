#include "main.h"

auto configFile = "/config.json";

AutoWar::AutoWar(QWidget *parent) : QWidget(parent) {
    setWindowTitle("红警多开");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    resize(640, 480);

    auto *mainLayout = new QVBoxLayout(this);

    auto *topLayout = new QHBoxLayout();

    auto *openAllButton = new QPushButton("打开游戏", this);
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
            file.resize(0);
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

    QList<QVariantMap> redAccountList;
    for (const auto &accountVal: accounts) {
        const auto account = accountVal.toObject();
        const auto qqRemark = account["remark"].toString();
        const auto link = account["link"].toString();

        const auto redAccounts = account["red"].toArray();
        for (const auto &redVal: redAccounts) {
            const auto red = redVal.toObject();

            QVariantMap info;
            info["qqRemark"] = qqRemark;
            info["redRemark"] = red["remark"].toString();
            info["link"] = link;
            info["region"] = red["region"].toInt();
            redAccountList.append(info);
        }
    }

    if(redAccountList.isEmpty()){
        log("请先配置QQ账号", "red");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("选择要打开的账号");
    dialog.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout mainLayout(&dialog);

    QListWidget listWidget;
    mainLayout.addWidget(&listWidget);

    for (const auto &info: redAccountList) {
        const QString qqRemark = info["qqRemark"].toString();
        const QString redRemark = info["redRemark"].toString();
        const int region = info["region"].toInt();

        QString itemText = QString("%1 %2 %3区").arg(qqRemark, redRemark, QString::number(region));

        auto item = new QListWidgetItem(itemText);
        const bool isExisting = browsers.contains(redRemark);

        item->setCheckState(isExisting ? Qt::Checked : Qt::Unchecked);
        item->setFlags(isExisting ? (item->flags() & ~Qt::ItemIsEnabled) : (item->flags() | Qt::ItemIsUserCheckable));
        item->setData(Qt::UserRole, info);
        listWidget.addItem(item);
    }

    QPushButton btnOk("确定"), btnCancel("取消");
    QHBoxLayout btnLayout;
    btnLayout.addWidget(&btnOk);
    btnLayout.addWidget(&btnCancel);
    mainLayout.addLayout(&btnLayout);

    QObject::connect(&btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(&btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    for (int i = 0; i < listWidget.count(); ++i) {
        QListWidgetItem *item = listWidget.item(i);
        if (item->checkState() != Qt::Checked) continue;

        const QVariantMap info = item->data(Qt::UserRole).toMap();
        const QString redRemark = info["redRemark"].toString();

        if (browsers.contains(redRemark)) continue;

        auto browser = new RedGameBrowser(
                panelTabWidget,
                info["qqRemark"].toString(),
                redRemark,
                info["link"].toString(),
                info["region"].toInt()
        );

        browsers.insert(redRemark, browser);
        browser->show();
        panelTabWidget->addTabWithLabel(browser->panel, redRemark);

        connect(browser, &RedGameBrowser::closed, [this](const QString &remark) {
            panelTabWidget->removeTabByLabel(remark);
            browsers.remove(remark);
            log(QString("已关闭游戏窗口(%1)").arg(remark), "red");
        });

        log(QString("已打开游戏窗口(%1)").arg(redRemark));
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
        RedGameBrowser *browser = it.value();
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
    QApplication::setStyle(QStyleFactory::create("Fusion"));

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
