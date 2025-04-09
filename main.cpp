#include "main.h"

auto configName = "/config.json";
auto settingDefaultName = "/setting-default.json";

AutoWar::AutoWar(
        QWidget *parent
) : QWidget(parent), consoleTextEdit(new QTextEdit(this)), panelTabWidget(new LabelTabWidget(this)) {
    setWindowTitle("红警多开");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    resize(640, 480);

    auto mainLayout = new QVBoxLayout(this);

    auto topLayout = new QHBoxLayout();

    auto allBrowserButton = new QPushButton("打开游戏", this);
    auto qqConfigButton = new QPushButton("QQ账号", this);
    auto redConfigButton = new QPushButton("红警账号", this);
    auto batchRunButton = new QPushButton("全部执行", this);
    auto batchStopButton = new QPushButton("全部停止", this);
    auto otherFunctionButton = new QPushButton("其它功能", this);
    auto clearLogButton = new QPushButton("清空日志", this);
    topLayout->addWidget(allBrowserButton);
    topLayout->addWidget(qqConfigButton);
    topLayout->addWidget(redConfigButton);
    topLayout->addWidget(batchRunButton);
    topLayout->addWidget(batchStopButton);
    topLayout->addWidget(otherFunctionButton);
    topLayout->addWidget(clearLogButton);

    mainLayout->addLayout(topLayout);

    auto bodyLayout = new QHBoxLayout();
    consoleTextEdit->setReadOnly(true);
    bodyLayout->addWidget(panelTabWidget, 3);
    bodyLayout->addWidget(consoleTextEdit, 2);

    mainLayout->addLayout(bodyLayout);

    connect(allBrowserButton, &QPushButton::clicked, this, &AutoWar::onOpenAllBrowser);
    connect(qqConfigButton, &QPushButton::clicked, this, &AutoWar::onOpenQQManager);
    connect(redConfigButton, &QPushButton::clicked, this, &AutoWar::onOpenRedManager);
    connect(batchRunButton, &QPushButton::clicked, this, &AutoWar::onOpenCmdSelector);
    connect(batchStopButton, &QPushButton::clicked, this, &AutoWar::onBatchStop);
    connect(otherFunctionButton, &QPushButton::clicked, this, &AutoWar::onShowOther);
    connect(clearLogButton, &QPushButton::clicked, this, &AutoWar::onClearConsole);

    loadConfig();
}

void AutoWar::consolePrint(const QString &text, const QString &color) const {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTime currentTime = currentDateTime.time();
    QString timeString = currentTime.toString("hh:mm:ss");

    consoleTextEdit->append(QString(
            R"(
                <div>
                <span style="color:green;">[%1]</span>
                <span style="color:%2;">%3</span>
                </div>
                )"
    ).arg(timeString, color, text));
}

void AutoWar::loadConfig() const {
    auto configPath = QCoreApplication::applicationDirPath() + configName;
    QFileInfo fileInfo(configPath);

    if (!fileInfo.exists()) {
        QFile newFile(configPath);
        if (!newFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            consolePrint(QString("无法创建配置文件: %1").arg(configPath));
            return;
        }
        QTextStream stream(&newFile);
        stream << "{\"account\": []}";
        newFile.close();
        consolePrint("已创建默认配置文件");
    }

    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        consolePrint(QString("配置文件存在但无法打开: %1").arg(configFile.errorString()));
        return;
    }

    QJsonParseError parseError{};
    auto configJson = configFile.readAll();
    auto configDoc = QJsonDocument::fromJson(configJson, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        consolePrint(QString("配置文件解析错误: %1").arg(parseError.errorString()));

        if (configJson.trimmed().isEmpty()) {
            consolePrint("检测到空文件，重新初始化配置");
            configFile.resize(0);
            QTextStream stream(&configFile);
            stream << "{\"account\": []}";
        }
        configFile.close();
        return;
    }

    auto settingDefaultPath = QCoreApplication::applicationDirPath() + settingDefaultName;
    QFile settingDefaultFile(settingDefaultPath);
    if (!settingDefaultFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        consolePrint(QString("默认配置文件存在但无法打开: %1").arg(settingDefaultFile.errorString()));
        settingDefaultFile.close();
        return;
    }

    auto settingDefaultJson = settingDefaultFile.readAll();
    auto settingDefaultDoc = QJsonDocument::fromJson(settingDefaultJson);
    state.settingDefault = settingDefaultDoc.object();

    auto configObj = configDoc.object();
    auto accountArray = configObj["account"].toArray();

    for (int i = 0; i < accountArray.count(); ++i) {
        auto accountObj = accountArray[i].toObject();
        auto redArray = accountObj["red"].toArray();
        for (int j = 0; j < redArray.count(); ++j) {
            auto redObj = redArray[j].toObject();
            redObj["setting"] = mergeSetting(state.settingDefault, redObj["setting"].toObject());
            redArray[j] = redObj;
        }
        accountObj["red"] = redArray;
        accountArray[i] = accountObj;
    }
    configObj["account"] = accountArray;
    state.config = configObj;
    onConfigChanged();

    configFile.close();
    settingDefaultFile.close();
}

void AutoWar::onOpenAllBrowser() {
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

    if (redAccountList.isEmpty()) {
        consolePrint("请先配置QQ账号", "red");
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
        const bool isExisting = workers.contains(redRemark);

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

    connect(&btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(&btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    for (int i = 0; i < listWidget.count(); ++i) {
        QListWidgetItem *item = listWidget.item(i);
        if (item->checkState() != Qt::Checked) continue;

        const QVariantMap info = item->data(Qt::UserRole).toMap();
        const QString redRemark = info["redRemark"].toString();

        if (workers.contains(redRemark)) continue;

        if (!QUrl(info["link"].toString()).isValid()) {
            consolePrint(QString("链接无效，请点击[QQ账号]按钮，重新登录QQ号(%1)").arg(info["qqRemark"].toString()), "red");
            continue;
        }

        auto worker = new RedWorker(
                info["link"].toString(),
                info["region"].toInt(),
                info["qqRemark"].toString(),
                redRemark
        );

        workers.insert(redRemark, worker);

        auto panel = new RedController(redRemark, panelTabWidget);
        panelTabWidget->addTabWithLabel(panel, redRemark);

        connect(panel, &RedController::toRefreshBrowser, worker, &RedWorker::onRefreshBrowser);
        connect(panel, &RedController::toRunTask, worker, &RedWorker::onRunTask);
        connect(panel, &RedController::toStopTask, worker, &RedWorker::onStopTask);
        connect(worker, &RedWorker::toLog, panel, &RedController::onLog);
        connect(worker, &RedWorker::toConsole, this, &AutoWar::onConsolePrint);
        connect(worker, &RedWorker::closed, this, &AutoWar::onBrowserClosed);

        consolePrint(QString("(%1)已打开游戏窗口").arg(redRemark));
    }
}

void AutoWar::onOpenQQManager() {
    auto dialog = new QQManger(this);
    connect(dialog, &QQManger::configChanged, this, &AutoWar::onConfigChanged);
    dialog->exec();
}

void AutoWar::onOpenRedManager() {
    if (state.config["account"].toArray().isEmpty()) {
        consolePrint("请先配置QQ账号", "red");
        return;
    }

    auto dialog = new RedManger(this);
    connect(dialog, &RedManger::configChanged, this, &AutoWar::onConfigChanged);
    dialog->exec();
}

void AutoWar::onOpenCmdSelector() {
    auto dialog = new CmdSelector("全部运行", this);
    connect(dialog, &CmdSelector::taskCreated, this, &AutoWar::onTaskCreated);
    dialog->exec();
}

void AutoWar::onBatchStop() {
    consolePrint("全部停止", "red");
    for (auto it = workers.begin(); it != workers.end(); ++it) {
        auto worker = it.value();
        if (worker) worker->stopTask();
    }
}

void AutoWar::onShowOther() {
    auto dialog = new FuncSelector(workers, this);
    connect(dialog, &FuncSelector::toConsole, this, &AutoWar::onConsolePrint);

    dialog->exec();
}

void AutoWar::onTaskCreated(const QString &command, const std::function<void(Env &env)> &task) const {
    consolePrint(QString("全部运行: %1").arg(command), "green");
    for (auto it = workers.begin(); it != workers.end(); ++it) {
        auto worker = it.value();
        if (worker) worker->runTask(command, task);
    }
}

void AutoWar::onConfigChanged() const {
    QString configPath = QCoreApplication::applicationDirPath() + configName;

    QFile file(configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(state.config);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        consolePrint("配置已保存");
    } else {
        consolePrint("配置保存失败", "red");
    }
}

void AutoWar::onConsolePrint(const QString &text, const QString &color) const { consolePrint(text, color); }

void AutoWar::onClearConsole() const { consoleTextEdit->clear(); }

void AutoWar::onBrowserClosed(const QString &remark) {
    panelTabWidget->removeTabByLabel(remark);
    workers.remove(remark);
    consolePrint(QString("(%1)已关闭游戏窗口").arg(remark), "red");
}

void AutoWar::closeEvent(QCloseEvent *event) {
    if (!workers.empty()) {
        QMessageBox::question(
                this,
                "关闭",
                "请先关闭全部游戏窗口，再关闭程序",
                QMessageBox::Yes
        );

        event->ignore();
        return;
    }

    QWidget::closeEvent(event);
}


int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));

    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    AutoWar autoWar;
    autoWar.show();

    return QApplication::exec();
}
