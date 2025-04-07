#include "redBrowser.h"

#include <utility>


Worker::Worker(HWND hwnd, int region, const QString &qqRemark, const QString &redRemark) : workerThread(nullptr) {
    auto emitter = new Emitter();
    env = {
            .hwnd=hwnd,
            .emitter=emitter,
            .stopFlag=new std::atomic<bool>(false),
            .region=region,
            .qqRemark=qqRemark,
            .redRemark=redRemark
    };
    connect(emitter, &Emitter::log, this, &Worker::onEmitterLog);
    connect(emitter, &Emitter::error, this, &Worker::onEmitterError);
}

void Worker::runTask(const QString &command, const std::function<void(Env &env)> &task) {
    if (workerThread) {
        emit toLog("正在运行中，请先结束命令");
        return;
    }

    env.stopFlag->store(false);

    errorList.clear();
    abortMessage.clear();

    workerThread = new QThread(this);
    connect(workerThread, &QThread::started, this, [this, command, task]() {
        emit toLog(QString("开始运行: %1").arg(command), "blue");

        try {
            task(env);
            workerThread->quit();
        } catch (const std::exception &e) {
            if (!env.stopFlag->load()) abortMessage = QString(e.what());
            workerThread->quit();
        }
    }, Qt::DirectConnection);

    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::destroyed, this, [this, command]() {
        QMutexLocker locker(&workerMutex);

        if (abortMessage.isEmpty()) {
            emit toLog(QString("运行完成: %1").arg(command), "blue");
        } else {
            emit toLog(QString("运行出错: %1").arg(abortMessage), "red");
        }

        for (const auto &error: errorList) emit toLog(error, "red");

        if (abortMessage.isEmpty()) {
            if (errorList.empty()) {
                emit toConsole(QString("(%1)[%2]运行完成").arg(env.redRemark, command), "green");
            } else {
                emit toConsole(QString("(%1)[%2]运行完成: %3个警告").arg(env.redRemark, command, QString::number(errorList.size())), "orange");
            }
        } else {
            if (errorList.empty()) {
                emit toConsole(QString("(%1)[%2]运行出错: %3").arg(env.redRemark, command, abortMessage), "red");
            } else {
                emit toConsole(QString("(%1)[%2]运行完成: %3; %4个警告").arg(env.redRemark, command, abortMessage, QString::number(errorList.size())), "red");
            }
        }

        workerThread = nullptr;
    });

    workerThread->start();
}

void Worker::stopTask() {
    env.stopFlag->store(true);

    QMutexLocker locker(&workerMutex);

    if (!workerThread) {
        emit  toLog("当前无命令正在执行");
        return;
    }

    workerThread->quit();
    emit toLog("命令已停止执行", "red");
}


void Worker::onEmitterLog(const QString &text, const QString &color) const { if (!env.stopFlag->load()) emit toLog(text, color); }

void Worker::onEmitterError(const QString &text) { if (!env.stopFlag->load()) errorList.append(text); }

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
    connect(worker, &Worker::toLog, this, &RedBrowser::onLog);
    connect(worker, &Worker::toConsole, this, &RedBrowser::onConsole);

    emit toLog(QString("%1 游戏已打开").arg(redRemark));
}


void RedBrowser::runTask(const QString &command, const std::function<void(Env &)> &task) const {
    if (worker) worker->runTask(command, task);
}

void RedBrowser::stopTask() const {
    if (worker) worker->stopTask();
}

void RedBrowser::onRefresh() const {
    emit toLog(QString("刷新游戏"));
    browser->load(url);
}

void RedBrowser::onLog(const QString &text, const QString &color) const { emit toLog(text, color); }

void RedBrowser::onConsole(const QString &text, const QString &color) const { emit toConsole(text, color); }

void RedBrowser::onRunTask(const QString &command, const std::function<void(Env &)> &task) const { runTask(command, task); }

void RedBrowser::onStopTask() const { stopTask(); }

void RedBrowser::closeEvent(QCloseEvent *event) {
    if (worker) worker->stopTask();

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

