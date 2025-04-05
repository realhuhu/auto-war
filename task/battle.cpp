#include "battle.h"

void countryWar(const Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = parseBoolSetting("国家战争", "checkbox", env.setting);

    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);
    int hour = local_time->tm_hour;

    if (hour >= 20 && (setting["8点领签到体力"] || setting["8点领VIP体力"] || setting["8点领VIP国战大礼"])) {
        clicker = std::make_unique<Clicker>("国家战争/返回基地.png");

        if (clicker->founded()) {
            clicker->click(
                    {.finishUntilList={new Image("国家战争/活动及公告.png")}}
            )->end();
        }


        if (setting["8点领签到体力"]) {
            clicker = std::make_unique<Clicker>("国家战争/活动及公告.png")->click(
                    {
                            .runUntilList={new Image("国家战争/连续登录.png", {.finishWait=2})},
                            .finishUntilList={new IfImage("国家战争/领取连续登录奖励.png", {.mode=Mode::RGB})}
                    }
            );

            if (clicker->founded()) {
                clicker = clicker->click(
                        {.runUntilList={new Image("国家战争/领取连续登录奖励.png", InnerReverse)}}
                );
            }

            clicker->locate(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
            )->end();
        }

        if (setting["8点领VIP体力"]) {
            clicker = std::make_unique<Clicker>("国家战争/vip福利礼包.png")->click(
                    {.runUntilList={new Image("国家战争/vip福利礼包标题.png")}}
            )->locate(
                    {.finishUntilList={new IfImage("国家战争/点击领取.png", {.mode=Mode::RGB})}}
            );

            if (clicker->founded()) {
                clicker = clicker->click(
                        {.finishUntilList={new AnyImage({"国家战争/未购买vip月卡.png", "国家战争/已成功领取.png"}), new Image("国家战争/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
                );
            }

            clicker = clicker->locate(
                    {.finishUntilList={new Image("国家战争/每周折扣礼包.png")}}
            )->click(
                    {.finishUntilList={new Image("国家战争/vip恢复卡.png"), new IfImage("国家战争/免费领取.png", {.mode=Mode::RGB})}}
            );

            if (clicker->founded()) {
                clicker = clicker->click(
                        {.finishUntilList={new AnyImage({"国家战争/未开通vip.png", "国家战争/未购买vip月卡.png", "国家战争/已成功领取.png"}), new Image("国家战争/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
                );
            }
            clicker->locate(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
            )->end();
        }


        if (setting["8点领VIP国战大礼"]) {
            clicker = std::make_unique<Clicker>("国家战争/vip国战大礼.png");

            if (clicker->founded()) {
                clicker->click(
                        {.finishUntilList={new Image("国家战争/领取.png")}}
                )->click(
                        {.finishUntilList={new AnyImage({"国家战争/非vip用户.png", "国家战争/时间未到.png", "国家战争/领取成功.png", "国家战争/已领取过.png"}), new Image("国家战争/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse), new Image("国家战争/关闭窗口.png")}}
                )->click(
                        {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
                )->end();
            }
        }
    }

    std::make_unique<Clicker>("国家战争/国家.png")->click(
            {.runUntilList={new Image("国家战争/世界地图.png")}}
    )->click(
            {.finishUntilList={new Image("国家战争/前往.png")}}
    )->end();

    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>("国家战争/当前所在.png")->locate(
                {.finishUntilList={new IfAnyImage(
                        {
                                "国家战争/纽约.png", "国家战争/芝加哥.png", "国家战争/波尔多.png",
                                "国家战争/马赛.png", "国家战争/慕尼黑.png", "国家战争/法兰克福.png",
                                "国家战争/列宁格勒.png", "国家战争/叶卡捷琳堡.png", "国家战争/都灵.png",
                                "国家战争/米兰.png", "国家战争/伯明翰.png", "国家战争/曼彻斯特.png"
                        }, {.onPrevious=Previous::RIGHT_CENTER}
                )}}
        );

        if (!clicker->founded()) {
            std::make_unique<Clicker>("国家战争/返回基地.png")->click(
                    {.finishUntilList={new Image("国家战争/国家.png")}}
            )->end();

            emit env.emitter->log("国家战争: 已提前结束，请先移动到与摩多城相邻的城");
            return;
        }

        auto city = QFileInfo(clicker->imgPath).baseName();
        emit env.emitter->log(QString::fromStdString("当前所在: " + city.toStdString()), "blue");

        int offsetX = 0;
        int offsetY = 0;
        QString anchor;
        if (city == "芝加哥") {
            offsetX = 341;
            offsetY = -119;
            anchor = "旧金山城";
        } else if (city == "纽约") {
            offsetX = -171;
            offsetY = -285;
            anchor = "旧金山城";
        } else if (city == "米兰") {
            offsetX = 267;
            offsetY = -207;
            anchor = "那不勒斯城";
        } else if (city == "都灵") {
            offsetX = -310;
            offsetY = 75;
            anchor = "那不勒斯城";
        } else if (city == "波尔多") {
            offsetX = -10;
            offsetY = -281;
            anchor = "巴黎城";
        } else if (city == "马赛") {
            offsetX = 114;
            offsetY = 267;
            anchor = "巴黎城";
        } else if (city == "列宁格勒") {
            offsetX = -332;
            offsetY = -90;
            anchor = "斯大林格勒城";
        } else if (city == "叶卡捷琳堡") {
            offsetX = 358;
            offsetY = 89;
            anchor = "斯大林格勒城";
        } else if (city == "法兰克福") {
            offsetX = 112;
            offsetY = 249;
            anchor = "汉堡城";
        } else if (city == "慕尼黑") {
            offsetX = 385;
            offsetY = -47;
            anchor = "汉堡城";
        } else if (city == "伯明翰") {
            offsetX = 159;
            offsetY = 54;
            anchor = "伯明翰城";
        } else if (city == "曼彻斯特") {
            offsetX = 89;
            offsetY = -206;
            anchor = "曼彻斯特城";
        }

        auto cityPath = QString("国家战争/%1.png").arg(anchor);

        clicker = clicker->locate(
                {.finishUntilList={new Image("国家战争/前往.png")}}
        )->click(
                {
                        .startWait=0.5,
                        .runUntilList={new Image(cityPath)},
                        .finishUntilList={new ImageStable(cityPath)}
                }, 3
        )->click(
                {.runUntilList={new Image("国家战争/城市信息.png")}, .finishWait=1.5}, 1, offsetX, offsetY
        )->locate(
                {.finishUntilList ={new IfImage("国家战争/扫荡.png")}}
        );

        if (!clicker->founded()) {
            clicker = std::make_unique<Clicker>("国家战争/召唤支援兵.png")->click(
                    {.finishUntilList={new AnyImage({"国家战争/去购买.png", "国家战争/确定.png"})}}
            );

            if (clicker->imgPath == "国家战争/去购买.png") {
                clicker = clicker->click(
                        {.finishUntilList={new Image("国家战争/兑换.png")}}
                )->click(
                        {.finishUntilList={new Image("国家战争/确定.png")}}
                )->click(
                        {.finishUntilList={new Image("国家战争/确定.png", InnerReverse), new Image("国家战争/召唤支援兵.png")}}
                )->click(
                        {.finishUntilList={new Image("国家战争/确定.png")}, .finishWait=0.5}
                );
            }

            clicker = clicker->click(
                    {.finishUntilList={new Image("国家战争/扫荡.png")}}
            );
        }

        clicker = clicker->click(
                {
                        .startUntilList={new Image("国家战争/可战斗.png", {.mode=Mode::RGB, .finishWait=0.6})},
                        .finishUntilList={new AnyImage({"国家战争/恢复行动力.png", "国家战争/跳过战斗.png", "国家战争/战斗CD中.png"})}
                }
        );

        if (clicker->imgPath == "国家战争/恢复行动力.png") {
            clicker->locate(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", {.onPrevious=Previous::RIGHT})}}
            )->click(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse), new Image("国家战争/领取图纸.png")}}
            )->click(
                    {.finishUntilList={new Image("国家战争/返回基地.png")}}
            )->click(
                    {.finishUntilList={new Image("国家战争/国家.png")}}
            )->end();

            return;
        }

        if (clicker->imgPath == "国家战争/战斗CD中.png") {
            clicker->locate(
                    {.finishUntilList={new Image("国家战争/确定.png", {.onPrevious=Previous::DOWN})}}
            )->click(
                    {.finishUntilList={new Image("国家战争/确定.png", InnerReverse), new Image("国家战争/当前所在.png")}}
            )->end();

            continue;
        }

        clicker = clicker->click({.runUntilList={new AnyImage({"国家战争/结束战斗.png", "国家战争/国家排行榜.png"})}});

        if (clicker->imgPath == "国家战争/国家排行榜.png") {
            clicker->locate(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse), new Image("国家战争/当前所在.png")}}
            )->end();

            continue;
        }

        clicker->click({.finishUntilList={new Image("国家战争/结束战斗.png", InnerReverse), new Image("国家战争/当前所在.png")}})->end();
    }
}
