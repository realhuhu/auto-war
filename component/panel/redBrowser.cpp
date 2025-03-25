#include "redBrowser.h"

RedBrowser::RedBrowser(
        const QString &qqRemark,
        const QString &redRemark,
        const QString &link,
        int region
) : remark(redRemark), workerThread(nullptr) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("%1 %2 %3区").arg(qqRemark, redRemark, QString::number(region)));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedWidth(970);
    setMinimumHeight(640);
    setMaximumHeight(890);
    resize(970, 890);

    QWebEngineProfile *profile = nullptr;
    auto modifiedRemark = redRemark.trimmed().replace("/", "_").replace("\\", "_");
    if (profileMap.contains(modifiedRemark)) {
        profile = profileMap.value(modifiedRemark);
    } else {
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/game/" + modifiedRemark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        profile = new QWebEngineProfile(modifiedRemark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        profileMap.insert(modifiedRemark, profile);
    }

    url = QString("%1&region=%2").arg(link, QString::number(region - 1));

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    browser = new QWebEngineView(this);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(url);
    mainLayout->addWidget(browser);

    emit log(QString("%1 游戏已打开").arg(redRemark));

    runCommand();
    runCommand();
}

void RedBrowser::refresh() const {
    emit log(QString("刷新游戏"));
    browser->load(url);
}


void RedBrowser::runCommand() {
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
    }

    workerThread = new QThread(this);
    connect(workerThread, &QThread::started, this, [this]() {
        test(remark);
        workerThread->quit();
        emit log("已结束");
    });

    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::destroyed, this, [this]() {
        QMutexLocker locker(&workerMutex);
        workerThread = nullptr;
    });

    workerThread->start();
}

void RedBrowser::closeEvent(QCloseEvent *event) {
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
    }

    if (browser) {
        browser->deleteLater();
        browser = nullptr;
    }

    emit RedBrowser::closed(remark);
    QDialog::closeEvent(event);
}

RedBrowser::~RedBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}

QMap<QString, QWebEngineProfile *> RedBrowser::profileMap;
