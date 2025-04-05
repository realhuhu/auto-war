#include "redController.h"

#include <utility>

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

    connect(refreshButton, &QPushButton::clicked, this, &RedController::onRefresh);
    connect(runButton, &QPushButton::clicked, this, &RedController::onRun);
    connect(stopButton, &QPushButton::clicked, this, &RedController::onStop);
    connect(clearButton, &QPushButton::clicked, this, &RedController::onClear);
}

void RedController::onLog(const QString &text, const QString &color) const {
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

void RedController::onRefresh() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
            this,
            "刷新游戏",
            QString("确认刷新窗口%1？").arg(remark),
            QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) emit toRefreshBrowser();
}


void RedController::onRun() {
    auto dialog = new CmdSelector(this);
    connect(dialog, &CmdSelector::taskCreated, this, &RedController::onTaskCreated);
    dialog->show();
}

void RedController::onStop() { emit toStopTask(); }


void RedController::onClear() const { logTextEdit->clear(); }

void RedController::onTaskCreated(std::function<void(Env &env)> task) { emit toRunTask(std::move(task)); }
