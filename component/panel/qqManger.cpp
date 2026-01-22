#include "qqManger.h"

QQManger::QQManger(QWidget* parent) : QDialog(parent), networkManager(new QNetworkAccessManager(this))
{
    setWindowTitle("QQ账号配置");
    resize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto mainLayout = new QVBoxLayout(this);

    QStringList headers;
    headers << "备注" << "QQ号" << "密码" << "游戏ID" << "链接" << "状态" << "操作";
    tableWidget = new QTableWidget(0, headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setItemDelegateForColumn(QQNumberCol, new PlaceholderDelegate("非必填", this));
    tableWidget->setItemDelegateForColumn(PasswordCol, new PlaceholderDelegate("非必填", this));
    tableWidget->setItemDelegateForColumn(GameIDCol, new PlaceholderDelegate("非必填", this));
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
    connect(addButton, &QPushButton::clicked, this, &QQManger::onAdd);
    connect(testAllButton, &QPushButton::clicked, this, &QQManger::onTestAll);
    connect(loginAllButton, &QPushButton::clicked, this, &QQManger::onLoginAll);
    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(testAllButton);
    buttonLayout->addWidget(loginAllButton);
    mainLayout->addWidget(buttonContainer);

    loadConfig();
}

int QQManger::getIndex(QObject* button) const
{
    QWidget* buttonWidget = qobject_cast<QWidget*>(button);

    if (!buttonWidget) return -1;

    for (int i = 0; i < tableWidget->rowCount(); ++i)
    {
        if (tableWidget->cellWidget(i, ActionCol) == buttonWidget) return i;
    }

    return -1;
}

void QQManger::insertRow(const QString& remark, const QString& qq, const QString& password, const QString& id,
                         const QString& link)
{
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    auto remarkItem = new QTableWidgetItem(remark);
    remarkItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);
    remarkItem->setBackground(QColor(240, 240, 240));

    auto qqItem = new QTableWidgetItem(qq);
    auto pwdItem = new QTableWidgetItem(password);
    auto idItem = new QTableWidgetItem(id);
    auto linkItem = new QTableWidgetItem(link);

    auto statusItem = new QTableWidgetItem("待测试");
    statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
    statusItem->setForeground(Qt::darkGray);
    statusItem->setBackground(QColor(240, 240, 240));

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

    tableWidget->setItem(row, RemarkCol, remarkItem);
    tableWidget->setItem(row, QQNumberCol, qqItem);
    tableWidget->setItem(row, PasswordCol, pwdItem);
    tableWidget->setItem(row, GameIDCol, idItem);
    tableWidget->setItem(row, LinkCol, linkItem);
    tableWidget->setItem(row, StatusCol, statusItem);
    tableWidget->setCellWidget(row, ActionCol, buttonWidget);

    connect(testBtn, &QPushButton::clicked, this, [this]()
    {
        int index = getIndex(sender()->parent());
        if (index != -1) handleTest(index);
    });

    connect(loginBtn, &QPushButton::clicked, this, [this]()
    {
        int index = getIndex(sender()->parent());
        if (index != -1) handleLogin(index);
    });

    connect(deleteBtn, &QPushButton::clicked, this, [this]()
    {
        int index = getIndex(sender()->parent());
        if (index != -1) handleDelete(index);
    });
}

void QQManger::setStatus(int row, Status status) const
{
    auto statusItem = tableWidget->item(row, StatusCol);
    QString text;
    QColor color;

    switch (status)
    {
    case Waiting:
        text = "待测试";
        color = Qt::darkGray;
        break;
    case Empty:
        text = "无效链接";
        color = Qt::darkRed;
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

int QQManger::scanPID(const QString& id)
{
    QProcess wmicProcess;
    QList<int> pidList;

    wmicProcess.start(
        "wmic",
        {
            "process",
            "get",
            "ProcessID,CommandLine",
            "/value"
        }
    );

    if (!wmicProcess.waitForStarted() || !wmicProcess.waitForFinished()) return 0;

    const auto outputStr = QString(wmicProcess.readAllStandardOutput());
    QStringList lines = outputStr.split(QRegExp("[\r\n]+"), Qt::SkipEmptyParts);

    bool founded = false;
    for (QString& line : lines)
    {
        if (line.contains("--type=ppapi") && !founded) founded = true;

        if (line.startsWith("ProcessId=") && founded)
        {
            pidList.append(line.replace("ProcessId=", "").toInt());
            founded = false;
        }
    }

    const QRegularExpression re("^\\d+$");
    for (const auto pid : pidList)
    {
        const HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess == nullptr || hProcess == INVALID_HANDLE_VALUE)continue;

        try
        {
            const auto scanner = new MemoryScanner(hProcess);

            if (scanner->Search("74 61 6E 6B 73 74 6F 72 6D").empty())
            {
                CloseHandle(hProcess);
                continue;
            }

            auto results = scanner->Search("2E 73 6F 6C");

            QMap<QString, int> resultMap;
            for (const auto result : results)
            {
                const auto mem = new Memory(hProcess, result);
                auto str = QString::fromStdString(mem->ReadString(-20, 20));

                if (!str.contains('/')) continue;

                auto candidate = str.section('/', -1);

                if (!re.match(candidate).hasMatch()) continue;

                resultMap[candidate] += 1;
            }

            CloseHandle(hProcess);

            if (resultMap.empty())continue;

            QList<QPair<QString, int>> list;
            for (auto it = resultMap.begin(); it != resultMap.end(); ++it)
            {
                list.append(qMakePair(it.key(), it.value()));
            }

            // 使用std::sort按照值降序排序，如果值相同，可以按照键升序（或其它规则）
            std::sort(list.begin(), list.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b)
            {
                return a.second > b.second;
            });

            if (list[0].first == id) return pid;
        }
        catch (const std::exception&)
        {
            CloseHandle(hProcess);
        }
    }
    return 0;
}

QString QQManger::scanLink(int pid)
{
    QString url = "https://tankstorm-qqgame.sincetimes.com/?";
    QStringList hexURLParts;
    for (const QChar& ch : url)
    {
        const auto asciiValue = static_cast<uchar>(ch.toLatin1());
        QString hexStr = QString("%1").arg(asciiValue, 2, 16, QLatin1Char('0')).toUpper();
        hexURLParts.append(hexStr);
    }
    const QByteArray hexData = hexURLParts.join(" ").toUtf8(); // 保存到局部变量
    const char* hexURL = hexData.constData(); // 安全使用

    const HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (hProcess == nullptr || hProcess == INVALID_HANDLE_VALUE) return nullptr;

    try
    {
        const auto scanner = new MemoryScanner(hProcess);
        const auto res = scanner->Search(hexURL);
        if (res.empty())
        {
            CloseHandle(hProcess);
            return nullptr;
        }
        const auto mem = new Memory(hProcess, res.front());
        auto rawURL = QString::fromStdString(mem->ReadString(0x00));
        if (const int regionIndex = rawURL.indexOf("&region="); regionIndex != -1) rawURL = rawURL.left(regionIndex);
        CloseHandle(hProcess);
        return rawURL;
    }
    catch (const std::exception&)
    {
        CloseHandle(hProcess);
        return nullptr;
    }
}

void QQManger::loadConfig()
{
    tableWidget->setRowCount(0);

    QJsonArray accounts = state.config["account"].toArray();
    std::vector<QJsonObject> accountObjects;

    for (auto val : accounts) if (val.isObject()) accountObjects.push_back(val.toObject());

    std::sort(accountObjects.begin(), accountObjects.end(), [](const QJsonObject& a, const QJsonObject& b)
    {
        return a["order"].toInt(0) < b["order"].toInt(0);
    });

    QJsonArray sortedAccounts;
    for (const auto& obj : accountObjects) sortedAccounts.append(obj);

    for (auto accountVal : accounts)
    {
        QJsonObject account = accountVal.toObject();

        insertRow(
            account["remark"].toString(),
            account["qq"].toString(),
            account["password"].toString(),
            account["id"].toString(),
            account["link"].toString()
        );
    }
}

void QQManger::saveConfig()
{
    QJsonArray accountArray;
    QHash<QString, QJsonArray> existingReds;

    for (const auto accountRef : state.config["account"].toArray())
    {
        QJsonObject accountObj = accountRef.toObject();
        existingReds.insert(accountObj["remark"].toString(), accountObj["red"].toArray());
    }

    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        QJsonObject accountObj;

        auto remarkItem = tableWidget->item(row, RemarkCol);
        auto qqItem = tableWidget->item(row, QQNumberCol);
        auto pwdItem = tableWidget->item(row, PasswordCol);
        auto idItem = tableWidget->item(row, GameIDCol);
        auto linkItem = tableWidget->item(row, LinkCol);

        accountObj["order"] = row;
        accountObj["remark"] = remarkItem ? remarkItem->text() : "";
        accountObj["qq"] = qqItem ? qqItem->text() : "";
        accountObj["password"] = pwdItem ? pwdItem->text() : "";
        accountObj["id"] = idItem ? idItem->text() : "";
        accountObj["link"] = linkItem ? linkItem->text() : "";

        if (existingReds.contains(accountObj["remark"].toString()))
        {
            accountObj["red"] = existingReds[accountObj["remark"].toString()];
        }
        else
        {
            accountObj["red"] = QJsonArray();
        }

        accountArray.append(accountObj);
    }

    state.config["account"] = accountArray;
    emit configChanged();
}

void QQManger::handleDelete(int row)
{
    if (row < 0 || row >= tableWidget->rowCount()) return;

    auto remarkItem = tableWidget->item(row, RemarkCol);
    QString remark = remarkItem ? remarkItem->text() : "未知账号";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "确认删除",
        QString("确定要删除账号[%1]吗？").arg(remark),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) return;

    tableWidget->removeRow(row);
    auto modifiedRemark = remark.trimmed().replace("/", "_").replace("\\", "_");
    auto storagePath = QCoreApplication::applicationDirPath() + "/web_profile/login/" + modifiedRemark;
    QDir(storagePath).removeRecursively();
}

void QQManger::handleTest(int row)
{
    auto remarkItem = tableWidget->item(row, RemarkCol);
    auto linkItem = tableWidget->item(row, LinkCol);
    auto url = QUrl(linkItem->text());
    auto remark = remarkItem->text();

    if (!url.isValid())
    {
        setStatus(row, Empty);
        return;
    }

    setStatus(row, Testing);

    auto reply = networkManager->get(QNetworkRequest(url));
    auto timeoutTimer = new QTimer(reply);
    timeoutTimer->setSingleShot(true);

    connect(timeoutTimer, &QTimer::timeout, [reply]() { if (reply->isRunning()) reply->abort(); });

    connect(reply, &QNetworkReply::finished, [this, reply, remark, timeoutTimer]()
    {
        timeoutTimer->stop();
        Status status;

        if (reply->error() == QNetworkReply::NoError)
        {
            status = QString::fromUtf8(reply->readAll()).toUpper().contains("ID:") ? Valid : Invalid;
        }
        else
        {
            status = reply->error() == QNetworkReply::OperationCanceledError ? Timeout : NetError;
        }

        for (int i = 0; i < tableWidget->rowCount(); ++i)
        {
            if (tableWidget->item(i, RemarkCol)->text() == remark)
            {
                setStatus(i, status);
                break;
            }
        }

        reply->deleteLater();
        timeoutTimer->deleteLater();
    });

    timeoutTimer->start(30000);
}

void QQManger::handleLogin(int row)
{
    auto remarkItem = tableWidget->item(row, RemarkCol);
    auto passwordItem = tableWidget->item(row, PasswordCol);
    auto idItem = tableWidget->item(row, GameIDCol);
    auto linkItem = tableWidget->item(row, LinkCol);
    QString remark = remarkItem ? remarkItem->text() : "未知账号";
    QString password = passwordItem ? passwordItem->text() : "";
    QString id = idItem ? idItem->text() : QString();

    if (id.size() > 5)
    {
        if (const auto pid = scanPID(id))
        {
            if (const auto url = scanLink(pid); url != nullptr)
            {
                linkItem->setText(url);
                handleTest(row);
                return;
            }
        }
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(password);
    auto browser = new QQBrowser(this, remark);
    connect(browser, &QQBrowser::linkDetected, [this, &row](const QUrl& url)
    {
        auto linkItem = tableWidget->item(row, LinkCol);
        linkItem->setText(url.toString());

        setStatus(row, Valid);
    });
    browser->exec();
}

void QQManger::onAdd()
{
    bool ok;
    QString remark;

    while (true)
    {
        remark = QInputDialog::getText(this, "添加账号", "请输入QQ备注(设置后无法修改！)", QLineEdit::Normal, "", &ok);

        if (!ok) return;

        if (remark.length() == 0 || remark.length() > 10)
        {
            QMessageBox::warning(this, "输入错误", "备注应当为1-10字符");
            continue;
        }

        bool exists = false;
        for (int i = 0; i < tableWidget->rowCount(); ++i)
        {
            if (tableWidget->item(i, RemarkCol)->text() == remark)
            {
                exists = true;
                break;
            }
        }

        if (exists)
        {
            QMessageBox::warning(this, "输入错误", "备注名称已存在，请重新输入！");
            continue;
        }

        break;
    }

    insertRow(remark);
}

void QQManger::onTestAll() { for (int row = 0; row < tableWidget->rowCount(); ++row) handleTest(row); }

void QQManger::onLoginAll()
{
    for (int row = 0; row < tableWidget->rowCount(); ++row) { setStatus(row, Waiting); }
    for (int row = 0; row < tableWidget->rowCount(); ++row) { handleLogin(row); }
}

void QQManger::closeEvent(QCloseEvent* event)
{
    saveConfig();
    QDialog::closeEvent(event);
}
