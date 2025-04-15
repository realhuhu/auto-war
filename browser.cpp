#include "browser.h"


// 构建PID到父PID的映射
std::map<DWORD, DWORD> BuildPidParentPidMap() {
    std::map<DWORD, DWORD> pidMap;

    // 创建进程快照
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return pidMap;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    // 遍历进程列表
    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return pidMap;
    }

    do {
        pidMap[pe.th32ProcessID] = pe.th32ParentProcessID;
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);
    return pidMap;
}

// 获取所有后代进程的PID
std::vector<DWORD> GetAllChildProcesses(DWORD parentPid) {
    std::map<DWORD, DWORD> pidMap = BuildPidParentPidMap();
    std::vector<DWORD> children;
    std::queue<DWORD> q;
    std::set<DWORD> visited;

    // 初始将父进程加入队列
    q.push(parentPid);
    visited.insert(parentPid);

    // 广度优先搜索遍历子进程
    while (!q.empty()) {
        DWORD currentPid = q.front();
        q.pop();

        // 查找所有直接子进程
        for (const auto &pair: pidMap) {
            DWORD pid = pair.first;
            DWORD ppid = pair.second;

            if (ppid == currentPid && !visited.count(pid)) {
                children.push_back(pid);
                visited.insert(pid);
                q.push(pid); // 继续查找该子进程的后代
            }
        }
    }

    return children;
}


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

    if (!remark.isEmpty()) {
        socket->connectToServer(remark);
        if (!socket->waitForConnected(2000)) { return; }
        connect(socket, &QLocalSocket::readyRead, this, &Browser::onMainData);

        auto modifiedRemark = remark.trimmed().replace("/", "_").replace("\\", "_");
        QString storagePath = QCoreApplication::applicationDirPath() + "/web_profile/game/" + modifiedRemark;
        QDir dir(storagePath);
        if (!dir.exists()) dir.mkpath(".");

        auto profile = new QWebEngineProfile(modifiedRemark, QCoreApplication::instance());
        profile->setPersistentStoragePath(storagePath);
        profile->setCachePath(storagePath + "/cache");
        profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);

        browser->setPage(new QWebEnginePage(profile, browser));
    }


    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    browser->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->load(url);
    mainLayout->addWidget(browser);

    if (!remark.isEmpty()) {
        auto timer = new QTimer(this);
        timer->setInterval(1000); // 每秒触发一次
        connect(timer, &QTimer::timeout, this, [this, timer, elapsed = 0]() mutable {
            elapsed++;

            // 获取进程ID并转换为字符串
            QString pidStr = QString::number(getFlashProcess());

            // 有效PID检查（假设非0为有效值）
            bool validPid = !pidStr.isEmpty() && pidStr != "0";

            if (validPid || elapsed >= 10) {
                QJsonObject obj;
                obj["type"] = "INIT";
                obj["hwnd"] = QVariant(browser->winId()).toString();
                obj["pid"] = validPid ? pidStr : "0"; // 超时后强制设为0

                socket->write(QJsonDocument(obj).toJson());
                timer->stop();
                timer->deleteLater(); // 清理定时器

                // 无论是否发送都停止定时器（符合需求逻辑）
                if (elapsed >= 10) {
                    timer->stop();
                    timer->deleteLater();
                }
            }
        });
        timer->start();
    }
}

void Browser::refresh() const { browser->load(url); }


DWORD Browser::getFlashProcess() {
    DWORD res = 0;
    DWORD currentPid = GetCurrentProcessId();
    std::vector<DWORD> childPids = GetAllChildProcesses(currentPid);

    QStringList conditions;
    for (DWORD pid: childPids) {
        conditions.append(QString("ProcessID=%1").arg(pid));
    }
    QString whereClause = QString("(%1)").arg(conditions.join(" OR "));

    QProcess wmicProcess;
    QStringList args = {
            "process",
            "where",
            whereClause,
            "get",
            "ProcessID,CommandLine",
            "/value"
    };

    wmicProcess.start("wmic", args);

    if (!wmicProcess.waitForStarted() || !wmicProcess.waitForFinished()) return res;

    QByteArray output = wmicProcess.readAllStandardOutput();

    QString outputStr = QString::fromStdString(output.toStdString());

    QStringList lines = outputStr.split(QRegExp("[\r\n]+"), Qt::SkipEmptyParts);
    bool founded = false;
    for (QString &line: lines) {
        if (line.startsWith("ProcessId=") && founded) {
            return line.replace("ProcessId=", "").toInt();
        }
        if (line.contains("--type=ppapi") && !founded) founded = true;
    }

    return 0;
}

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

    QCommandLineOption urlOption("url", "传入获取的链接", "链接", "http://www.bluepc.com.cn/web/utility/Flash%20Player%20%E6%B5%8B%E8%AF%95%E9%A1%B5%E9%9D%A2.htm");
    QCommandLineOption titleOption("title", "传入要设置的窗口标题", "窗口标题", "标题");
    QCommandLineOption remarkOption("remark", "传入红警账号名称", "红警账号名称");
    QCommandLineOption pepperOption("register-pepper-plugins", "传入path/to/pepflashplayer.dll;application/x-demo", "flash插件支持", "pepflashplayer.dll;application/x-demo");

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
