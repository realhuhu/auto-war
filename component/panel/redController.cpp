#include "redController.h"

RedController::RedController(
        QString redRemark,
        QWidget *parent
) : remark(std::move(redRemark)), QWidget(parent), logTextEdit(new QTextEdit(this)) {
    auto mainLayout = new QVBoxLayout(this);

    logTextEdit->setReadOnly(true);
    mainLayout->addWidget(logTextEdit);

    auto footLayout = new QHBoxLayout();
    auto refreshButton = new QPushButton("刷新游戏", this);
    auto runButton = new QPushButton("执行命令", this);
    auto stopButton = new QPushButton("停止命令", this);
    auto clearButton = new QPushButton("清空输出", this);
    footLayout->addWidget(refreshButton);
    footLayout->addWidget(runButton);
    footLayout->addWidget(stopButton);
    footLayout->addWidget(clearButton);
    mainLayout->addLayout(footLayout);

    connect(refreshButton, &QPushButton::clicked, this, &RedController::refresh);
    connect(runButton, &QPushButton::clicked, this, [this] {
        emit runCommand("test");
    });
    connect(clearButton, &QPushButton::clicked, this, &RedController::clear);
}

void RedController::log(const QString &text, const QString &color) const {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTime currentTime = currentDateTime.time();
    QString timeString = currentTime.toString("hh:mm:ss");

    logTextEdit->append(QString(
            R"(
                <div>
                <span style="color:white;background-color:green;">&nbsp;%1&nbsp;</span>
                <span style="color:%2;">%3</span>
                <img src=":/ui/transparent.png" height='14' width='1'>
                </div>
                )"
    ).arg(timeString, color, text));
}

void RedController::refresh() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
            this,
            "刷新游戏",
            QString("确认刷新窗口%1？").arg(remark),
            QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) emit refreshBrowser();
}

void RedController::clear() const {
    logTextEdit->clear();
}
