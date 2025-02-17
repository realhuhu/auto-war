#include "panelWidget.h"

auto configFile = "/config.json";

PanelWidget::PanelWidget(QWidget *parent) : QWidget(parent) {
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

    connect(captureButton, &QPushButton::clicked, this, &PanelWidget::startCapture);
    connect(executeButton, &QPushButton::clicked, this, &PanelWidget::selectCommand);
    connect(stopButton, &QPushButton::clicked, this, &PanelWidget::stopCommand);
    connect(clearButton, &QPushButton::clicked, this, &PanelWidget::clearText);

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
    tasks["公会战役"] = guildWar;
    tasks["国家战争"] = countryWar;
    commandBattle = QStringList({"国家争霸", "世界争霸", "剿灭将领", "军备合成", "公会战役", "国家战争"});

    tasks["英雄中心"] = heroCenter;
    tasks["战争学院"] = warCenter;
    tasks["国家宝箱"] = countryChest;
    tasks["将领抽奖"] = admiral;
    tasks["参谋抽奖"] = adviser;
    tasks["火炮抽奖"] = mortar;
    tasks["配件抽奖"] = equipment;
    tasks["军备抽奖"] = arms;
    tasks["每日签到"] = signIn;
    tasks["矿区争夺"] = oreField;
    tasks["月卡领取"] = monthlyCard;
    tasks["其它活动"] = otherActivity;
    tasks["每日任务"] = dailyTask;
    tasks["周任务"] = weeklyTask;
    tasks["公会领奖"] = guild;
    tasks["公会建筑"] = guildBuilding;
    commandDaily = QStringList({
                                       "英雄中心", "战争学院", "国家宝箱", "将领抽奖",
                                       "参谋抽奖", "火炮抽奖", "配件抽奖", "军备抽奖",
                                       "每日签到", "矿区争夺", "月卡领取", "其它活动",
                                       "每日任务", "周任务", "公会领奖", "公会建筑"
                               });

    tasks["开卡国战"] = loopCountryWar;
    commandSpecial = QStringList({"开卡国战"});

    hook = nullptr;
    isWaiting = false;
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    log("使用方法: 先获取窗口，再执行命令", "blue");

    connect(this, &PanelWidget::logMessage, this, &PanelWidget::onLogMessage);
    connect(Emitter::instance(), &Emitter::log, this, &PanelWidget::onLogMessage);


    log(QString("当前缩放率:%1").arg(QString::number(state.scale)));
    log("QQ游戏支持任意缩放率，其他客户端(如浏览器、360游戏)可能需要调整为100%缩放再打开辅助!");

    QFile file(QCoreApplication::applicationDirPath() + configFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log("无法打开配置文件: config.json");
    } else {
        auto jsonData = file.readAll();
        auto doc = QJsonDocument::fromJson(jsonData);
        state.config = doc.object();
        file.close();
    }
}

void PanelWidget::closeEvent(QCloseEvent *event) {
    if (hook) UnhookWindowsHookEx(hook);

    if (state.currentThread) {
        state.stopFlag.store(true);
        state.currentThread->quit();
        state.currentThread->wait();
    }

    QWidget::closeEvent(event);
}

void PanelWidget::log(const QString &text, const QString &color) const {
    outputText->append(QString("<div style=\"color:%1;\">%2</div>").arg(color, text));
}

void PanelWidget::onLogMessage(const QString &text, const QString &color) {
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
                <img src=":/resource/ui/transparent.png" height='14' width='1'>
                </div>
                )"
    ).arg(timeString, color, text));

    previousLog = text;
}

void PanelWidget::startCapture() {
    log("请用鼠标点击游戏窗口");

    isWaiting = true;
    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, nullptr, 0);

    if (hook == nullptr) log("Failed to set hook");
}

void PanelWidget::selectCommand() {
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
    auto *autoRunGroupBox = new QGroupBox("一键执行", &selectDialog);
    auto *specialGroupBox = new QGroupBox("特殊功能", &selectDialog);
    auto *otherFunctionGroupBox = new QGroupBox("其它", &selectDialog);

    battleGroupBox->setStyleSheet(style);
    dailyGroupBox->setStyleSheet(style);
    autoRunGroupBox->setStyleSheet(style);
    specialGroupBox->setStyleSheet(style);
    otherFunctionGroupBox->setStyleSheet(style);

    auto *battleLayout = new QGridLayout(battleGroupBox);
    auto *dailyLayout = new QGridLayout(dailyGroupBox);
    auto *autoRunLayout = new QGridLayout(autoRunGroupBox);
    auto *specialLayout = new QGridLayout(specialGroupBox);
    auto *otherFunctionLayout = new QGridLayout(otherFunctionGroupBox);

    battleLayout->setSpacing(5);
    dailyLayout->setSpacing(5);
    autoRunLayout->setSpacing(5);
    specialLayout->setSpacing(5);
    otherFunctionLayout->setSpacing(5);

    for (int i = 0; i < 4; ++i) {
        battleLayout->setColumnMinimumWidth(i, 100);
        dailyLayout->setColumnMinimumWidth(i, 100);
        autoRunLayout->setColumnMinimumWidth(i, 100);
        specialLayout->setColumnMinimumWidth(i, 100);
        otherFunctionLayout->setColumnMinimumWidth(i, 100);

        battleLayout->setColumnStretch(i, 1);
        dailyLayout->setColumnStretch(i, 1);
        autoRunLayout->setColumnStretch(i, 1);
        specialLayout->setColumnStretch(i, 1);
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
        if (col >= 4) {
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
        if (col >= 4) {
            col = 0;
            row++;
        }
    }

    row = col = 0;
    for (int i = 1; i <= 4; ++i) {
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
        if (col >= 4) {
            col = 0;
            row++;
        }
    }

    row = col = 0;
    for (const auto &command: commandSpecial) {
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

        specialLayout->addWidget(customBtn, row, col);

        col += 1;
        if (col >= 4) {
            col = 0;
            row++;
        }
    }

    auto *viewBtn = new ButtonWithSetting("查看截屏", false);
    auto *folderBtn = new ButtonWithSetting("安装位置", false);
    auto *replaceBtn = new ButtonWithSetting("替换图片", false);
    auto *clickerBtn = new ButtonWithSetting("连点器", false);
    auto *activityBtn = new ButtonWithSetting("活动预告", false);

    viewBtn->getTextButton()->setAutoDefault(false);
    folderBtn->getTextButton()->setAutoDefault(false);
    replaceBtn->getTextButton()->setAutoDefault(false);
    clickerBtn->getTextButton()->setAutoDefault(false);
    activityBtn->getTextButton()->setAutoDefault(false);

    viewBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    folderBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    replaceBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    clickerBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    activityBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(viewBtn->getTextButton(), &QToolButton::clicked, [this, &selectDialog]() {
        if (!state.hwnd) {
            log("请先获取游戏窗口!");
            selectDialog.accept();
            return;
        }
        auto *dialog = new ImageDialog(this);
        dialog->exec();
    });

    connect(folderBtn->getTextButton(), &QToolButton::clicked, [this, &selectDialog]() {
        QString dirPath = QCoreApplication::applicationDirPath();

        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath))) {
            log("无法打开程序所在目录：" + dirPath + ", 请手动打开");
        } else {
            log("已打开程序所在目录：" + dirPath);
        }
        selectDialog.accept();
    });

    connect(replaceBtn->getTextButton(), &QToolButton::clicked, [this, &selectDialog]() {
        if (!state.hwnd) {
            log("请先获取游戏窗口!");
            selectDialog.accept();
            return;
        }

        auto *dialog = new ReplaceDialog(this);
        selectDialog.accept();
        this->showMinimized();
        dialog->show();
    });

    connect(clickerBtn->getTextButton(), &QToolButton::clicked, [this, &selectDialog]() {
        if (!state.hwnd) {
            log("请先获取游戏窗口!");
            selectDialog.accept();
            return;
        }

        auto *dialog = new ClickerDialog(this);
        selectDialog.accept();
        this->showMinimized();
        dialog->exec();
    });

    connect(activityBtn->getTextButton(), &QToolButton::clicked, [this]() {
        auto *dialog = new ActivityResultDialog(this);
        dialog->exec();
    });


    otherFunctionLayout->addWidget(viewBtn, 0, 0);
    otherFunctionLayout->addWidget(folderBtn, 0, 1);
    otherFunctionLayout->addWidget(replaceBtn, 0, 2);
    otherFunctionLayout->addWidget(clickerBtn, 0, 3);
    otherFunctionLayout->addWidget(activityBtn, 1, 0);

    layout->addWidget(battleGroupBox);
    layout->addWidget(dailyGroupBox);
    layout->addWidget(autoRunGroupBox);
    layout->addWidget(specialGroupBox);
    layout->addWidget(otherFunctionGroupBox);

    selectDialog.exec();
}

void PanelWidget::stopCommand() {
    state.stopFlag.store(true);

    if (!state.currentThread) {
        log("当前无命令正在执行");
        return;
    }

    state.currentThread->quit();
    state.currentThread->wait();
    log("命令已停止执行");
}

void PanelWidget::clearText() { outputText->clear(); }

void PanelWidget::runCommand(const QString &command) {
    if (!state.hwnd) {
        log("请先获取游戏窗口!");
        return;
    }

    state.errorList.clear();
    if (state.currentThread) {
        state.stopFlag.store(true);
        state.currentThread->quit();
        state.currentThread->wait();
    }

    state.stopFlag.store(false);
    state.currentThread = new QThread(this);

    auto func = tasks[command];
    connect(state.currentThread, &QThread::started, [func, this]() {
        try {
            func();
            if (!state.errorList.empty()) {
                for (const auto &i: state.errorList) {
                    emit logMessage(QString::fromStdString(i), "red");
                }
            }
            emit logMessage("运行完成", "red");
        } catch (const std::exception &e) {
            if (!state.stopFlag.load()) {
                emit logMessage("出错了: " + QString(e.what()), "red");
                emit logMessage("运行结束", "red");
            } else {
                emit logMessage("运行完成", "red");
            }
        }
    });

    connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
    connect(state.currentThread, &QThread::destroyed, this, []() { state.currentThread = nullptr; });
    state.currentThread->start();

    log("开始执行命令: " + command, "blue");
}

void PanelWidget::batchRunCommand(const QString &command) {
    if (!state.hwnd) {
        log("请先获取游戏窗口!");
        return;
    }

    state.errorList.clear();
    if (state.currentThread) {
        state.stopFlag.store(true);
        state.currentThread->quit();
        state.currentThread->wait();
    }

    state.stopFlag.store(false);
    state.currentThread = new QThread(this);

    connect(state.currentThread, &QThread::started, [this, &command]() {
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
                            emit logMessage("出错了: " + QString(e.what()), "red");
                            emit logMessage(subCommand + "运行错误", "red");
                        } else {
                            emit logMessage("运行完成: " + subCommand, "red");
                        }

                        throw e;
                    }
                }
            }

            if (!state.errorList.empty()) {
                for (const auto &i: state.errorList) {
                    emit logMessage(QString::fromStdString(i), "red");
                }
            }
            emit logMessage("一键执行完成", "red");
        } catch (const std::exception &e) {
            if (!state.stopFlag.load()) {
                if (!state.errorList.empty()) {
                    for (const auto &i: state.errorList) {
                        emit logMessage(QString::fromStdString(i), "red");
                    }
                }
                emit logMessage("一键执行错误", "red");
            } else {
                emit logMessage("一键执行完成", "red");
            }
        }
    });

    connect(state.currentThread, &QThread::finished, state.currentThread, &QThread::deleteLater);
    connect(state.currentThread, &QThread::destroyed, this, []() { state.currentThread = nullptr; });
    state.currentThread->start();
}

void PanelWidget::setCommand(const QString &command) {
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
    auto *confirmButton = new QPushButton("确定");
    auto *saveButton = new QPushButton("确定并保存");
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonLayout);

    connect(
            confirmButton,
            &QPushButton::clicked,
            [command, &checkboxItems, &inputItems, &settingDialog, &selectItems, commandConfig]() {

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

                settingDialog.accept();
            }
    );

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

                QFile file(QCoreApplication::applicationDirPath() + configFile);
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QJsonDocument doc(state.config);
                    file.write(doc.toJson());
                    file.close();
                    log("配置保存成功", "blue");
                } else {
                    log("配置保存失败");
                }

                settingDialog.accept();
            }
    );

    settingDialog.exec();
}

LRESULT CALLBACK PanelWidget::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_LBUTTONDOWN) {
            auto *window = qobject_cast<PanelWidget *>(qApp->activeWindow());
            if (window && window->isWaiting) {
                auto *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
                HWND hwnd = WindowFromPoint(pMouseStruct->pt);
                wchar_t buffer[256] = {0};

                GetWindowTextW(hwnd, buffer, 256);
                if (hwnd) {
                    window->log(
                            QString("获取到窗口: ") + QString::fromWCharArray(buffer) + "(0x" +
                            QString::number(reinterpret_cast<qulonglong>(hwnd), 16) + ")", "blue"
                    );
                    window->log("请不要最小化游戏窗口！但可以放在其它窗口后面", "red");
                    window->isWaiting = false;
                    state.hwnd = hwnd;
                    UnhookWindowsHookEx(window->hook);
                    window->hook = nullptr;
                } else {
                    window->log("获取窗口句柄失败", "red");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
