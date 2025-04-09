#ifndef RED_RED_BROWSER_H
#define RED_RED_BROWSER_H

#include <QDir>
#include <QDialog>
#include <QThread>
#include <QProcess>
#include <QVariant>
#include <QTabWidget>
#include <QBasicMutex>
#include <QHBoxLayout>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "redController.h"
#include "../../util/tool.h"
#include "../../util/state.h"
#include "../../processor/env.h"
#include "../../interceptor/storage.h"

class RedWorker : public QObject {
Q_OBJECT

public:
    Env env;
    QLocalServer *server;
    QProcess *browserProcess;
    QLocalSocket *browserSocket;

    QString remark;
    QThread *workerThread;
    QBasicMutex workerMutex;
    QString abortMessage;
    QList<QString> errorList{};

    explicit RedWorker(const QString &link, int region, const QString &qqRemark, QString redRemark);

    void runTask(const QString &command, const std::function<void(Env &env)> &task);

    void stopTask();

    ~RedWorker();

public slots:

    void onBrowserConnect();

    void onBrowserData();

    void onProcessStart() const;

    void onProcessFinish(int, QProcess::ExitStatus);

    void onRunTask(const QString &command, const std::function<void(Env &)> &task);

    void onStopTask();

    void onEmitterLog(const QString &text, const QString &color = "black") const;

    void onEmitterError(const QString &text);

    void onRefreshBrowser() const;

signals:

    void toLog(const QString &text, const QString &color = "black") const;

    void toConsole(const QString &text, const QString &color = "black") const;

    void closed(const QString &remark);

};

#endif //RED_RED_BROWSER_H
