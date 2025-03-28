#ifndef RED_QQ_BROWSER_H
#define RED_QQ_BROWSER_H

#include <QDir>
#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "../../interceptor/link.h"

class QQBrowser : public QDialog {
Q_OBJECT

public:
    QWebEngineView *browser;
    static QMap<QString, QWebEngineProfile *> profileMap;
    static QMap<QString, LinkInterceptor *> interceptorMap;

    explicit QQBrowser(QWidget *parent, const QString &remark);

    void onLinkDetected(QUrl url);

    void closeEvent(QCloseEvent *event) override;

    ~QQBrowser() override;

signals:

    void linkDetected(QUrl url);
};

#endif //RED_QQ_BROWSER_H
