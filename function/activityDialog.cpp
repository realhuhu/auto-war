# include "activityDialog.h"

ActivityResultDialog::ActivityResultDialog(QWidget *parent) : QDialog(parent) {
    this->setWindowTitle("活动预告");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setMinimumWidth(400);
    this->setMinimumHeight(400);

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(5);

    label = new QLabel("获取中...");
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"活动名称", "开始时间", "结束时间"});

    tableView = new QTableView(this);
    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(label);
    layout->addWidget(tableView);
    layout->addWidget(new QLabel("列出的活动一定准确，近期活动一般无遗漏，远期活动可能会增加"));
    setLayout(layout);

    QTimer::singleShot(0, this, SLOT(fetchDataAndUpdateUI()));
}

QVector<ResultEntry> ActivityResultDialog::getActivity() {
    QString text;
    QString urlTemplate = "http://100616028cdn-1251006671.file.myqcloud.com/100616028/res/20120522/config/dll_%1.xml";
    QDateTime currentTime = QDateTime::currentDateTime();
    QDate currentDate = currentTime.date();
    int version = 1;
    QEventLoop loop;
    QNetworkAccessManager manager;
    bool correctDate = false;

    while (true) {
        auto dateStr = currentDate.toString("yyyyMMdd");
        auto versionStr = QString::number(version, 10).rightJustified(2, '0');
        auto resultStr = dateStr + versionStr;

        label->setText("尝试获取: " + dateStr + "...");

        const QString url = QString(urlTemplate).arg(resultStr);

        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        auto tryText = QString::fromUtf8(reply->readAll());
        reply->deleteLater();

        if (!correctDate) text = tryText;

        if (!correctDate && !tryText.contains("errorcode")) correctDate = true;

        if (correctDate && tryText.contains("errorcode")) {
            label->setText(QString("最后更新: %1 v%2").arg(dateStr, QString::number(version - 1)));
            break;
        }

        if (correctDate) {
            text = tryText;
            version++;
        } else {
            currentDate = currentDate.addDays(-1);
        }
    }

    text.replace(QRegularExpression("&(?!amp;)"), "&amp;");

    QXmlStreamReader reader(text);
    QVector<ResultEntry> res;
    QXmlStreamReader::TokenType token;

    while (!reader.atEnd()) {
        token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) break;
    }

    bool skip = false;
    while (!reader.atEnd()) {
        if (skip) {
            skip = false;
        } else {
            reader.readNext();
        }
        if (reader.name() == "panel") {
            auto name = reader.attributes().value("name").toString();
            if (name.isEmpty())continue;

            while (true) {
                reader.readNext();
                if (reader.name() == "topbtn") {
                    QString startStr = reader.attributes().value("start").toString();
                    QString endStr = reader.attributes().value("end").toString();

                    if (name.isEmpty() || startStr.isEmpty() || endStr.isEmpty()) break;
                    auto startTime = getTime(startStr);
                    auto endTime = getTime(endStr);

                    if (startTime <= currentTime) break;

                    ResultEntry entry;
                    entry.name = name;
                    entry.startTime = startTime;
                    entry.endTime = endTime;
                    res.push_back(entry);
                }

                if (reader.name() == "panel") {
                    skip = true;
                    break;
                }
            }
        }
    }

    std::sort(res.begin(), res.end(), [](const ResultEntry &a, const ResultEntry &b) {
        return a.startTime < b.startTime;
    });

    return res;
}

QDateTime ActivityResultDialog::getTime(const QString &timeStr) {
    QDateTime dateTime;
    // 尝试按照 "yyyy:MM:dd:hh:mm:ss" 格式解析
    dateTime = QDateTime::fromString(timeStr, "yyyy:M:d:h:m:s");
    if (dateTime.isValid()) {
        return dateTime;
    }
    // 尝试按照 "yyyy/MM/dd/hh:mm:ss" 格式解析
    dateTime = QDateTime::fromString(timeStr, "yyyy:M:d:h:m");
    return dateTime;
}

void ActivityResultDialog::fetchDataAndUpdateUI() {
    auto data = getActivity();

    model->removeRows(0, model->rowCount());

    for (const auto &entry: data) {
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(entry.name);
        rowItems << new QStandardItem(entry.startTime.toString("yyyy-MM-dd hh:mm"));
        rowItems << new QStandardItem(entry.endTime.toString("yyyy-MM-dd hh:mm"));
        model->appendRow(rowItems);
    }
}