#include "main.h"
#include <Windows.h>
#include <functional>

#include <QDialog>
#include <QThread>
#include <QWidget>
#include <QScreen>
#include <QDateTime>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

#include "state.h"
#include "task/task.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("红警自动");
    resize(420, 280);

    // 获取主屏幕
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    auto *mainLayout = new QVBoxLayout(this);

    auto *controlLayout = new QHBoxLayout();

    auto *get_hwnd_button = new QPushButton("获取窗口");
    auto *execute_button = new QPushButton("执行命令");
    auto *stop_button = new QPushButton("停止命令");
    auto *clear_button = new QPushButton("清空输出");

    connect(get_hwnd_button, &QPushButton::clicked, this, &MainWindow::start_hwnd_capture);
    connect(execute_button, &QPushButton::clicked, this, &MainWindow::select_command);
    connect(stop_button, &QPushButton::clicked, this, &MainWindow::stop_command);
    connect(clear_button, &QPushButton::clicked, this, &MainWindow::clear_text);

    controlLayout->addWidget(get_hwnd_button);
    controlLayout->addWidget(execute_button);
    controlLayout->addWidget(stop_button);
    controlLayout->addWidget(clear_button);

    mainLayout->addLayout(controlLayout);

    output_text = new QTextEdit();
    output_text->setReadOnly(true);
    mainLayout->addWidget(output_text);

    command_options["公会报名"] = guild_war;
    command_options["军备获取"] = arms_compound;
    command_options["剿灭将领"] = exterminate_enemy;
    command_options["国家争霸"] = country_arena;
    command_options["世界争霸"] = world_arena;
    command_options["国家战争"] = country_war;
    commands = QStringList({
                                   "公会报名", "军备获取", "剿灭将领",
                                   "国家争霸", "世界争霸", "国家战争"
                           });

    // 初始时不安装钩子
    hook = nullptr;
    isWaitingForHwnd = false;
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    onLogText("使用方法: 先获取窗口，再执行命令", "blue");

    // 连接日志信号和槽函数
    connect(this, &MainWindow::logMessage, this, &MainWindow::onLogMessage);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (state.currentThread && state.currentThread->isRunning()) {
        state.currentThread->quit();
        state.currentThread->wait();
    }
    // 移除鼠标钩子
    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    QWidget::closeEvent(event);
}

void MainWindow::onLogText(const QString &text, const QColor &color) const {
    // 创建HTML格式的字符串，其中包含了颜色信息
    QString html = QString("<div style=\"color:%1;\">%2</div>").arg(color.name(), text);

    // 将HTML字符串追加到QTextEdit中
    output_text->append(html);
}

void MainWindow::onLogMessage(const QString &text, const QColor &color) const {
    QDateTime currentDateTime = QDateTime::currentDateTime();

    // 提取时间部分
    QTime currentTime = currentDateTime.time();

    // 格式化时间为字符串 "HH:MM:SS"
    QString timeString = currentTime.toString("hh:mm:ss");

    QString html = QString(
            R"(
                <div>
                <span style="color:white;background-color:green;margin_right 5px">&nbsp;%1&nbsp;</span>
                <span style="color:%2;">%3</span>
                </div>
                )"
    ).arg(timeString, color.name(), text);

    // 将HTML字符串追加到QTextEdit中
    output_text->append(html);
}

void MainWindow::start_hwnd_capture() {
    onLogText("请用鼠标点击游戏窗口");
    isWaitingForHwnd = true;
    // 安装全局鼠标钩子
    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);
    if (hook == nullptr) {
        onLogText("Failed to set hook");
    }
}

void MainWindow::run_command(const QString &command) {
    if (!state.hwnd) {
        onLogText("请先获取游戏窗口!");
        return;
    }

    if (state.currentThread) {
        state.stopFlag.store(true);
        state.currentThread->quit();
        state.currentThread->wait();
        state.currentThread = nullptr;
        return;
    }

    state.stopFlag.store(false);
    state.currentThread = new QThread(this);

    auto func = command_options[command];
    QObject::connect(state.currentThread, &QThread::started, [func, this]() {
        try {
            func();
            emit logMessage("运行完成", "red");
            // 命令执行完成后，结束线程
            if (state.currentThread) {
                state.currentThread->quit();
                state.currentThread = nullptr;
            }
        } catch (const std::exception &e) {
            if (!state.stopFlag.load()) {
                emit logMessage("出错了: " + QString(e.what()), "red");
            } else {
                emit logMessage("运行完成", "red");
            }

            // 命令执行失败后，也结束线程
            if (state.currentThread) {
                state.currentThread->quit();
                state.currentThread = nullptr;
            }
        }
    });

    QObject::connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
    state.currentThread->start();

    onLogText("开始执行命令: " + command, "blue");
}

void MainWindow::stop_command() {
    if (!state.currentThread || !state.currentThread->isRunning()) {
        onLogText("当前无命令正在执行");
        return;
    }

    state.stopFlag.store(true);
    state.currentThread->quit();
    state.currentThread->wait();
    onLogText("命令已停止执行");
}

void MainWindow::select_command() {
    QDialog selectDialog(this);
    selectDialog.setWindowTitle("选择命令");

    auto *layout = new QVBoxLayout(&selectDialog);

    auto *commandLayout = new QGridLayout();
    int row = 0;
    int col = 0;

    // 确保命令选项的顺序
    for (const auto &command: commands) {
        auto *btn = new QPushButton(command);
        connect(btn, &QPushButton::clicked, [this, command, &selectDialog]() {
            selectDialog.accept();
            run_command(command);
        });
        commandLayout->addWidget(btn, row, col);
        col++;
        if (col >= 3) {  // 每行最多3个按钮
            col = 0;
            row++;
        }
    }

    layout->addLayout(commandLayout);

    if (selectDialog.exec() == QDialog::Accepted) {
        onLogMessage("开始运行", "red");
    }
}

void MainWindow::clear_text() {
    output_text->clear();
}

LRESULT CALLBACK MainWindow::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_LBUTTONDOWN) {
            auto *window = qobject_cast<MainWindow *>(qApp->activeWindow());
            if (window && window->isWaitingForHwnd) {
                auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
                // 获取鼠标点击位置的句柄
                HWND hwnd = WindowFromPoint(pMouseStruct->pt);
                wchar_t buffer[256] = {0};

                // 调用GetWindowTextW函数获取Unicode字符串
                GetWindowTextW(hwnd, buffer, 256);
                if (hwnd) {
                    // 返回转换后的字符串
                    window->onLogText(
                            QString("获取到窗口: ") + QString::fromWCharArray(buffer) + "(0x" +
                            QString::number(reinterpret_cast<qulonglong>(hwnd), 16) + ")", "blue"
                    );
                    window->onLogText("请不要最小化游戏窗口！但可以放在其它窗口后面", "red");
                    window->isWaitingForHwnd = false;
                    state.hwnd = hwnd;
                    // 移除钩子
                    UnhookWindowsHookEx(window->hook);
                    window->hook = nullptr;
                } else {
                    window->onLogText("获取窗口句柄失败", "red");
                }
            }
        }
    }
    // 调用下一个钩子
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}


int main(int argc, char *argv[]) {
//    state.hwnd = reinterpret_cast<HWND>(0x00060AC2);
//    CV::find_positions(CV::get_screen(),R"("C:\Users\huhu\Desktop\123.png")");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();/**/
    return QApplication::exec();
}

#include "main.moc"