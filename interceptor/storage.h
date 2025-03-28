#ifndef RED_STORAGE_H
#define RED_STORAGE_H

#include <QUrl>
#include <QBuffer>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestInterceptor>

class StorageInterceptor : public QWebEngineUrlRequestInterceptor {
Q_OBJECT
public:
    explicit StorageInterceptor(QObject *parent = nullptr);

    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
};


class StorageProxyHandler : public QWebEngineUrlSchemeHandler {
Q_OBJECT
public:
    explicit StorageProxyHandler(QObject *parent = nullptr);

    void requestStarted(QWebEngineUrlRequestJob *job) override;

private:
    QNetworkAccessManager *manager;
};

#endif //RED_STORAGE_H
