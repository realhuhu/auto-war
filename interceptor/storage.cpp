#include "storage.h"

StorageInterceptor::StorageInterceptor(QObject *parent) : QWebEngineUrlRequestInterceptor(parent) {}

void StorageInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    QUrl url = info.requestUrl();
    if (url.scheme() == "http" && url.path().endsWith(".dat")
        && url.host() == "100616028cdn-1251006671.file.myqcloud.com") {
        qDebug() << "[ORIGINAL]" << url.toString();
        auto newUrl = QUrl("proxy" + url.toString().mid(url.scheme().length()));
        qDebug() << "[TO]" << newUrl.scheme() << newUrl.toString();
        info.redirect(newUrl);
    }
}

StorageProxyHandler::StorageProxyHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent),
                                                            manager(new QNetworkAccessManager(this)) {}

void StorageProxyHandler::requestStarted(QWebEngineUrlRequestJob *job) {
    QUrl url = job->requestUrl();
    QUrl newUrl = QString("http" + url.toString().mid(url.scheme().length()));
    qDebug() << "[PROXY]" << newUrl.toString();

    QNetworkRequest request(newUrl);
    const QMap<QByteArray, QByteArray> headers = job->requestHeaders();
    for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
        request.setRawHeader(it.key(), it.value());
    }

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [job, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            // 创建数据缓冲区
            QBuffer *buffer = new QBuffer(job);
            buffer->setData(data);
            buffer->open(QIODevice::ReadOnly);

            // 调用三参数重载
            job->reply(
                    reply->header(QNetworkRequest::ContentTypeHeader).toByteArray(),
                    buffer
            ); // Qt 5.14+
        } else {
            qDebug() << reply->error();
        }
        reply->deleteLater();
    });
}