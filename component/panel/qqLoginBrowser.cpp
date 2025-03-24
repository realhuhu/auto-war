#include "qqLoginBrowser.h"

LinkInterceptor::LinkInterceptor(QObject *parent) : QWebEngineUrlRequestInterceptor(parent) {}

void LinkInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    const QUrl url = info.requestUrl();
    if (url.host() == "qqgame.app100616028.twsapp.com") emit linkDetected(url);
}

QQLoginBrowser::QQLoginBrowser(QWidget *parent, QString remark) : QDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("登录账号：%1").arg(remark));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(970, 640);

    remark = remark.trimmed().replace("/", "_").replace("\\", "_");

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QWebEngineProfile *profile = nullptr;
    LinkInterceptor *interceptor = nullptr;

    if (profileMap.contains(remark)) {
        profile = profileMap.value(remark);
        interceptor = interceptorMap.value(remark);
    } else {
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/login/" + remark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        interceptor = new LinkInterceptor();
        profile = new QWebEngineProfile(remark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        profile->setUrlRequestInterceptor(interceptor);
        profileMap.insert(remark, profile);
        interceptorMap.insert(remark, interceptor);
    }

    connect(interceptor, &LinkInterceptor::linkDetected, this, &QQLoginBrowser::onLinkDetected);

    browser = new QWebEngineView(this);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));

    mainLayout->addWidget(browser);
}


void QQLoginBrowser::onLinkDetected(QUrl url) {
    emit linkDetected(std::move(url));
    close();
}

void QQLoginBrowser::closeEvent(QCloseEvent *event) {
    if (!browser) return;

    browser->deleteLater();
    browser = nullptr;

    QDialog::closeEvent(event);
}

QQLoginBrowser::~QQLoginBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}

QMap<QString, QWebEngineProfile *> QQLoginBrowser::profileMap;
QMap<QString, LinkInterceptor *> QQLoginBrowser::interceptorMap;
