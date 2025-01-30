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
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QDateTime>
#include <QTextEdit>
#include <QJsonArray>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>


#include "state.h"
#include "flow/cv.h"
#include "flow/emitter.h"
#include "task/daily.h"
#include "task/battle.h"
#include "function/clicker.h"

auto configFile = "config.json";

float getScale() {
    MONITORINFOEX info = {};
    POINT ptZero = {0, 0};
    info.cbSize = sizeof(info);
    GetMonitorInfo(MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY), &info);
    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    return static_cast<float>(devMode.dmPelsWidth) / (info.rcMonitor.right - info.rcMonitor.left);
}

class ButtonWithSetting : public QWidget {
Q_OBJECT
public:
    explicit ButtonWithSetting(
            const QString &text,
            bool showSettingButton = true,
            QWidget *parent = nullptr
    ) : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        textButton = new QPushButton(text);
        textButton->setFixedHeight(25);
        textButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        layout->addWidget(textButton);

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
        layout->setContentsMargins(0, 0, 0, 0);

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

    [[nodiscard]] QSpinBox *getSpinBox() const { return spinBox; }

private:
    QLabel *label;
    QSpinBox *spinBox;
};

class LabeledComboBox : public QWidget {
public:
    LabeledComboBox(const QString &text, const QJsonArray &options, const QString &initialValue = "") {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        label = new QLabel(text + ":");
        comboBox = new QComboBox();
        comboBox->setFixedWidth(100);

        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        for (const auto &option: options) {
            comboBox->addItem(option.toString());
        }

        if (!initialValue.isEmpty()) {
            comboBox->setCurrentText(initialValue);
        }

        layout->addWidget(label);
        layout->addWidget(comboBox);
        layout->addStretch(1);
    }

    QComboBox *getComboBox() {
        return comboBox;
    }

private:
    QLabel *label;
    QComboBox *comboBox;
};

class ImageDialog : public QDialog {
Q_OBJECT

public:
    explicit ImageDialog(const QImage &image, QWidget *parent) : QDialog(parent) {
        this->setWindowTitle("查看截屏");
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

        int maxWidth = 400;
        int maxHeight = 400;

        auto *layout = new QVBoxLayout(this);
        auto imageLabel = new QLabel(this);
        imageLabel->setMaximumSize(maxWidth, maxHeight);

        layout->addWidget(imageLabel);

        int originalWidth = image.width();
        int originalHeight = image.height();

        float scaleWidth = static_cast<float>(maxWidth) / originalWidth;
        float scaleHeight = static_cast<float>(maxHeight) / originalHeight;
        float scale = qMin(scaleWidth, scaleHeight); // 取较小的缩放比例，保证图像不会被拉伸超出边界
        int scaledWidth = static_cast<int>(originalWidth * scale);
        int scaledHeight = static_cast<int>(originalHeight * scale);

        auto scaledImage = image.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    };
};

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("红警自动");
    resize(480, 320);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    auto *mainLayout = new QVBoxLayout(this);

    auto *controlLayout = new QHBoxLayout();

    auto *captureButton = new QPushButton("获取窗口");
    auto *executeButton = new QPushButton("执行命令");
    auto *stopButton = new QPushButton("停止命令");
    auto *clearButton = new QPushButton("清空输出");

    connect(captureButton, &QPushButton::clicked, this, &MainWindow::startCapture);
    connect(executeButton, &QPushButton::clicked, this, &MainWindow::selectCommand);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopCommand);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearText);

    controlLayout->addWidget(captureButton);
    controlLayout->addWidget(executeButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(clearButton);

    mainLayout->addLayout(controlLayout);

    outputText = new QTextEdit();
    outputText->setReadOnly(true);
    mainLayout->addWidget(outputText);

    tasks["军备合成"] = armsCompound;
    tasks["剿灭将领"] = exterminateEnemy;
    tasks["国家争霸"] = countryArena;
    tasks["世界争霸"] = worldArena;
    tasks["国家战争"] = countryWar;
    commandBattle = QStringList({"国家争霸", "世界争霸", "剿灭将领", "军备合成", "国家战争"});

    tasks["英雄中心"] = heroCenter;
    tasks["战争学院"] = warCenter;
    tasks["国家宝箱"] = countryChest;
    tasks["公会领奖"] = guild;
    tasks["将领抽奖"] = admiral;
    tasks["参谋抽奖"] = adviser;
    tasks["火炮抽奖"] = mortar;
    tasks["配件抽奖"] = equipment;
    tasks["军备抽奖"] = arms;
    tasks["每日任务"] = dailyTask;
    tasks["周任务"] = weeklyTask;
    tasks["每日签到"] = signIn;
    tasks["矿区争夺"] = oreField;
    tasks["月卡领取"] = monthlyCard;
    tasks["公会建筑"] = guildBuilding;
    commandDaily = QStringList({
                                       "英雄中心", "战争学院", "国家宝箱", "公会领奖", "将领抽奖", "参谋抽奖",
                                       "火炮抽奖", "配件抽奖", "军备抽奖", "每日任务", "周任务", "每日签到",
                                       "矿区争夺", "月卡领取", "公会建筑"
                               });

    hook = nullptr;
    isWaiting = false;
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    onLogText("使用方法: 先获取窗口，再执行命令", "blue");

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

    if (hook) UnhookWindowsHookEx(hook);

    QWidget::closeEvent(event);
}

void MainWindow::onLogText(const QString &text, const QString &color) const {
    outputText->append(QString("<div style=\"color:%1;\">%2</div>").arg(color, text));
}

void MainWindow::onLogMessage(const QString &text, const QString &color) {
    if (state.stopFlag.load()) return;

    if (previousLog == text) return;

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTime currentTime = currentDateTime.time();
    QString timeString = currentTime.toString("hh:mm:ss");

    outputText->append(QString(
            R"(
                <div>
                <span style="color:white;background-color:green;">&nbsp;%1&nbsp;</span>
                <span style="color:%2;">%3</span>
                <img src=":/resources/ui/transparent.png" height='14' width='1'>
                </div>
                )"
    ).arg(timeString, color, text));

    previousLog = text;
}

void MainWindow::startCapture() {
    onLogText("请用鼠标点击游戏窗口");

    isWaiting = true;
    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);

    if (hook == nullptr) onLogText("Failed to set hook");
}

void MainWindow::selectCommand() {
    QDialog selectDialog(this);
    selectDialog.setWindowTitle("选择命令");
    selectDialog.setWindowFlags(selectDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

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
    auto *dailyGroupBox = new QGroupBox("日常任务", &selectDialog);
    auto *autoRunBox = new QGroupBox("一键执行", &selectDialog);
    auto *otherFunctionBox = new QGroupBox("其它功能", &selectDialog);

    battleGroupBox->setStyleSheet(style);
    dailyGroupBox->setStyleSheet(style);
    autoRunBox->setStyleSheet(style);
    otherFunctionBox->setStyleSheet(style);

    auto *battleLayout = new QGridLayout(battleGroupBox);
    auto *dailyLayout = new QGridLayout(dailyGroupBox);
    auto *autoRunLayout = new QGridLayout(autoRunBox);
    auto *otherFunctionLayout = new QGridLayout(otherFunctionBox);

    battleLayout->setSpacing(5);
    dailyLayout->setSpacing(5);
    autoRunLayout->setSpacing(5);
    otherFunctionLayout->setSpacing(5);

    for (int i = 0; i < 3; ++i) {
        battleLayout->setColumnMinimumWidth(i, 100);
        dailyLayout->setColumnMinimumWidth(i, 100);
        autoRunLayout->setColumnMinimumWidth(i, 100);
        otherFunctionLayout->setColumnMinimumWidth(i, 100);

        battleLayout->setColumnStretch(i, 1);
        dailyLayout->setColumnStretch(i, 1);
        autoRunLayout->setColumnStretch(i, 1);
        otherFunctionLayout->setColumnStretch(i, 1);
    }

    int row, col;

    row = col = 0;
    for (const auto &command: commandBattle) {
        auto *customBtn = new ButtonWithSetting(command, state.config.contains(command));
        customBtn->getTextButton()->setAutoDefault(false);
        customBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(customBtn->getTextButton(), &QPushButton::clicked, [this, command, &selectDialog]() {
            selectDialog.accept();
            runCommand(command);
        });
        if (customBtn->getSettingButton()) {
            connect(customBtn->getSettingButton(), &QToolButton::clicked, [this, command]() {
                setCommand(command);
            });
        }

        battleLayout->addWidget(customBtn, row, col);

        col += 1;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }

    row = col = 0;
    for (const auto &command: commandDaily) {
        auto *customBtn = new ButtonWithSetting(command, state.config.contains(command));
        customBtn->getTextButton()->setAutoDefault(false);
        customBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(customBtn->getTextButton(), &QPushButton::clicked, [this, command, &selectDialog]() {
            selectDialog.accept();
            runCommand(command);
        });

        if (customBtn->getSettingButton()) {
            connect(customBtn->getSettingButton(), &QToolButton::clicked, [this, command]() {
                setCommand(command);
            });
        }

        dailyLayout->addWidget(customBtn, row, col);

        col += 1;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }

    row = col = 0;
    for (int i = 1; i <= 3; ++i) {
        auto command = "预设" + QString::number(i);
        auto *customBtn = new ButtonWithSetting(command, state.config.contains(command));
        customBtn->getTextButton()->setAutoDefault(false);
        customBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(customBtn->getTextButton(), &QToolButton::clicked, [this, command, &selectDialog]() {
            selectDialog.accept();
            batchRunCommand(command);
        });

        if (customBtn->getSettingButton()) {
            connect(customBtn->getSettingButton(), &QToolButton::clicked, [this, command]() {
                setCommand(command);
            });
        }

        autoRunLayout->addWidget(customBtn, row, col);

        col += 1;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }

    auto *viewBtn = new ButtonWithSetting("查看截屏", false);
    auto *clickerBtn = new ButtonWithSetting("连点器", false);

    viewBtn->getTextButton()->setAutoDefault(false);
    clickerBtn->getTextButton()->setAutoDefault(false);

    viewBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    clickerBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(viewBtn->getTextButton(), &QToolButton::clicked, [this, &selectDialog]() {
        if (!state.hwnd) {
            onLogText("请先获取游戏窗口!");
            selectDialog.accept();
            return;
        }

        auto screenshot = CV::getScreen(Mode::RGB);
        cv::cvtColor(screenshot, screenshot, cv::COLOR_BGR2RGB);

        auto *dialog = new ImageDialog(QImage(
                screenshot.data,
                screenshot.cols,
                screenshot.rows,
                static_cast<int>(screenshot.step),
                QImage::Format_RGB888
        ), this);
        dialog->exec();
    });

    connect(clickerBtn->getTextButton(), &QToolButton::clicked, [this, &selectDialog]() {
        if (!state.hwnd) {
            onLogText("请先获取游戏窗口!");
            selectDialog.accept();
            return;
        }

        auto *dialog = new ClickerDialog(this);
        selectDialog.accept();
        this->showMinimized();
        dialog->exec();
    });


    otherFunctionLayout->addWidget(viewBtn, 0, 0);
    otherFunctionLayout->addWidget(clickerBtn, 0, 1);

    layout->addWidget(battleGroupBox);
    layout->addWidget(dailyGroupBox);
    layout->addWidget(autoRunBox);
    layout->addWidget(otherFunctionBox);

    selectDialog.exec();
}

void MainWindow::stopCommand() {
    if (!state.currentThread || !state.currentThread->isRunning()) {
        onLogText("当前无命令正在执行");
        return;
    }

    state.stopFlag.store(true);
    state.currentThread->quit();
    state.currentThread->wait();
    onLogText("命令已停止执行");
}

void MainWindow::clearText() {
    outputText->clear();
}

void MainWindow::runCommand(const QString &command) {
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

void MainWindow::batchRunCommand(const QString &command) {
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

    QObject::connect(state.currentThread, &QThread::started, [this, &command]() {
        try {
            emit logMessage("开始一键执行", "red");

            auto checkboxArray = state.config[command].toObject()["checkbox"].toArray();

            std::vector<QJsonObject> checkboxes;
            for (const auto &item: checkboxArray) {
                checkboxes.push_back(item.toObject());
            }

            std::sort(checkboxes.begin(), checkboxes.end(), [](const QJsonObject &a, const QJsonObject &b) {
                return a["order"].toInt() < b["order"].toInt();
            });

            for (const auto &checkbox: checkboxes) {
                if (state.stopFlag.load()) break;

                if (checkbox["value"].toBool()) {
                    auto subCommand = checkbox["text"].toString();

                    try {
                        emit logMessage("开始运行: " + subCommand, "blue");
                        tasks[subCommand]();
                        emit logMessage("运行完成: " + subCommand, "blue");
                    } catch (const std::exception &e) {
                        if (!state.stopFlag.load()) {
                            emit logMessage("出错了: " + QString::fromStdString(e.what()), "red");
                            emit logMessage(subCommand + "运行错误", "red");
                        } else {
                            emit logMessage("运行完成: " + subCommand, "red");
                        }

                        throw e;
                    }
                }
            }

            emit logMessage("一键执行完成", "red");

            if (state.currentThread) {
                state.currentThread->quit();
                state.currentThread = nullptr;
            }
        } catch (const std::exception &e) {
            if (!state.stopFlag.load()) {
                emit logMessage("出错了: " + QString(e.what()), "red");
                emit logMessage("一键执行错误", "red");
            } else {
                emit logMessage("一键执行完成", "red");
            }

            if (state.currentThread) {
                state.currentThread->quit();
                state.currentThread = nullptr;
            }
        }
    });

    QObject::connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
    state.currentThread->start();
}

void MainWindow::setCommand(const QString &command) {
    QDialog settingDialog(this);
    settingDialog.setWindowTitle("设置 " + command);
    settingDialog.setMinimumWidth(300);
    settingDialog.setWindowFlags(settingDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QJsonObject commandConfig = state.config[command].toObject();

    auto *mainLayout = new QVBoxLayout(&settingDialog);
    mainLayout->setSpacing(15);

    auto *checkboxLayout = new QGridLayout();
    checkboxLayout->setSpacing(5);

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
                if (std::get<0>(item) == order) {
                    std::get<2>(item) = checked;
                    break;
                }
            }
        });
    }

    mainLayout->addLayout(checkboxLayout);

    auto *inputLayout = new QGridLayout();
    inputLayout->setSpacing(5);

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

    std::sort(inputItems.begin(), inputItems.end(), [](const auto &a, const auto &b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    for (const auto &item: inputItems) {
        int order = std::get<0>(item);
        QString text = std::get<1>(item);
        int value = std::get<2>(item);

        auto *labeledSpinBox = new LabeledSpinBox(text, value);

        inputLayout->addWidget(labeledSpinBox, std::floor(order / 2), order % 3);

        connect(labeledSpinBox->getSpinBox(), QOverload<int>::of(&QSpinBox::valueChanged),
                [&inputItems, order, text](int newValue) {
                    for (auto &item: inputItems) {
                        if (std::get<0>(item) == order) {
                            std::get<2>(item) = newValue;
                            break;
                        }
                    }
                });
    }

    mainLayout->addLayout(inputLayout);

    auto *comboBoxLayout = new QGridLayout();
    comboBoxLayout->setSpacing(5);

    QJsonArray selectArray = commandConfig["select"].toArray();
    std::vector<std::tuple<int, QString, QString, QJsonArray>> selectItems;

    for (const auto &select: selectArray) {
        QJsonObject selectObj = select.toObject();
        selectItems.emplace_back(
                selectObj["order"].toInt(),
                selectObj["text"].toString(),
                selectObj["value"].toString(),
                selectObj["options"].toArray()
        );
    }

    std::sort(selectItems.begin(), selectItems.end(), [](const auto &a, const auto &b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    for (const auto &item: selectItems) {
        int order = std::get<0>(item);
        QString text = std::get<1>(item);
        QString value = std::get<2>(item);
        QJsonArray options = std::get<3>(item);

        auto *labeledComboBox = new LabeledComboBox(text, options, value);

        comboBoxLayout->addWidget(labeledComboBox, std::floor(order / 2), order % 2);

        connect(labeledComboBox->getComboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged),
                [&selectItems, order, text](int index) {
                    for (auto &item: selectItems) {
                        if (std::get<0>(item) == order) {
                            std::get<2>(item) = std::get<3>(item)[index].toString();
                            break;
                        }
                    }
                });
    }

    mainLayout->addLayout(comboBoxLayout);

    QString tips = commandConfig["tips"].toString();
    auto *tipsLabel = new QLabel(tips);
    tipsLabel->setWordWrap(true);
    mainLayout->addWidget(tipsLabel);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    auto *saveButton = new QPushButton("确定");
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonLayout);

    connect(
            saveButton,
            &QPushButton::clicked,
            [this, command, &checkboxItems, &inputItems, &settingDialog, &selectItems, commandConfig]() {

                QJsonArray newCheckboxArray;
                for (const auto &item: checkboxItems) {
                    newCheckboxArray.append(QJsonObject{
                            {"order", std::get<0>(item)},
                            {"text",  std::get<1>(item)},
                            {"value", std::get<2>(item)}
                    });
                }

                QJsonArray newInputArray;
                for (const auto &item: inputItems) {
                    newInputArray.append(QJsonObject{
                            {"order", std::get<0>(item)},
                            {"text",  std::get<1>(item)},
                            {"value", std::get<2>(item)}
                    });
                }

                QJsonArray newSelectArray;
                for (const auto &item: selectItems) {
                    newSelectArray.append(QJsonObject{
                            {"order",   std::get<0>(item)},
                            {"text",    std::get<1>(item)},
                            {"value",   std::get<2>(item)},
                            {"options", std::get<3>(item)}
                    });
                }

                QJsonObject newCommandConfig;
                newCommandConfig["checkbox"] = newCheckboxArray;
                newCommandConfig["input"] = newInputArray;
                newCommandConfig["select"] = newSelectArray;
                newCommandConfig["tips"] = commandConfig["tips"].toString();

                state.config[command] = newCommandConfig;

                QFile file("config.json");
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QJsonDocument doc(state.config);
                    file.write(doc.toJson());
                    file.close();
                    onLogText("配置保存成功", "blue");
                } else {
                    onLogText("配置保存失败");
                }

                settingDialog.accept();
            }
    );

    settingDialog.exec();
}

LRESULT CALLBACK MainWindow::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_LBUTTONDOWN) {
            auto *window = qobject_cast<MainWindow *>(qApp->activeWindow());
            if (window && window->isWaiting) {
                auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
                HWND hwnd = WindowFromPoint(pMouseStruct->pt);
                wchar_t buffer[256] = {0};

                GetWindowTextW(hwnd, buffer, 256);
                if (hwnd) {
                    window->onLogText(
                            QString("获取到窗口: ") + QString::fromWCharArray(buffer) + "(0x" +
                            QString::number(reinterpret_cast<qulonglong>(hwnd), 16) + ")", "blue"
                    );
                    window->onLogText("请不要最小化游戏窗口！但可以放在其它窗口后面", "red");
                    window->isWaiting = false;
                    state.hwnd = hwnd;
                    UnhookWindowsHookEx(window->hook);
                    window->hook = nullptr;
                } else {
                    window->onLogText("获取窗口句柄失败", "red");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

int main(int argc, char *argv[]) {
    state.scale = getScale();

    qRegisterMetaType<QTextCursor>("QTextCursor");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return QApplication::exec();
}

#include "main.moc"