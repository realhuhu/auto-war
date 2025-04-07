#include "cmdSelector.h"

CmdSelector::CmdSelector(const QString &title, QWidget *parent) : QDialog(parent) {
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    tasks["国家争霸"] = countryArena;
    tasks["世界争霸"] = worldArena;
    tasks["剿灭将领"] = exterminateEnemy;
    tasks["军备合成"] = armsCompound;
    tasks["国家战争"] = countryWar;

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

    tasks["预设1"] = presupposition1;
    tasks["预设2"] = presupposition2;
    tasks["预设3"] = presupposition3;
    tasks["预设4"] = presupposition4;

    tasks["开卡国战"] = loopCountryWar;

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createCmdGroup(
            "自动战斗",
            {
                    "国家争霸", "世界争霸", "剿灭将领", "军备合成",
                    "国家战争"
            }
    ));
    mainLayout->addWidget(
            createCmdGroup(
                    "日常任务",
                    {
                            "英雄中心", "战争学院", "国家宝箱", "将领抽奖",
                            "参谋抽奖", "火炮抽奖", "配件抽奖", "军备抽奖",
                            "每日签到", "矿区争夺", "月卡领取", "其它活动",
                            "每日任务", "周任务", "公会领奖", "公会建筑"
                    }
            ));
    mainLayout->addWidget(createCmdGroup(
            "一键执行",
            {
                    "预设1", "预设2", "预设3", "预设4"
            }
    ));
    mainLayout->addWidget(createCmdGroup(
            "特殊功能",
            {
                    "开卡国战"
            }
    ));
}

QGroupBox *CmdSelector::createCmdGroup(const QString &groupTitle, const QStringList &cmdList) {
    auto *groupBox = new QGroupBox(groupTitle, this);
    groupBox->setStyleSheet(R"(
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
    )");

    auto gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(5);

    auto row = 0;
    auto col = 0;
    for (const auto &command: cmdList) {
        auto *btn = new QPushButton(command);
        btn->setAutoDefault(false);

        connect(btn, &QPushButton::clicked, [this, command]() {
            accept();
            emit taskCreated(command, createTask(command));
        });

        gridLayout->addWidget(btn, row, col);

        col += 1;
        if (col >= 4) {
            col = 0;
            row++;
        }
    }

    return groupBox;
}

std::function<void(Env &env)> CmdSelector::createTask(const QString &cmd) {
    return tasks[cmd];
}
