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
    QTextEdit *output_text;
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

        auto *get_hwnd_button = new QPushButton("获取句柄");
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

        state.output_text = output_text;

        command_options["公会报名"] = guild_war;
        command_options["军备获取"] = arms_compound;
        command_options["剿灭将领"] = exterminate_enemy;
        command_options["国家争霸"] = country_arena;
        command_options["世界争霸"] = world_arena;
        command_options["国家战争"] = country_war;

        // 初始时不安装钩子
        hook = nullptr;
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
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

protected:
    // 全局鼠标钩子的回调函数
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            if (wParam == WM_LBUTTONDOWN) {
                auto *window = qobject_cast<MainWindow *>(qApp->activeWindow());
                if (window && window->isWaitingForHwnd) {
                    auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
                    // 获取鼠标点击位置的句柄
                    HWND hwnd = WindowFromPoint(pMouseStruct->pt);
                    if (hwnd) {
                        window->output_text->append(
                                QString("获取到窗口句柄: 0x%1").arg(reinterpret_cast<qulonglong>(hwnd), 0, 16)
                        );
                        window->isWaitingForHwnd = false;
                        state.hwnd = hwnd;
                        // 移除钩子
                        UnhookWindowsHookEx(window->hook);
                        window->hook = nullptr;
                    } else {
                        window->output_text->append("获取窗口句柄失败");
                    }
                }
            }
        }
        // 调用下一个钩子
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

private slots:

    void start_hwnd_capture() {
        output_text->append("请用鼠标点击任意窗口");
        isWaitingForHwnd = true;
        // 安装全局鼠标钩子
        hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);
        if (hook == nullptr) {
            output_text->append("Failed to set hook");
        }
    }


    void run_command(const QString &command) {
        if (state.currentThread) {
            state.currentThread->quit();
            state.currentThread = nullptr;
            return;
        }

        state.currentThread = new QThread(this);

        std::function<void()> func = command_options[command];
        QObject::connect(state.currentThread, &QThread::started, [func]() {
            try {
                func();
                state.output_text->append("运行完成");
                // 命令执行完成后，结束线程
                if (state.currentThread) {
                    state.currentThread->quit();
                    state.currentThread = nullptr;
                }
            } catch (const std::exception &e) {
                state.output_text->append(QString("出错了: %1").arg(e.what()));
                // 命令执行失败后，也结束线程
                if (state.currentThread) {
                    state.currentThread->quit();
                    state.currentThread = nullptr;
                }
            }
        });

        QObject::connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
        state.currentThread->start();

        output_text->append(QString("开始执行命令: %1").arg(command));
    }


    void stop_command() const {
        if (!state.currentThread || !state.currentThread->isRunning()) {
            output_text->append("当前无命令正在执行");
            return;
        }

        state.currentThread->quit();
        state.currentThread->wait();
        output_text->append("命令已停止执行");
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


    void clear_text() const {
        output_text->clear();
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