#include "link.h"

LinkInterceptor::LinkInterceptor(QObject *parent) : QWebEngineUrlRequestInterceptor(parent) {}

void LinkInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    const QUrl url = info.requestUrl();
    if (url.host() == "tankstorm-qqgame.sincetimes.com") emit linkDetected(url);
}
