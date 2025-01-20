#include "main.h"
#include <Windows.h>
#include <functional>

#include <QIcon>
#include <QFile>
#include <QLabel>
#include <QDialog>
#include <QThread>
#include <QWidget>
#include <QScreen>
#include <QSpinBox>
#include <QCheckBox>
#include <QDateTime>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QGroupBox>


#include "state.h"
#include "task/battle.h"
#include "task/daily.h"
#include "flow/emitter.h"

auto configFile = "config.json";

float getScale() {
    MONITORINFOEX info = {};
    POINT ptZero = {0, 0};
    info.cbSize = sizeof(info);
    GetMonitorInfo(MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY), &info);
    DEVMODE devmode = {};
    devmode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
    return static_cast<float>(devmode.dmPelsWidth) / (info.rcMonitor.right - info.rcMonitor.left);
}


class ButtonWithSetting : public QWidget {
Q_OBJECT
public:
    explicit ButtonWithSetting(const QString &text, bool showSettingButton = true, QWidget *parent = nullptr)
            : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        // 创建文字按钮
        textButton = new QPushButton(text);
        textButton->setFixedHeight(25);
        textButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        layout->addWidget(textButton);

        // 根据参数决定是否创建和添加设置按钮
        if (showSettingButton) {
            settingButton = new QToolButton();
            settingButton->setIcon(QIcon(":/resources/ui/setting.png"));
            settingButton->setIconSize(QSize(16, 16));
            settingButton->setFixedHeight(25);
            settingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            layout->addWidget(settingButton);
        } else {
            settingButton = nullptr;
        }

        // 设置布局
        setLayout(layout);
    }

    [[nodiscard]] QPushButton *getTextButton() const { return textButton; }

    [[nodiscard]] QToolButton *getSettingButton() const { return settingButton; }

private:
    QPushButton *textButton;
    QToolButton *settingButton;
};


class LabeledSpinBox : public QWidget {
public:
    LabeledSpinBox(const QString &text, int value, QWidget *parent = nullptr)
            : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);  // 移除外边距

        label = new QLabel(text + ":");
        spinBox = new QSpinBox();
        spinBox->setValue(value);
        spinBox->setMinimum(1);
        spinBox->setFixedWidth(100);

        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        layout->addWidget(label);
        layout->addWidget(spinBox);
        layout->addStretch(1);
    }

    [[nodiscard]] QSpinBox *getSpinBox() const {
        return spinBox;
    }

private:
    QLabel *label;
    QSpinBox *spinBox;
};

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("红警自动");
    resize(480, 320);

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

    tasks["军备合成"] = arms_compound;
    tasks["剿灭将领"] = exterminate_enemy;
    tasks["国家争霸"] = country_arena;
    tasks["世界争霸"] = world_arena;
    tasks["国家战争"] = country_war;
    command_battle = QStringList({"国家争霸", "世界争霸", "剿灭将领", "军备合成", "国家战争"});

    tasks["英雄中心"] = hero_center;
    tasks["战争学院"] = war_center;
    tasks["国家宝箱"] = country_chest;
    tasks["公会领奖"] = guild;
    tasks["将领抽奖"] = admiral;
    tasks["参谋抽奖"] = adviser;
    tasks["火炮抽奖"] = mortar;
    tasks["配件抽奖"] = equipment;
    tasks["军备抽奖"] = arms;
    tasks["公会建筑"] = guild_building;
    command_daily = QStringList({
                                        "英雄中心", "战争学院", "国家宝箱", "公会领奖", "将领抽奖", "参谋抽奖",
                                        "火炮抽奖", "配件抽奖", "军备抽奖", "公会建筑",
                                });

    // 初始时不安装钩子
    hook = nullptr;
    isWaitingForHwnd = false;
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    onLogText("使用方法: 先获取窗口，再执行命令", "blue");

    // 连接日志信号和槽函数
    connect(this, &MainWindow::logMessage, this, &MainWindow::onLogMessage);
    connect(Emitter::instance(), &Emitter::log, this, &MainWindow::onLogMessage);


    onLogText(QString::fromStdString("当前缩放率: ") + QString::number(state.scale));

    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        onLogText("无法打开配置文件: config.json");
    } else {
        auto jsonData = file.readAll();
        auto doc = QJsonDocument::fromJson(jsonData);
        state.config = doc.object();
        file.close();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (state.currentThread && state.currentThread->isRunning()) {
        state.stopFlag.store(true);
        state.currentThread->quit();
        state.currentThread->wait();
    }
    // 移除鼠标钩子
    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    QWidget::closeEvent(event);
}

void MainWindow::onLogText(const QString &text, const QString &color) const {
    // 创建HTML格式的字符串，其中包含了颜色信息
    QString html = QString("<div style=\"color:%1;\">%2</div>").arg(color, text);

    // 将HTML字符串追加到QTextEdit中
    output_text->append(html);
}

void MainWindow::onLogMessage(const QString &text, const QString &color) const {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (state.stopFlag.load()) {
        return;
    }

    // 提取时间部分
    QTime currentTime = currentDateTime.time();

    // 格式化时间为字符串 "HH:MM:SS"
    QString timeString = currentTime.toString("hh:mm:ss");

    QString html = QString(
            R"(
                <div>
                <span style="color:white;background-color:green;">&nbsp;%1&nbsp;</span>
                <span style="color:%2;">%3</span>
                <img src=":/resources/ui/transparent.png" height='14'>
                </div>
                )"
    ).arg(timeString, color, text);

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

    auto func = tasks[command];
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
                emit logMessage("运行结束", "red");
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

    auto style = R"(
        QGroupBox {
            border: 1px solid #888;
            border-radius: 5px; /* 设置圆角 */
            padding-top: 10px; /* 设置顶部内边距 */
        }
        QGroupBox::title {
            border: 1px solid #888;
            border-radius: 5px 1px 1px 1px; /* 设置圆角 */
            padding: 2px;
        }
    )";

    auto *battleGroupBox = new QGroupBox("自动战斗", &selectDialog);

    battleGroupBox->setStyleSheet(style);

    auto *battleLayout = new QGridLayout(battleGroupBox);
    battleLayout->setSpacing(5);  // 设置布局间距为10

    // 设置每列的最小宽度
    for (int i = 0; i < 3; ++i) {
        battleLayout->setColumnMinimumWidth(i, 100);  // 设置每列的最小宽度为100
        battleLayout->setColumnStretch(i, 1);  // 设置每列的伸缩比例为1
    }

    int row = 0;
    int col = 0;

    // 确保命令选项的顺序
    for (const auto &command: command_battle) {
        auto *customBtn = new ButtonWithSetting(command, state.config.contains(command));

        // 连接文字按钮的点击事件
        connect(customBtn->getTextButton(), &QPushButton::clicked, [this, command, &selectDialog]() {
            selectDialog.accept();
            run_command(command);
        });

        // 连接设置按钮的点击事件
        connect(customBtn->getSettingButton(), &QToolButton::clicked, [this, command]() {
            set_command(command);
        });

        // 取消按钮的默认选中状态
        customBtn->getTextButton()->setAutoDefault(false);

        // 设置 ButtonWithSetting 的大小策略
        customBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        battleLayout->addWidget(customBtn, row, col);

        col += 1;
        if (col >= 3) {  // 每行最多3个按钮组
            col = 0;
            row++;
        }
    }

    auto *dailyGroupBox = new QGroupBox("日常任务", &selectDialog);

    dailyGroupBox->setStyleSheet(style);

    auto *dailyLayout = new QGridLayout(dailyGroupBox);
    dailyLayout->setSpacing(5);  // 设置布局间距为10

    // 设置每列的最小宽度
    for (int i = 0; i < 3; ++i) {
        dailyLayout->setColumnMinimumWidth(i, 100);  // 设置每列的最小宽度为100
        dailyLayout->setColumnStretch(i, 1);  // 设置每列的伸缩比例为1
    }

    row = 0;
    col = 0;

    // 确保命令选项的顺序
    for (const auto &command: command_daily) {
        auto *customBtn = new ButtonWithSetting(command, state.config.contains(command));

        // 连接文字按钮的点击事件
        connect(customBtn->getTextButton(), &QPushButton::clicked, [this, command, &selectDialog]() {
            selectDialog.accept();
            run_command(command);
        });

        // 连接设置按钮的点击事件
        connect(customBtn->getSettingButton(), &QToolButton::clicked, [this, command]() {
            set_command(command);
        });

        // 取消按钮的默认选中状态
        customBtn->getTextButton()->setAutoDefault(false);

        // 设置 ButtonWithSetting 的大小策略
        customBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        dailyLayout->addWidget(customBtn, row, col);

        col += 1;
        if (col >= 3) {  // 每行最多3个按钮组
            col = 0;
            row++;
        }
    }

    layout->addWidget(battleGroupBox);
    layout->addWidget(dailyGroupBox);

    if (selectDialog.exec() == QDialog::Accepted) {
        //        onLogText("开始运行", "red");
    }
}

void MainWindow::set_command(const QString &command) {
    QDialog settingDialog(this);
    settingDialog.setWindowTitle("设置 " + command);
    settingDialog.setMinimumWidth(300);

    // 获取命令对应的配置
    QJsonObject commandConfig = state.config[command].toObject();

    // 创建主布局
    auto *mainLayout = new QVBoxLayout(&settingDialog);

    // 创建复选框布局
    auto *checkboxLayout = new QGridLayout();
    checkboxLayout->setSpacing(5);

    // 获取复选框配置
    QJsonArray checkboxArray = commandConfig["checkbox"].toArray();
    std::vector<std::tuple<int, QString, bool>> checkboxItems;

    for (const auto &checkbox: checkboxArray) {
        QJsonObject checkboxObj = checkbox.toObject();
        checkboxItems.emplace_back(
                checkboxObj["order"].toInt(),
                checkboxObj["text"].toString(),
                checkboxObj["value"].toBool()
        );
    }

    // 按 order 排序
    std::sort(checkboxItems.begin(), checkboxItems.end(), [](const auto &a, const auto &b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    for (const auto &item: checkboxItems) {
        int order = std::get<0>(item);
        QString text = std::get<1>(item);
        bool value = std::get<2>(item);

        auto *checkBox = new QCheckBox(text);
        checkBox->setChecked(value);

        checkboxLayout->addWidget(checkBox, std::floor(order / 3), order % 3);

        connect(checkBox, &QCheckBox::toggled, [&checkboxItems, order, text](bool checked) {
            for (auto &item: checkboxItems) {
                if (std::get<0>(item) == order && std::get<1>(item) == text) {
                    std::get<2>(item) = checked;
                    break;
                }
            }
        });
    }

    mainLayout->addLayout(checkboxLayout);

    // 创建输入框布局
    auto *inputLayout = new QGridLayout();
    inputLayout->setSpacing(10);

    // 获取输入框配置
    QJsonArray inputArray = commandConfig["input"].toArray();
    std::vector<std::tuple<int, QString, int>> inputItems;

    for (const auto &input: inputArray) {
        QJsonObject inputObj = input.toObject();
        inputItems.emplace_back(
                inputObj["order"].toInt(),
                inputObj["text"].toString(),
                inputObj["value"].toInt()
        );
    }

    // 按 order 排序
    std::sort(inputItems.begin(), inputItems.end(), [](const auto &a, const auto &b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    for (const auto &item: inputItems) {
        int order = std::get<0>(item);
        QString text = std::get<1>(item);
        int value = std::get<2>(item);

        auto *labeledSpinBox = new LabeledSpinBox(text, value);

        inputLayout->addWidget(labeledSpinBox, std::floor(order / 2), 2 * (order % 3), 1, 2);

        connect(labeledSpinBox->getSpinBox(), QOverload<int>::of(&QSpinBox::valueChanged),
                [this, &inputItems, order, text](int newValue) {
                    for (auto &item: inputItems) {
                        if (std::get<0>(item) == order && std::get<1>(item) == text) {
                            std::get<2>(item) = newValue;
                            break;
                        }
                    }
                });
    }

    mainLayout->addLayout(inputLayout);

    // 添加提示文本
    QString tips = commandConfig["tips"].toString();
    auto *tipsLabel = new QLabel(tips);
    tipsLabel->setWordWrap(true);
    mainLayout->addWidget(tipsLabel);

    // 创建保存按钮布局
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();  // 添加伸缩空间，将按钮对齐到右侧
    auto *saveButton = new QPushButton("确定");
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonLayout);

    // 连接保存按钮的点击事件
    connect(
            saveButton,
            &QPushButton::clicked,
            [this, command, &checkboxItems, &inputItems, &settingDialog, commandConfig]() {

                QJsonArray newCheckboxArray;
                for (const auto &item: checkboxItems) {
                    int order = std::get<0>(item);
                    QString text = std::get<1>(item);
                    bool value = std::get<2>(item);
                    QJsonObject obj = {{"text",  text},
                                       {"value", value},
                                       {"order", order}};
                    newCheckboxArray.append(obj);
                }

                QJsonArray newInputArray;
                for (const auto &item: inputItems) {
                    int order = std::get<0>(item);
                    QString text = std::get<1>(item);
                    int value = std::get<2>(item);
                    QJsonObject obj = {{"text",  text},
                                       {"value", value},
                                       {"order", order}};
                    newInputArray.append(obj);
                }

                QJsonObject newCommandConfig;
                newCommandConfig["checkbox"] = newCheckboxArray;
                newCommandConfig["input"] = newInputArray;
                newCommandConfig["tips"] = commandConfig["tips"].toString();

                state.config[command] = newCommandConfig;

                // 将 config 写回 config.json 文件
                QFile file("config.json");
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QJsonDocument doc(state.config);
                    file.write(doc.toJson());
                    file.close();
                    onLogText("配置保存成功", "blue");
                } else {
                    onLogText("配置保存失败");
                }

                // 关闭对话框
                settingDialog.accept();
            }
    );

    settingDialog.exec();
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
    state.scale = getScale();

    qRegisterMetaType<QTextCursor>("QTextCursor");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();/**/
    return QApplication::exec();
}

#include "main.moc"