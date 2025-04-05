#include "qqBrowser.h"


QQBrowser::QQBrowser(
        QWidget *parent, const QString &remark
) : QDialog(parent), browser(new QWebEngineView(this)),
    modifiedRemark(remark.trimmed().replace("/", "_").replace("\\", "_")) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("登录账号：%1").arg(remark));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(970, 640);

    QWebEngineProfile *profile = nullptr;
    if (profileMap.contains(modifiedRemark)) {
        profile = profileMap.value(modifiedRemark);
    } else {
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/login/" + modifiedRemark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        auto interceptor = new LinkInterceptor();
        connect(interceptor, &LinkInterceptor::linkDetected, this, &QQBrowser::onLinkDetected);

        profile = new QWebEngineProfile(modifiedRemark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        profile->setUrlRequestInterceptor(interceptor);
        profileMap.insert(modifiedRemark, profile);
    }

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    mainLayout->addWidget(browser);

    browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));
}

void QQBrowser::closeEvent(QCloseEvent *event) {
    if (browser) {
        browser->deleteLater();
        browser = nullptr;
    }

    profileMap.remove(modifiedRemark);
    QDialog::closeEvent(event);
}

QQBrowser::~QQBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}

void QQBrowser::onLinkDetected(QUrl url) {
    emit linkDetected(std::move(url));
    close();
}


QMap<QString, QWebEngineProfile *> QQBrowser::profileMap;
