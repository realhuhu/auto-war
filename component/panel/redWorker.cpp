#include "redWorker.h"

RedWorker::RedWorker(
        const QString &link,
        int region,
        const QString &qqRemark,
        QString redRemark
) : remark(std::move(redRemark)),
    server(new QLocalServer(this)),
    browserSocket(nullptr),
    workerThread(nullptr),
    browserProcess(new QProcess(this)) {
    auto emitter = new Emitter();
    env = {
            .hwnd=nullptr,
            .emitter=emitter,
            .stopFlag=new std::atomic<bool>(false),
            .region=region,
            .qqRemark=qqRemark,
            .redRemark=remark
    };
    connect(emitter, &Emitter::log, this, &RedWorker::onEmitterLog);
    connect(emitter, &Emitter::error, this, &RedWorker::onEmitterError);

    QLocalServer::removeServer(remark);

    if (!server->listen(remark)) {
        toConsole(QString("(%1)无法与游戏窗口建立连接").arg((remark)), "red");
        return;
    }
    connect(server, &QLocalServer::newConnection, this, &RedWorker::onBrowserConnect);

    browserProcess->start(
            "./游戏窗口.exe", QStringList{
                    "--url", QString("%1&region=%2").arg(link, QString::number(region - 1)),
                    "--title", QString("%1 %2 %3区").arg(qqRemark, remark, QString::number(region)),
                    "--remark", remark,
                    "--register-pepper-plugins", "pepflashplayer.dll;application/x-demo"
            }
    );
    connect(browserProcess, &QProcess::started, this, &RedWorker::onProcessStart);
    connect(browserProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RedWorker::onProcessFinish);
}

void RedWorker::runTask(const QString &command, const std::function<void(Env &env)> &task) {
    if (!env.hwnd) {
        emit toLog("未初始化!", "red");
        return;
    }

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

void RedWorker::stopTask() {
    env.stopFlag->store(true);

    QMutexLocker locker(&workerMutex);

    if (!workerThread) {
        emit toLog("当前无命令正在执行");
        return;
    }

    workerThread->quit();
    emit toLog("命令已停止执行", "red");
}

void RedWorker::onBrowserConnect() {
    browserSocket = server->nextPendingConnection();
    connect(browserSocket, &QLocalSocket::readyRead, this, &RedWorker::onBrowserData);
    connect(browserSocket, &QLocalSocket::disconnected, browserSocket, &QLocalSocket::deleteLater);
}

void RedWorker::onBrowserData() {
    if (!browserSocket || browserSocket->bytesAvailable() <= 0) {
        emit toLog("未初始化!", "red");
        return;
    }

    QByteArray data = browserSocket->readAll();

    auto json = QJsonDocument::fromJson(data).object();

    if (json["type"] == "HWND") {
        auto var = QVariant::fromValue(json["value"].toString());
        auto wid = var.value<WId>();
        env.hwnd = reinterpret_cast<HWND>(wid);
        emit toLog("初始化完成");
    }
}


void RedWorker::onProcessStart() const { emit toConsole(QString("(%1)正在打开游戏窗口").arg(remark)); }

void RedWorker::onProcessFinish(int, QProcess::ExitStatus) {
    stopTask();

    emit closed(remark);
}

void RedWorker::onRunTask(const QString &command, const std::function<void(Env &)> &task) { runTask(command, task); }

void RedWorker::onStopTask() { stopTask(); }

void RedWorker::onEmitterLog(const QString &text, const QString &color) const { if (!env.stopFlag->load()) emit toLog(text, color); }

void RedWorker::onEmitterError(const QString &text) { if (!env.stopFlag->load()) errorList.append(text); }

void RedWorker::onRefreshBrowser() const {
    if (!browserSocket || browserSocket->state() != QLocalSocket::ConnectedState) {
        emit toLog("未初始化!", "red");
        return;
    }

    browserSocket->write(QString("REFRESH").toUtf8());
    browserSocket->flush();
}

