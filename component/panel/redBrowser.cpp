#include "redBrowser.h"

RedBrowser::RedBrowser(
        QTabWidget *tabWidget,
        const QString &qqRemark,
        const QString &redRemark,
        const QString &link,
        int region
) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("(%1)%2-%3-%4区").arg(WIdToQSting(winId()), qqRemark, redRemark, QString::number(region)));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedWidth(970);
    setMinimumHeight(640);
    setMaximumHeight(890);
    resize(970, 890);

    url = QString("%1&region=%2").arg(link, QString::number(region - 1));
    auto remark = redRemark.trimmed().replace("/", "_").replace("\\", "_");

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QWebEngineProfile *profile = nullptr;

    if (profileMap.contains(remark)) {
        profile = profileMap.value(remark);
    } else {
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/game/" + remark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        profile = new QWebEngineProfile(remark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        profileMap.insert(remark, profile);
    }

    browser = new QWebEngineView(this);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(url);

    mainLayout->addWidget(browser);

    panel = new ControlPanel(winId(), tabWidget);
    connect(panel, &ControlPanel::refreshBrowser, this, &RedBrowser::refresh);
}

void RedBrowser::refresh() const { browser->load(url); }

void RedBrowser::closeEvent(QCloseEvent *event) {
    if (!browser) return;

    browser->deleteLater();
    browser = nullptr;

    emit RedBrowser::closed(winId());

    QDialog::closeEvent(event);
}

RedBrowser::~RedBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}

QMap<QString, QWebEngineProfile *> RedBrowser::profileMap;
