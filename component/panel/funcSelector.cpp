#include "funcSelector.h"

FuncSelector::FuncSelector(QWidget *parent) : QDialog(parent) {
    setWindowTitle("其它功能");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto gridLayout = new QGridLayout(this);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    auto folderBtn = new QPushButton("安装位置", this);
    auto replaceBtn = new QPushButton("替换图片", this);
    auto clickerBtn = new QPushButton("连点器", this);
    auto activityBtn = new QPushButton("活动预告", this);

    connect(folderBtn, &QPushButton::clicked, [this]() {
        QString dirPath = QCoreApplication::applicationDirPath();
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath))) {
            emit toConsole(QString("无法打开程序所在目录：%1请手动打开").arg(dirPath));
        } else {
            emit toConsole(QString("已打开程序所在目录：%1").arg(dirPath));
        }
    });

    connect(replaceBtn, &QPushButton::clicked, [this]() {});

    connect(clickerBtn, &QPushButton::clicked, [this]() {});

    connect(activityBtn, &QPushButton::clicked, [this]() {
        auto *dialog = new ActivityDialog(this);
        dialog->exec();
    });

    gridLayout->addWidget(folderBtn, 0, 0);
    gridLayout->addWidget(replaceBtn, 0, 1);
    gridLayout->addWidget(clickerBtn, 0, 2);
    gridLayout->addWidget(activityBtn, 1, 0);
}
