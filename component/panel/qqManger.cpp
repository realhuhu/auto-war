#include "qqManger.h"

QQManger::QQManger(QWidget *parent) : QDialog(parent), networkManager(new QNetworkAccessManager(this)) {
    setWindowTitle("QQ账号配置");
    resize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto mainLayout = new QVBoxLayout(this);

    QStringList headers;
    headers << "备注" << "QQ号" << "密码" << "链接" << "状态" << "操作";
    tableWidget = new QTableWidget(0, 6);
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setItemDelegateForColumn(QQNumberCol, new PlaceholderDelegate("不填则为扫码登录", this));
    tableWidget->setItemDelegateForColumn(PasswordCol, new PlaceholderDelegate("不填则为扫码登录", this));
    tableWidget->setItemDelegateForColumn(LinkCol, new PlaceholderDelegate("点击登录按钮获取链接", this));
    tableWidget->horizontalHeader()->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #87CEFA;
            border: 1px solid #E0E0E0;
        }
    )");

    mainLayout->addWidget(tableWidget);

    auto buttonContainer = new QWidget();
    auto buttonLayout = new QHBoxLayout(buttonContainer);
    auto addButton = new QPushButton("添加QQ账号");
    auto testAllButton = new QPushButton("全部测试");
    auto loginAllButton = new QPushButton("全部登录");
    connect(addButton, &QPushButton::clicked, this, &QQManger::addNew);
    connect(testAllButton, &QPushButton::clicked, this, &QQManger::testAll);
    connect(loginAllButton, &QPushButton::clicked, this, &QQManger::loginAll);
    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(testAllButton);
    buttonLayout->addWidget(loginAllButton);

    mainLayout->addWidget(buttonContainer);

    loadConfig();
}

int QQManger::getIndex(QObject *button) const {
    QWidget *buttonWidget = qobject_cast<QWidget *>(button);

    if (!buttonWidget) return -1;

    for (int i = 0; i < tableWidget->rowCount(); ++i) {
        if (tableWidget->cellWidget(i, ActionCol) == buttonWidget) return i;
    }

    return -1;
}

void QQManger::insertRow(const QString &remark, const QString &qq, const QString &password, const QString &link) {
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    auto remarkItem = new QTableWidgetItem(remark);
    remarkItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);
    remarkItem->setBackground(QColor(240, 240, 240));
    tableWidget->setItem(row, RemarkCol, remarkItem);

    auto qqItem = new QTableWidgetItem(qq);
    tableWidget->setItem(row, QQNumberCol, qqItem);

    auto pwdItem = new QTableWidgetItem(password);
    tableWidget->setItem(row, PasswordCol, pwdItem);

    auto linkItem = new QTableWidgetItem(link);
    tableWidget->setItem(row, LinkCol, linkItem);

    auto statusItem = new QTableWidgetItem("待测试");
    statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
    statusItem->setForeground(Qt::darkGray);
    statusItem->setBackground(QColor(240, 240, 240));
    tableWidget->setItem(row, StatusCol, statusItem);

    auto buttonWidget = new QWidget();
    auto btnLayout = new QHBoxLayout(buttonWidget);
    btnLayout->setContentsMargins(2, 0, 2, 0);
    auto testBtn = new QPushButton("测试");
    auto loginBtn = new QPushButton("登录");
    auto deleteBtn = new QPushButton("删除");
    deleteBtn->setStyleSheet("background-color: #ff4444; color: white;");
    btnLayout->addWidget(testBtn);
    btnLayout->addWidget(loginBtn);
    btnLayout->addWidget(deleteBtn);

    tableWidget->setCellWidget(row, ActionCol, buttonWidget);


    connect(testBtn, &QPushButton::clicked, this, [this]() {
        int index = getIndex(sender()->parent());
        if (index != -1) handleTest(index);
    });

    connect(loginBtn, &QPushButton::clicked, this, [this]() {
        int index = getIndex(sender()->parent());
        if (index != -1) handleLogin(index);
    });

    connect(deleteBtn, &QPushButton::clicked, this, [this]() {
        int index = getIndex(sender()->parent());
        if (index != -1) handleDelete(index);
    });
}

void QQManger::setStatus(int row, Status status) {
    auto statusItem = tableWidget->item(row, StatusCol);
    QString text;
    QColor color;

    switch (status) {
        case Waiting:
            text = "待测试";
            color = Qt::darkGray;
            break;
        case Empty:
            text = "空链接";
            color = Qt::yellow;
            break;
        case Testing:
            text = "测试中...";
            color = Qt::lightGray;
            break;
        case Timeout:
            text = "超时";
            color = Qt::red;
            break;
        case NetError:
            text = "网络错误";
            color = Qt::red;
            break;
        case Valid:
            text = "有效";
            color = Qt::green;
            break;
        case Invalid:
            text = "已过期";
            color = Qt::red;
            break;
    }

    statusItem->setText(text);
    statusItem->setForeground(color);
}

void QQManger::loadConfig() {
    tableWidget->setRowCount(0);

    QJsonArray accounts = state.config["account"].toArray();
    std::vector<QJsonObject> accountObjects;

    for (auto val: accounts) if (val.isObject()) accountObjects.push_back(val.toObject());

    std::sort(accountObjects.begin(), accountObjects.end(), [](const QJsonObject &a, const QJsonObject &b) {
        return a["order"].toInt(0) < b["order"].toInt(0);
    });

    QJsonArray sortedAccounts;
    for (const auto &obj: accountObjects) sortedAccounts.append(obj);

    for (auto accountVal: accounts) {
        QJsonObject account = accountVal.toObject();

        insertRow(
                account["remark"].toString(),
                account["qq"].toString(),
                account["password"].toString(),
                account["link"].toString()
        );
    }
}

void QQManger::saveConfig() {
    QJsonArray accounts;
    QHash<QString, QJsonArray> existingReds;

    if (state.config.contains("account") && state.config["account"].isArray()) {
        for (const auto val: state.config["account"].toArray()) {
            QJsonObject obj = val.toObject();
            QString remark = obj["remark"].toString();
            existingReds.insert(remark, obj["red"].toArray());
        }
    }

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QJsonObject account;

        auto remarkItem = tableWidget->item(row, RemarkCol);
        auto qqItem = tableWidget->item(row, QQNumberCol);
        auto pwdItem = tableWidget->item(row, PasswordCol);
        auto linkItem = tableWidget->item(row, LinkCol);

        account["order"] = row + 1;
        account["remark"] = remarkItem ? remarkItem->text() : "";
        account["qq"] = qqItem ? qqItem->text() : "";
        account["password"] = pwdItem ? pwdItem->text() : "";
        account["link"] = linkItem ? linkItem->text() : "";

        if (existingReds.contains(account["remark"].toString())) {
            account["red"] = existingReds[account["remark"].toString()];
        } else {
            account["red"] = QJsonArray();
        }

        accounts.append(account);
    }

    state.config["account"] = accounts;

    emit configChanged();
}

void QQManger::handleDelete(int row) {
    if (row >= 0 && row < tableWidget->rowCount()) {
        auto remarkItem = tableWidget->item(row, RemarkCol);
        QString remark = remarkItem ? remarkItem->text() : "未知账号";

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
                this,
                "确认删除",
                QString("确定要删除账号[%1]吗？").arg(remark),
                QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            tableWidget->removeRow(row);

            remark = remark.trimmed().replace("/", "_").replace("\\", "_");
            auto storagePath = QCoreApplication::applicationDirPath() + "/web_profile/" + remark;
            QDir(storagePath).removeRecursively();
        }
    }
}

void QQManger::handleTest(int row) {
    auto remarkItem = tableWidget->item(row, RemarkCol);
    auto linkItem = tableWidget->item(row, LinkCol);
    auto url = linkItem->text();
    auto remark = remarkItem->text();

    if (url.isEmpty()) {
        setStatus(row, Empty);
        return;
    }

    setStatus(row, Testing);

    auto reply = networkManager->get(QNetworkRequest(url));
    auto timeoutTimer = new QTimer(reply);
    timeoutTimer->setSingleShot(true);

    connect(timeoutTimer, &QTimer::timeout, [reply]() { if (reply->isRunning()) reply->abort(); });

    connect(reply, &QNetworkReply::finished, [this, reply, remark, timeoutTimer]() {
        timeoutTimer->stop();
        Status status;

        if (reply->error() == QNetworkReply::NoError) {
            status = QString::fromUtf8(reply->readAll()).contains("\">ID:") ? Valid : Invalid;
        } else {
            status = reply->error() == QNetworkReply::OperationCanceledError ? Timeout : NetError;
        }

        for (int i = 0; i < tableWidget->rowCount(); ++i) {
            if (tableWidget->item(i, RemarkCol)->text() == remark) {
                setStatus(i, status);
                break;
            }
        }

        reply->deleteLater();
        timeoutTimer->deleteLater();
    });

    timeoutTimer->start(30000);
}

void QQManger::handleLogin(int row) {
    auto remarkItem = tableWidget->item(row, RemarkCol);
    QString remark = remarkItem ? remarkItem->text() : "未知账号";

    auto browser = new QQLoginBrowser(this, remark);
    connect(browser, &QQLoginBrowser::linkDetected, [this, &row](const QUrl &url) {
        auto linkItem = tableWidget->item(row, LinkCol);
        linkItem->setText(url.toString());

        setStatus(row, Valid);
    });
    browser->exec();
}

void QQManger::addNew() {
    bool ok;
    QString remark;

    while (true) {
        remark = QInputDialog::getText(this, "添加账号", "请输入QQ备注(设置后无法修改！)", QLineEdit::Normal, "", &ok);

        if (!ok) return;

        if (remark.length() == 0 || remark.length() > 10) {
            QMessageBox::warning(this, "输入错误", "备注应当为1-10字符");
            continue;
        }

        bool exists = false;
        for (int i = 0; i < tableWidget->rowCount(); ++i) {
            if (tableWidget->item(i, RemarkCol)->text() == remark) {
                exists = true;
                break;
            }
        }

        if (exists) {
            QMessageBox::warning(this, "输入错误", "备注名称已存在，请重新输入！");
            continue;
        }

        break;
    }

    insertRow(remark);
}

void QQManger::testAll() { for (int row = 0; row < tableWidget->rowCount(); ++row) handleTest(row); }

void QQManger::loginAll() { for (int row = 0; row < tableWidget->rowCount(); ++row) handleLogin(row); }

void QQManger::closeEvent(QCloseEvent *event) {
    saveConfig();
    QDialog::closeEvent(event);
}

