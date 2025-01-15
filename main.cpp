#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QThread>
#include <functional>
#include <Windows.h>
#include <QScreen>
#include "state.h"

// 简单的模拟命令函数
void guild_war() {
    std::cout << "guild_war function called" << std::endl;
}

void arms_compound() {
    std::cout << "arms_compound function called" << std::endl;
}

void exterminate_enemy() {
    std::cout << "exterminate_enemy function called" << std::endl;
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
protected:
    void mousePressEvent(QMouseEvent* event) override {
        clickHandler(event);
        QWidget::mousePressEvent(event);
    }

private:
    QTextEdit* output_text;
    QMap<QString, std::function<void()>> command_options;
    std::function<void(QMouseEvent*)> clickHandler;

    // 内部辅助函数定义
    void handleMouseClick(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            POINT point;
            GetCursorPos(&point);
            HWND handle = WindowFromPoint(point);
            wchar_t buffer[1024];
            int length = GetWindowTextW(handle, buffer, sizeof(buffer) / sizeof(wchar_t));
            if (length > 0) {
                QString title = QString::fromWCharArray(buffer);
                output_text->append(QString("游戏窗口: %1(%2)").arg(title).arg((quintptr)handle, 0, 16));
                state.hwnd = handle;
                output_text->append("请不要最小化游戏窗口！但可以放在其它窗口后面");
            } else {
                output_text->append("获取窗口标题失败");
            }
            this->setMouseTracking(false);
        }
    }
public:
    explicit MainWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("红警自动");
        resize(420, 280);

        // 获取主屏幕
        QScreen* screen = QApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->geometry();
            int x = (screenGeometry.width() - width()) / 2;
            int y = (screenGeometry.height() - height()) / 2;
            move(x, y);
        }

        auto* mainLayout = new QVBoxLayout(this);

        auto* controlLayout = new QHBoxLayout();

        auto* get_hwnd_button = new QPushButton("获取句柄");
        auto* execute_button = new QPushButton("执行命令");
        auto* stop_button = new QPushButton("停止命令");
        auto* clear_button = new QPushButton("清空输出");

        connect(get_hwnd_button, &QPushButton::clicked, this, &MainWindow::get_hwnd);
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

        clickHandler = [this](QMouseEvent* event) {
            this->handleMouseClick(event);
        };
    }

    ~MainWindow() override {}

private slots:
    void get_hwnd() {
        output_text->append("请用鼠标点击游戏窗口");
        setMouseTracking(true);
    }

    void run_command(const QString& command) {
        if (state.currentThread && state.currentThread->isRunning()) {
            state.currentThread->quit();
            state.currentThread->wait();
        }

        state.currentThread = new QThread(this);
        std::function<void()> func = command_options[command];
        QObject::connect(state.currentThread, &QThread::started, [func]() {
            try {
                func();
                state.output_text->append("运行完成");
            } catch (const std::exception& e) {
                state.output_text->append(QString("出错了: %1").arg(e.what()));
            }
        });

        QObject::connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
        state.currentThread->start();

        output_text->append(QString("开始执行命令(%1): %2").arg(state.hwnd? "后台" : "前台").arg(command));
    }

    void stop_command() {
        if (!state.currentThread ||!state.currentThread->isRunning()) {
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

        auto* layout = new QVBoxLayout(&selectDialog);

        auto* commandLayout = new QGridLayout();
        int row = 0, col = 0;
        for (const QString& command : command_options.keys()) {
            auto* btn = new QPushButton(command);
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
        }

        layout->addLayout(commandLayout);

        if (selectDialog.exec() == QDialog::Accepted) {
            // 对话框接受后执行相应操作
        }
    }

    void clear_text() {
        output_text->clear();
    }

};

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}

#include "main.moc"