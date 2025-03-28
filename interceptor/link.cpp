#include "link.h"

LinkInterceptor::LinkInterceptor(QObject *parent) : QWebEngineUrlRequestInterceptor(parent) {}

void LinkInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    const QUrl url = info.requestUrl();
    if (url.host() == "qqgame.app100616028.twsapp.com") emit linkDetected(url);
}
