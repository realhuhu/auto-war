#ifndef RED_QQ_LOGIN_BROWSER_H
#define RED_QQ_LOGIN_BROWSER_H

#include <QDir>
#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestInterceptor>

class LinkInterceptor : public QWebEngineUrlRequestInterceptor {
Q_OBJECT
public:
    explicit LinkInterceptor(QObject *parent = nullptr);

    // 拦截请求的核心函数
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

signals:

    void linkDetected(QUrl url);
};

class QQLoginBrowser : public QDialog {
Q_OBJECT

public:
    QWebEngineView *browser;
    static QMap<QString, QWebEngineProfile *> profileMap;
    static QMap<QString, LinkInterceptor*> interceptorMap;

    explicit QQLoginBrowser(QWidget *parent, QString remark);

    void onLinkDetected(QUrl url);

    void closeEvent(QCloseEvent *event) override;

    ~QQLoginBrowser() override;

signals:

    void linkDetected(QUrl url);
};

#endif //RED_QQ_LOGIN_BROWSER_H
