#ifndef RED_LINK_H
#define RED_LINK_H

#include <QWebEngineUrlRequestInterceptor>

class LinkInterceptor : public QWebEngineUrlRequestInterceptor {
Q_OBJECT
public:
    explicit LinkInterceptor(QObject *parent = nullptr);

    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

signals:

    void linkDetected(QUrl url);
};

#endif //RED_LINK_H
