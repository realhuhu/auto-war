#include "browser.h"

AutoWarBrowser::AutoWarBrowser(QTabWidget *tabWidget) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("红警大战(%1)").arg(WIdToQSting(winId())));
    setFixedWidth(985);
    setMinimumHeight(700);
    setMaximumHeight(965);
    resize(985, 965);

    auto *mainLayout = new QHBoxLayout(this);

    browser = new QWebEngineView(this);
    QWebEngineProfile *engineProfile = browser->page()->profile();
    engineProfile->setCachePath("cache");
    engineProfile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
    engineProfile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));

    mainLayout->addWidget(browser);

    panel = new ControlPanel(winId(), tabWidget);
    connect(panel, &ControlPanel::refreshBrowser, this, &AutoWarBrowser::refresh);
}

void AutoWarBrowser::refresh() const { browser->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407")); }

void AutoWarBrowser::closeEvent(QCloseEvent *event) {
    if (!browser) return;

    browser->deleteLater();
    browser = nullptr;

    emit AutoWarBrowser::closed(winId());

    QDialog::closeEvent(event);
}

AutoWarBrowser::~AutoWarBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}