#include "redManager.h"

RedManger::RedManger(QWidget *parent) : QDialog(parent) {
    setWindowTitle("红警账号配置");
    resize(480, 320);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto mainLayout = new QVBoxLayout(this);

    QStringList headers;
    headers << "备注" << "QQ账号" << "区服" << "操作";
    tableWidget = new QTableWidget(0, 4);
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #87CEFA;
            border: 1px solid #E0E0E0;
        }
    )");

    mainLayout->addWidget(tableWidget);

    auto buttonContainer = new QWidget();
    auto buttonLayout = new QHBoxLayout(buttonContainer);
    auto addButton = new QPushButton("添加红警账号");
    connect(addButton, &QPushButton::clicked, this, &RedManger::addNew);
    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);

    mainLayout->addWidget(buttonContainer);

    loadConfig();
}

int RedManger::getIndex(QObject *button) const {
    QWidget *buttonWidget = qobject_cast<QWidget *>(button);

    if (!buttonWidget) return -1;

    for (int i = 0; i < tableWidget->rowCount(); ++i) {
        if (tableWidget->cellWidget(i, ActionCol) == buttonWidget) return i;
    }

    return -1;
}

void RedManger::insertRow(const QString &remark, const QString &qq, const QString &region) {
    const int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    auto remarkItem = new QTableWidgetItem(remark);
    auto qqItem = new QTableWidgetItem(qq);
    auto regionItem = new QTableWidgetItem(region);
    remarkItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);
    qqItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);
    regionItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);

    auto buttonWidget = new QWidget();
    auto btnLayout = new QHBoxLayout(buttonWidget);
    btnLayout->setContentsMargins(2, 0, 2, 0);
    auto configBtn = new QPushButton("设置");
    auto deleteBtn = new QPushButton("删除");
    deleteBtn->setStyleSheet("background-color: #ff4444; color: white;");
    btnLayout->addWidget(configBtn);
    btnLayout->addWidget(deleteBtn);

    tableWidget->setItem(row, RemarkCol, remarkItem);
    tableWidget->setItem(row, QQCol, qqItem);
    tableWidget->setItem(row, RegionCol, regionItem);
    tableWidget->setCellWidget(row, ActionCol, buttonWidget);

    connect(configBtn, &QPushButton::clicked, [this]() {
        int index = getIndex(sender()->parent());
        if (index != -1) handleConfig(index);
    });

    connect(deleteBtn, &QPushButton::clicked, [this]() {
        int index = getIndex(sender()->parent());
        if (index != -1) handleDelete(index);
    });

}

void RedManger::loadConfig() {
    tableWidget->setRowCount(0);

    const auto accounts = state.config["account"].toArray();
    for (const auto &accountVal: accounts) {
        const auto account = accountVal.toObject();
        const auto qqRemark = account["remark"].toString();

        const auto redAccounts = account["red"].toArray();
        for (const auto &redVal: redAccounts) {
            const auto red = redVal.toObject();

            const auto remark = red["remark"].toString();
            const auto region = QString::number(red["region"].toInt());

            insertRow(remark, qqRemark, region);
        }
    }
}

void RedManger::saveConfig() {
    auto accounts = state.config["account"].toArray();
    for (auto accountRef: accounts) {
        auto account = accountRef.toObject();
        account["red"] = QJsonArray();
        accountRef = account;
    }

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        auto qqItem = tableWidget->item(row, QQCol);
        auto remarkItem = tableWidget->item(row, RemarkCol);
        auto regionItem = tableWidget->item(row, RegionCol);

        QJsonObject redConfig;
        redConfig["remark"] = remarkItem->text();
        redConfig["region"] = regionItem->text().toInt();
        redConfig["order"] = row;


        const auto accountKey = qqItem->text();
        for (auto accountRef: accounts) {
            auto account = accountRef.toObject();
            if (account["remark"] == accountKey) {
                auto reds = account["red"].toArray();
                reds.append(redConfig);
                account["red"] = reds;
                accountRef = account;
                break;
            }
        }
    }

    state.config["account"] = accounts;
    emit configChanged();
}

void RedManger::handleConfig(int row) {

}

void RedManger::handleDelete(int row) {
    if (row < 0 || row >= tableWidget->rowCount())return;

    auto remarkItem = tableWidget->item(row, RemarkCol);
    QString remark = remarkItem ? remarkItem->text() : "未知账号";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
            this,
            "确认删除",
            QString("确定要删除账号[%1]吗？").arg(remark),
            QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) tableWidget->removeRow(row);
}

void RedManger::addNew() {
    QDialog dialog(this);
    dialog.setWindowTitle("添加红警账号");
    QFormLayout form(&dialog);

    auto remarkEdit = new QLineEdit(&dialog);
    form.addRow("备注:", remarkEdit);

    auto qqCombo = new QComboBox(&dialog);
    for (auto account: state.config["account"].toArray()) {
        auto obj = account.toObject();
        qqCombo->addItem(obj["remark"].toString());
    }
    form.addRow("QQ:", qqCombo);

    auto regionSpin = new QSpinBox(&dialog);
    regionSpin->setMinimum(1);
    regionSpin->setMaximum(9999);
    form.addRow("区服:", regionSpin);

    QDialogButtonBox btnBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(&btnBox);
    QObject::connect(&btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    const auto remark = remarkEdit->text().trimmed();
    const auto qq = qqCombo->currentText();
    const auto region = QString::number(regionSpin->value());

    if (remark.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "备注不能为空");
        return;
    }

    if (remark.length() == 0 || remark.length() > 10) {
        QMessageBox::warning(this, "输入错误", "备注应当为1-10字符");
        return;
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
        return;
    }

    insertRow(remark, qq, region);
}

void RedManger::closeEvent(QCloseEvent *event) {
    saveConfig();
    QDialog::closeEvent(event);
}



