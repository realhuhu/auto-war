#include "redController.h"

RedController::RedController(QString redRemark, QWidget *parent) : remark(std::move(redRemark)), QWidget(parent) {
    auto mainLayout = new QVBoxLayout(this);

    auto textEdit = new QTextEdit(this);
    mainLayout->addWidget(textEdit);

    auto footLayout = new QHBoxLayout();
    auto *refreshButton = new QPushButton("刷新游戏", this);
    auto *runButton = new QPushButton("执行命令", this);
    auto *stopButton = new QPushButton("停止命令", this);
    auto *clearButton = new QPushButton("清空输出", this);
    footLayout->addWidget(refreshButton);
    footLayout->addWidget(runButton);
    footLayout->addWidget(stopButton);
    footLayout->addWidget(clearButton);

    mainLayout->addLayout(footLayout);

    connect(refreshButton, &QPushButton::clicked, [this] {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
                this,
                "刷新游戏",
                QString("确认刷新窗口%1？").arg(remark),
                QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) emit refreshBrowser();
    });
}
