#include "cmdSelector.h"

CmdSelector::CmdSelector(QWidget *parent) : QDialog(parent) {
    tasks["英雄中心"] = heroCenter;
    tasks["国家战争"] = countryWar;

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createCmdGroup(
            "自动战斗",
            {
                    "国家争霸", "世界争霸", "剿灭将领", "军备合成",
                    "公会战役", "国家战争"
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
            "自动战斗",
            {
                    "国家争霸", "世界争霸", "剿灭将领", "军备合成",
                    "公会战役", "国家战争"
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
            this->accept();
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
