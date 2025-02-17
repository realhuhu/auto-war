#include "browser.h"


BrowserWindow::BrowserWindow(QWidget *parent) {
    setWindowTitle("红警浏览器");
    auto *mainLayout = new QHBoxLayout(this);

    auto *view = new QWebEngineView;
    view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true); // 注意：PluginsEnabled在现代Qt中可能不起作用
    view->load(QUrl("https://qqgame.qq.com/webappframe/?appid=10407"));
    view->setMinimumWidth(985);
    view->setMinimumHeight(935);

    panel = new PanelWidget(this);

    mainLayout->addWidget(view);
    mainLayout->addWidget(panel);
}

int main(int argc, char *argv[]) {
    state.scale = 1;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    auto cmd = realloc(argv, (argc + 1) * sizeof(char *));
    if (cmd == nullptr) exit(0);
    argv = (char **) cmd;
    argv[argc++] = strdup(QByteArray("--register-pepper-plugins='pepflashplayer.dll;application/x-demo'").constData());


    QApplication app(argc, argv);
    BrowserWindow window;
    window.show();
    return QApplication::exec();
}

