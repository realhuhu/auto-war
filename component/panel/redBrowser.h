#ifndef RED_RED_GAME_BROWSER_H
#define RED_RED_GAME_BROWSER_H

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
#include "../../task/daily.h"

class RedBrowser : public QDialog {
Q_OBJECT

public:
    QString url;
    QString remark;
    QWebEngineView *browser;
    QBasicMutex workerMutex;
    QThread *workerThread;
    QMap<QString, std::function<void()>> tasks;
    static QMap<QString, QWebEngineProfile *> profileMap;

    explicit RedBrowser(
            const QString &qqRemark,
            const QString &redRemark,
            const QString &link,
            int region
    );

    void closeEvent(QCloseEvent *event) override;

    ~RedBrowser() override;

public slots:

    void refresh() const;

    void runCommand();

signals:

    void closed(QString remark);

    void log(const QString &text, const QString &color = "black") const;

};

#endif //RED_RED_GAME_BROWSER_H
