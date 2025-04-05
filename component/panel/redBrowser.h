#ifndef RED_RED_BROWSER_H
#define RED_RED_BROWSER_H

#include <QDir>
#include <QDialog>
#include <QThread>
#include <QTabWidget>
#include <QBasicMutex>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "redController.h"
#include "../../util/tool.h"
#include "../../util/state.h"
#include "../../processor/env.h"
#include "../../interceptor/storage.h"

class Worker : public QObject {
Q_OBJECT

public:
    Env env;
    QThread *workerThread;
    QBasicMutex workerMutex;

    explicit Worker(HWND hwnd, int region, const QString &qqRemark, const QString &redRemark);

    void runTask(const std::function<void(Env &env)> &task);

    void stopTask();

    void close() const;

public slots:

    void onEmitterLog(const QString &text, const QString &color = "black") const;

signals:

    void toLog(const QString &text, const QString &color = "black") const;
};

class RedBrowser : public QDialog {
Q_OBJECT

public:
    QString url;
    QString redRemark;
    Worker *worker;
    QWebEngineView *browser;
    static QMap<QString, QWebEngineProfile *> profileMap;

    explicit RedBrowser(const QString &link, int region, const QString &qqRemark, const QString &redRemark);

    void runTask(const std::function<void(Env &env)> &task) const;

    void stopTask() const;

    void closeEvent(QCloseEvent *event) override;

    ~RedBrowser() override;

public slots:

    void onRefresh() const;

    void onLog(const QString &text, const QString &color = "black") const;

    void onRunTask(const std::function<void(Env &env)> &task) const;

    void onStopTask() const;

signals:

    void closed(QString remark);

    void toLog(const QString &text, const QString &color = "black") const;

};

#endif //RED_RED_BROWSER_H
