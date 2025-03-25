#include "qqBrowser.h"

LinkInterceptor::LinkInterceptor(QObject *parent) : QWebEngineUrlRequestInterceptor(parent) {}

void LinkInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    const QUrl url = info.requestUrl();
    if (url.host() == "qqgame.app100616028.twsapp.com") emit linkDetected(url);
}

QQBrowser::QQBrowser(QWidget *parent, const QString &remark) : QDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("登录账号：%1").arg(remark));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(970, 640);

    QWebEngineProfile *profile = nullptr;
    LinkInterceptor *interceptor = nullptr;
    auto modifiedRemark = remark.trimmed().replace("/", "_").replace("\\", "_");
    if (profileMap.contains(modifiedRemark)) {
        profile = profileMap.value(modifiedRemark);
        interceptor = interceptorMap.value(modifiedRemark);
    } else {
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/login/" + modifiedRemark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        interceptor = new LinkInterceptor();
        profile = new QWebEngineProfile(modifiedRemark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        profile->setUrlRequestInterceptor(interceptor);
        profileMap.insert(modifiedRemark, profile);
        interceptorMap.insert(modifiedRemark, interceptor);
    }
    connect(interceptor, &LinkInterceptor::linkDetected, this, &QQBrowser::onLinkDetected);

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    browser = new QWebEngineView(this);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));
    mainLayout->addWidget(browser);
}


void QQBrowser::onLinkDetected(QUrl url) {
    emit linkDetected(std::move(url));
    close();
}

void QQBrowser::closeEvent(QCloseEvent *event) {
    if (browser) {
        browser->deleteLater();
        browser = nullptr;
    }

    QDialog::closeEvent(event);
}

QQBrowser::~QQBrowser() {
    if (browser) {
        browser->page()->deleteLater();
        browser->close();
    }
}

QMap<QString, QWebEngineProfile *> QQBrowser::profileMap;
QMap<QString, LinkInterceptor *> QQBrowser::interceptorMap;
