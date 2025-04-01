#include "redBrowser.h"


Worker::Worker(HWND hwnd, int region, const QString &qqRemark, const QString &redRemark) : workerThread(nullptr) {
    env = {
            .hwnd=hwnd,
            .emitter=new Emitter(),
            .stopFlag=new std::atomic<bool>(false),
            .region=region,
            .qqRemark=qqRemark,
            .redRemark=redRemark,
            .setting=loadSetting(state.config, qqRemark, redRemark, state.settingDefault)
    };

    tasks["test"] = test;
}

void Worker::runCommand(const QString &command) {
    if (workerThread) {
        emit log("正在运行中，请先结束命令");
        return;
    }

    env.stopFlag->store(false);
    workerThread = new QThread(this);
    connect(workerThread, &QThread::started, this, [this, command]() {
        try {
            tasks[command](env);
            workerThread->quit();
        } catch (const std::exception &e) {
            if (!env.stopFlag->load()) {
                emit log("出错了: " + QString(e.what()), "red");
                emit log("运行结束", "red");
            } else {
                emit log("运行完成", "red");
            }
            workerThread->quit();
        }
    }, Qt::DirectConnection);

    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::destroyed, this, [this]() {
        QMutexLocker locker(&workerMutex);
        workerThread = nullptr;
    });

    workerThread->start();
}

void Worker::stopCommand() {
    env.stopFlag->store(true);

    QMutexLocker locker(&workerMutex);

    if (!workerThread) {
        log("当前无命令正在执行");
        return;
    }

    workerThread->quit();
    log("命令已停止执行");
}

void Worker::close() const {
    if (!workerThread) return;

    workerThread->quit();
    workerThread->wait();
}

RedBrowser::RedBrowser(
        const QString &link,
        int region,
        const QString &qqRemark,
        const QString &redRemark
) : url(QString("%1&region=%2").arg(link, QString::number(region - 1))),
    redRemark(redRemark),
    browser(new QWebEngineView(this)) {
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
//        profile->setUrlRequestInterceptor(new StorageInterceptor(this));
//        profile->installUrlSchemeHandler("proxy", new StorageProxyHandler(this));
        profileMap.insert(modifiedRemark, profile);
    }

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(url);
    mainLayout->addWidget(browser);

    worker = new Worker(reinterpret_cast<HWND>(browser->winId()), region, qqRemark, redRemark);
    connect(worker, &Worker::log, this, &RedBrowser::onLog);
    connect(worker->env.emitter, &Emitter::log, this, &RedBrowser::onLog);

    emit log(QString("%1 游戏已打开").arg(redRemark));
}

void RedBrowser::refresh() const {
    emit log(QString("刷新游戏"));
    browser->load(url);
}

void RedBrowser::onLog(const QString &text, const QString &color) const { emit log(text, color); }

void RedBrowser::closeEvent(QCloseEvent *event) {
    worker->stopCommand();
    worker->close();

    if (browser) {
        browser->deleteLater();
        browser = nullptr;
    }

    emit closed(redRemark);
    QDialog::closeEvent(event);
}

RedBrowser::~RedBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}

QMap<QString, QWebEngineProfile *> RedBrowser::profileMap;

