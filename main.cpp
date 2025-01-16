#include <iostream>
#include <Windows.h>
#include <functional>

#include <QMap>
#include <QDialog>
#include <QThread>
#include <QWidget>
#include <QScreen>
#include <QTextEdit>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

#include "state.h"
#include "task.h"


// 简单的模拟命令函数
void guild_war() {
    std::cout << "guild_war function called" << std::endl;
}

void arms_compound() {
    std::cout << "arms_compound function called" << std::endl;
}

void country_arena() {
    std::cout << "country_arena function called" << std::endl;
}

void world_arena() {
    std::cout << "world_arena function called" << std::endl;
}

void country_war() {
    std::cout << "country_war function called" << std::endl;
}

class MainWindow : public QWidget {
Q_OBJECT
public:
    QMap<QString, std::function<void()>> command_options;
    bool isWaitingForHwnd = false;
    HHOOK hook;  // 新增：用于存储鼠标钩子句柄


    explicit MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
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

        state.output_text = new QTextEdit();
        state.output_text->setReadOnly(true);
        mainLayout->addWidget(state.output_text);

        command_options["公会报名"] = guild_war;
        command_options["军备获取"] = arms_compound;
        command_options["剿灭将领"] = exterminate_enemy;
        command_options["国家争霸"] = country_arena;
        command_options["世界争霸"] = world_arena;
        command_options["国家战争"] = country_war;

        // 初始时不安装钩子
        hook = nullptr;
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        state.appendColoredText("使用方法: 先获取窗口，再执行命令", "blue");
    }


    void closeEvent(QCloseEvent *event) override {
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


    ~MainWindow() override = default;


    // 全局鼠标钩子的回调函数
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            if (wParam == WM_LBUTTONDOWN) {
                auto *window = qobject_cast<MainWindow *>(qApp->activeWindow());
                if (window && window->isWaitingForHwnd) {
                    auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
                    // 获取鼠标点击位置的句柄
                    HWND hwnd = WindowFromPoint(pMouseStruct->pt);
                    char buffer[256] = {0};

                    // 调用GetWindowText函数获取窗口标题
                    // 注意：这里使用GetWindowTextA来获取ANSI字符串，对于Unicode字符串应使用GetWindowTextW
                    GetWindowTextA(hwnd, buffer, 256);

                    if (hwnd) {
                        // 返回转换后的字符串
                        state.appendColoredText(
                                QString("获取到窗口: ") + buffer + "(0x" +
                                QString::number(reinterpret_cast<qulonglong>(hwnd)) + ")", "blue"
                        );
                        state.appendColoredText("请不要最小化游戏窗口！但可以放在其它窗口后面");
                        window->isWaitingForHwnd = false;
                        state.hwnd = hwnd;
                        // 移除钩子
                        UnhookWindowsHookEx(window->hook);
                        window->hook = nullptr;
                    } else {
                        state.appendColoredText("获取窗口句柄失败");
                    }
                }
            }
        }
        // 调用下一个钩子
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

private slots:

    void start_hwnd_capture() {
        state.appendColoredText("请用鼠标点击游戏窗口");
        isWaitingForHwnd = true;
        // 安装全局鼠标钩子
        hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);
        if (hook == nullptr) {
            state.appendColoredText("Failed to set hook");
        }
    }


    void run_command(const QString &command) {
        if (!state.hwnd) {
            state.appendColoredText("请先获取游戏窗口!");
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

        std::function<void()> func = command_options[command];
        QObject::connect(state.currentThread, &QThread::started, [func]() {
            try {
                func();
                state.appendColoredText("运行完成", "blue");
                // 命令执行完成后，结束线程
                if (state.currentThread) {
                    state.currentThread->quit();
                    state.currentThread = nullptr;
                }
            } catch (const std::exception &e) {
                state.appendColoredText("出错了: ");
                state.appendColoredText(e.what());
                // 命令执行失败后，也结束线程
                if (state.currentThread) {
                    state.currentThread->quit();
                    state.currentThread = nullptr;
                }
            }
        });

        QObject::connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
        state.currentThread->start();

        state.appendColoredText("开始执行命令: " + command, "blue");
    }


    static void stop_command() {
        if (!state.currentThread || !state.currentThread->isRunning()) {
            state.output_text->append("当前无命令正在执行");
            return;
        }

        state.stopFlag.store(true);
        state.currentThread->quit();
        state.currentThread->wait();
        state.output_text->append("命令已停止执行");
    }


    void select_command() {
        QDialog selectDialog(this);
        selectDialog.setWindowTitle("选择命令");

        auto *layout = new QVBoxLayout(&selectDialog);

        auto *commandLayout = new QGridLayout();
        auto it = command_options.begin();
        int row = 0;
        int col = 0;

        while (it != command_options.end()) {
            const QString &command = it.key();
            auto *btn = new QPushButton(command);
            connect(btn, &QPushButton::clicked, [this, command, &selectDialog]() {
                selectDialog.accept();
                run_command(command);
            });
            commandLayout->addWidget(btn, row, col);
            col++;
            if (col % 3 == 0) {
                col = 0;
                row++;
            }
            ++it;
        }

        layout->addLayout(commandLayout);

        if (selectDialog.exec() == QDialog::Accepted) {
            // 对话框接受后执行相应操作
        }
    }


    static void clear_text() {
        state.output_text->clear();
    }
};


int main(int argc, char *argv[]) {
    qRegisterMetaType<QTextCursor>("QTextCursor");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return QApplication::exec();
}


#include "main.moc"