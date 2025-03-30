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
#include "../../processor/env.h"
#include "../../task/daily.h"
#include "../../interceptor/storage.h"


class Worker : public QObject {
Q_OBJECT

public:
    Env env;
    QThread *workerThread;
    QBasicMutex workerMutex;
    QMap<QString, std::function<void(const Env &env)>> tasks;

    explicit Worker(HWND hwnd, int region, const QString &qqRemark, const QString &redRemark);

    void runCommand(const QString& command);

    void close() const;

signals:

    void log(const QString &text, const QString &color = "black") const;
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

    void closeEvent(QCloseEvent *event) override;

    ~RedBrowser() override;

public slots:

    void refresh() const;

    void onLog(const QString &text, const QString &color = "black") const;

signals:

    void closed(QString remark);

    void log(const QString &text, const QString &color = "black") const;

};

#endif //RED_RED_BROWSER_H
