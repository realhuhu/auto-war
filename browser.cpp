#include "browser.h"

Browser::Browser(
        QString redUrl,
        const QString &title,
        QString redRemark,
        QWidget *parent
) : QWidget(parent),
    url(std::move(redUrl)),
    remark(std::move(redRemark)),
    socket(new QLocalSocket(this)),
    browser(new QWebEngineView(this)) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedWidth(970);
    setMinimumHeight(640);
    setMaximumHeight(890);
    resize(970, 890);

    socket->connectToServer(remark);
    if (!socket->waitForConnected(2000)) { return; }
    connect(socket, &QLocalSocket::readyRead, this, &Browser::onMainData);

    auto modifiedRemark = redRemark.trimmed().replace("/", "_").replace("\\", "_");
    QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/game/" + modifiedRemark;

    QDir dir(storagePath);
    if (!dir.exists()) dir.mkpath(".");

    auto profile = new QWebEngineProfile(modifiedRemark, QCoreApplication::instance());
    profile->setPersistentStoragePath(storagePath);
    profile->setCachePath(storagePath + "/cache");
    profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    browser->setPage(new QWebEnginePage(profile, browser));
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(url);
    mainLayout->addWidget(browser);

    QJsonObject obj;
    obj["type"] = "HWND";
    obj["value"] = QVariant(browser->winId()).toString();
    socket->write(QJsonDocument(obj).toJson());
}

void Browser::refresh() const { browser->load(url); }

void Browser::onMainData() const {
    const QString msg = QString::fromUtf8(socket->readAll());

    if (msg == "REFRESH") {
        refresh();
        return;
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));

    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QCommandLineParser parser;
    parser.setApplicationDescription("传入链接打开游戏");
    parser.addHelpOption();

    QCommandLineOption urlOption("url", "传入获取的链接", "链接");
    QCommandLineOption titleOption("title", "传入要设置的窗口标题", "窗口标题");
    QCommandLineOption remarkOption("remark", "传入红警账号名称", "红警账号名称");
    QCommandLineOption pepperOption("register-pepper-plugins", "传入path/to/pepflashplayer.dll;application/x-demo", "flash插件支持");

    parser.addOption(urlOption);
    parser.addOption(titleOption);
    parser.addOption(remarkOption);
    parser.addOption(pepperOption);

    parser.process(app);

    auto browser = new Browser(
            parser.value(urlOption),
            parser.value(titleOption),
            parser.value(remarkOption)
    );
    browser->show();

    return QApplication::exec();
}
