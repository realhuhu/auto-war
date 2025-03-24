#include "redGameBrowser.h"

RedGameBrowser::RedGameBrowser(
        QTabWidget *tabWidget,
        const QString &qqRemark,
        const QString &redRemark,
        const QString &link,
        int region
) : remark(redRemark) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("%1 %2 %3区").arg(qqRemark, redRemark, QString::number(region)));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedWidth(970);
    setMinimumHeight(640);
    setMaximumHeight(890);
    resize(970, 890);

    url = QString("%1&region=%2").arg(link, QString::number(region - 1));
    auto modifiedRemark = redRemark.trimmed().replace("/", "_").replace("\\", "_");

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QWebEngineProfile *profile = nullptr;

    if (profileMap.contains(modifiedRemark)) {
        profile = profileMap.value(modifiedRemark);
    } else {
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/game/" + modifiedRemark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        profile = new QWebEngineProfile(modifiedRemark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
        profileMap.insert(modifiedRemark, profile);
    }

    browser = new QWebEngineView(this);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(url);

    mainLayout->addWidget(browser);

    panel = new RedController(redRemark, tabWidget);

    connect(panel, &RedController::refreshBrowser, this, &RedGameBrowser::refresh);
    connect(this, &RedGameBrowser::log, panel, &RedController::log);

    emit log(QString("%1 游戏已打开").arg(redRemark));
}

void RedGameBrowser::refresh() const { browser->load(url); }

void RedGameBrowser::closeEvent(QCloseEvent *event) {
    if (!browser) return;

    browser->deleteLater();
    browser = nullptr;

    emit RedGameBrowser::closed(remark);

    QDialog::closeEvent(event);
}

RedGameBrowser::~RedGameBrowser() {
    if (!browser) return;

    browser->page()->deleteLater();
    browser->close();
}

QMap<QString, QWebEngineProfile *> RedGameBrowser::profileMap;
