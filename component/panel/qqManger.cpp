#include "qqManger.h"

QQManger::QQManger(QWidget *parent) : QDialog(parent) {
    setWindowTitle("QQ账号配置");
    resize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

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
            padding: 4px;
            border: 1px solid #E0E0E0;
            font-weight: bold;
        }
    )");

    QWidget *buttonContainer = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    QPushButton *addButton = new QPushButton("添加账号");
    QPushButton *testAllButton = new QPushButton("全部测试");
    QPushButton *loginAllButton = new QPushButton("全部登录");
    connect(addButton, &QPushButton::clicked, this, &QQManger::addNew);
    connect(testAllButton, &QPushButton::clicked, this, &QQManger::testAll);
    connect(loginAllButton, &QPushButton::clicked, this, &QQManger::loginAll);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(testAllButton);
    buttonLayout->addWidget(loginAllButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableWidget);
    mainLayout->addWidget(buttonContainer);
}

void QQManger::handleDelete(int row) {
    if (row >= 0 && row < tableWidget->rowCount()) {
        QTableWidgetItem *remarkItem = tableWidget->item(row, RemarkCol);
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
        }
    }
}

void QQManger::handleTest(int row) {
    QString qq = tableWidget->item(row, QQNumberCol)->text();
    QString pwd = tableWidget->item(row, PasswordCol)->text();

    bool success = QRandomGenerator::global()->bounded(2);

    QTableWidgetItem *statusItem = tableWidget->item(row, StatusCol);
    statusItem->setText(success ? "有效" : "已过期");
    statusItem->setForeground(success ? Qt::darkGreen : Qt::red);
    statusItem->setBackground(QColor(240, 240, 240));
}

void QQManger::handleLogin(int row) {
    QString qq = tableWidget->item(row, QQNumberCol)->text();
    QTableWidgetItem *statusItem = tableWidget->item(row, StatusCol);
    statusItem->setText("有效");
    statusItem->setForeground(Qt::darkGreen);
}

void QQManger::addNew() {
    bool ok;
    QString remark;

    while (true) {
        remark = QInputDialog::getText(this, "添加账号", "请输入QQ备注(设置后无法修改！)", QLineEdit::Normal, "", &ok);

        if (!ok) return;

        if (remark.length() == 0 || remark.length() > 10) {
            QMessageBox::warning(this, "错误", "备注应当为1-10字符");
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
            QMessageBox::warning(this, "错误", "备注名称已存在，请重新输入！");
            continue;
        }

        break;
    }

    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    QTableWidgetItem *remarkItem = new QTableWidgetItem(remark);
    remarkItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);
    remarkItem->setBackground(QColor(240, 240, 240));
    tableWidget->setItem(row, RemarkCol, remarkItem);

    for (int col = QQNumberCol; col <= LinkCol; ++col) {
        QTableWidgetItem *item = new QTableWidgetItem();
        tableWidget->setItem(row, col, item);
    }

    QTableWidgetItem *statusItem = new QTableWidgetItem("待测试");
    statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
    statusItem->setForeground(Qt::darkGray);
    statusItem->setBackground(QColor(240, 240, 240));
    tableWidget->setItem(row, StatusCol, statusItem);

    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(buttonWidget);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton *testBtn = new QPushButton("测试");
    QPushButton *loginBtn = new QPushButton("登录");
    QPushButton *deleteBtn = new QPushButton("删除");
    deleteBtn->setStyleSheet("background-color: #ff4444; color: white;");
    btnLayout->addWidget(testBtn);
    btnLayout->addWidget(loginBtn);
    btnLayout->addWidget(deleteBtn);

    tableWidget->setCellWidget(row, ActionCol, buttonWidget);

    connect(testBtn, &QPushButton::clicked, this, [this, row]() { handleTest(row); });
    connect(loginBtn, &QPushButton::clicked, this, [this, row]() { handleLogin(row); });
    connect(deleteBtn, &QPushButton::clicked, this, [this, row]() { handleDelete(row); });
}

void QQManger::testAll() { for (int row = 0; row < tableWidget->rowCount(); ++row) handleTest(row); }

void QQManger::loginAll() { for (int row = 0; row < tableWidget->rowCount(); ++row) handleLogin(row); }