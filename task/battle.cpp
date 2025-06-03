#include "battle.h"

void countryArena(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto intSetting = parseIntSetting("国家争霸", "input", setting);

    clicker = std::make_unique<Clicker>(
            "国家争霸/争霸战.png"
    )->click(
            {.finishUntilList={new Image("国家争霸/争霸战图标.png", {.startWait=1})}}
    )->click(
            {.finishUntilList={new AnyImage({"国家争霸/争霸战标题.png", "国家争霸/未开启.png"})}}
    );

    if (clicker->imgPath == "国家争霸/未开启.png") {
        clicker->locate(
                {.finishUntilList={new Image("国家争霸/关闭窗口.png")}}
        )->click(
                {.finishUntilList={new Image("国家争霸/关闭窗口.png", InnerReverse)}}
        )->end();

        return;
    }

    clicker = std::make_unique<Clicker>("国家争霸/领取积分.png", ClickerInitConfig{.mode=Mode::RGB});

    if (clicker->founded()) {
        clicker->click({.finishUntilList={new Image("国家争霸/领取积分.png", {.onPrevious=Previous::INNER, .mode=Mode::RGB, .reverse=true})}})->end();
    }

    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>(
                "国家争霸/攻击.png", ClickerInitConfig{.wait=1}
        )->click(
                {
                        .selector=orderedRandomSelector("yCenter", "min", intSetting["打前"]),
                        .startUntilList={new Image("国家争霸/可攻击.png")},
                        .finishUntilList={new AnyImage({"国家争霸/跳过战斗.png", "国家争霸/购买次数.png"})}
                }
        );

        if (clicker->imgPath == "国家争霸/购买次数.png") {
            clicker->locate(
                    {.finishUntilList={new Image("国家争霸/关闭窗口.png")}}
            )->click(
                    {
                            .selector=positionSelector("xCenter", "min"),
                            .finishUntilList={new Image("国家争霸/购买次数.png", InnerReverse), new Image("国家争霸/关闭窗口.png")}
                    }
            )->click(
                    {.runUntilList={new Image("国家争霸/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        clicker = clicker->click(
                {.runUntilList={new Image("国家争霸/结束战斗.png")}}
        )->click(
                {.finishUntilList={new Image("国家争霸/争霸战标题.png", {.finishWait=1}), new IfImage("国家争霸/确认失败.png")}}
        );

        if (clicker->founded()) {
            clicker->click({.finishUntilList={new IfImage("国家争霸/确认失败.png", InnerReverse)}})->end();
        }
    }
}

void worldArena(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto intSetting = parseIntSetting("世界争霸", "input", setting);

    clicker = std::make_unique<Clicker>(
            "世界争霸/争霸战.png"
    )->click(
            {.finishUntilList={new Image("世界争霸/争霸战图标.png", {.startWait=1})}}
    )->click(
            {.finishUntilList={new AnyImage({"世界争霸/争霸战标题.png", "世界争霸/未开启.png", "世界争霸/未参加.png"})}}
    );

    if (clicker->imgPath == "世界争霸/未开启.png" || clicker->imgPath == "世界争霸/未参加.png") {
        clicker->locate(
                {.finishUntilList={new Image("世界争霸/关闭窗口.png")}}
        )->click(
                {.finishUntilList={new Image("世界争霸/关闭窗口.png", InnerReverse)}}
        )->end();
        return;
    }


    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>(
                "世界争霸/攻击.png", ClickerInitConfig{.wait=1}
        )->click(
                {
                        .selector=orderedRandomSelector("yCenter", "min", intSetting["打前"]),
                        .startUntilList={new Image("世界争霸/可攻击.png")},
                        .finishUntilList={new AnyImage({"世界争霸/跳过战斗.png", "世界争霸/购买次数.png"})}
                }
        );

        if (clicker->imgPath == "世界争霸/购买次数.png") {
            clicker->locate(
                    {.finishUntilList={new Image("世界争霸/关闭窗口.png")}}
            )->click(
                    {
                            .selector=positionSelector("xCenter", "min"),
                            .finishUntilList={new Image("世界争霸/购买次数.png", InnerReverse), new Image("世界争霸/关闭窗口.png")}
                    }
            )->click(
                    {.runUntilList={new Image("世界争霸/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        clicker = clicker->click(
                {.runUntilList={new Image("世界争霸/结束战斗.png")}}
        )->click(
                {.finishUntilList={new Image("世界争霸/争霸战标题.png", {.finishWait=1}), new IfImage("世界争霸/确认失败.png")}}
        );

        if (clicker->founded()) {
            clicker->click({.finishUntilList={new IfImage("世界争霸/确认失败.png", InnerReverse)}})->end();
        }
    }
}

void exterminateEnemy(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("剿灭将领", "checkbox", setting);

    clicker = std::make_unique<Clicker>("剿灭将领/剿灭将领标题.png");

    if (!clicker->founded()) {
        clicker = std::make_unique<Clicker>(
                "剿灭将领/剿灭将领.png"
        )->click(
                {.finishUntilList={new AnyImage({"剿灭将领/剿灭将领标题.png", "剿灭将领/等级不足.png"})}}
        );

        if (clicker->imgPath == "剿灭将领/等级不足.png") {
            clicker->locate(
                    {.finishUntilList={new Image("剿灭将领/确定.png")}}
            )->click(
                    {.finishUntilList={new Image("剿灭将领/确定.png", InnerReverse)}}
            )->end();

            return;
        }
    }

    std::vector<QString> chosen;

    for (const auto &pair: boolSetting) {
        if (pair.second) chosen.emplace_back(QString("剿灭将领/%1.png").arg(QString::fromStdString(pair.first)));
    }

    std::sort(chosen.begin(), chosen.end(), [](const QString &a, const QString &b) {
        auto getPriority = [](const QString &s) {
            if (s.startsWith("紫")) return 0;
            if (s.startsWith("蓝")) return 1;
            if (s.startsWith("绿")) return 2;
            return 3;
        };
        int prioA = getPriority(a);
        int prioB = getPriority(b);
        return (prioA != prioB) ? (prioA < prioB) : (a < b);
    });

    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>("剿灭将领/合成.png", ClickerInitConfig{.mode=Mode::RGB});

        if (clicker->founded()) {
            clicker->click(
                    {.finishUntilList={new Image("剿灭将领/确定.png")}}
            )->click(
                    {.finishUntilList={new Image("剿灭将领/确定.png", InnerReverse)}}
            )->end();
        }

        clicker = std::make_unique<Clicker>(
                "剿灭将领/信物商店.png"
        )->locate(
                {.finishUntilList={new IfAnyImage(chosen, {.onPrevious=Previous::RIGHT, .mode=Mode::RGB})}}
        );

        if (!clicker->founded()) {
            clicker = std::make_unique<Clicker>(
                    "剿灭将领/刷新.png"
            )->click(
                    {.finishUntilList={new IfImage("剿灭将领/确定.png")}}
            );

            if (clicker->founded()) clicker->click({.finishUntilList={new Image("剿灭将领/确定.png", InnerReverse)}})->end();

            continue;
        }

        clicker = clicker->locate(
                {
                        .startUntilList={new Image("剿灭将领/战斗进行中.png", {.reverse=true})},
                        .finishUntilList={new Image("剿灭将领/进攻.png", {.onPrevious=Previous::DOWN_CENTER, .finishWait=1})}
                }
        )->click(
                {.runUntilList={new AnyImage({"剿灭将领/确定.png", "剿灭将领/次数不足.png"})}}
        );

        if (clicker->imgPath == "剿灭将领/次数不足.png") {
            clicker->locate(
                    {.finishUntilList={new Image("剿灭将领/关闭窗口.png")}}
            )->click(
                    {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("剿灭将领/关闭窗口.png", InnerReverse), new Image("剿灭将领/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("剿灭将领/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        clicker->click({.finishUntilList={new Image("剿灭将领/信物商店.png", {.finishWait=3})}})->end();
    }
}

void armsCompound(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("军备合成", "checkbox", setting);

    clicker = std::make_unique<Clicker>("军备合成/合成军备.png");

    if (!clicker->founded()) {
        std::vector<QString> candidates;
        for (const auto &[key, value]: boolSetting) {
            if (value) candidates.push_back(QString("军备合成/%1.png").arg(QString::fromStdString(key)));
        }

        clicker = std::make_unique<Clicker>(
                "军备合成/军备研究图标.png"
        )->click(
                {.runUntilList={new Image("军备合成/军备合成.png")}}
        )->click(
                {.runUntilList={new Image("军备合成/选择品质.png")}}
        )->click(
                {.finishUntilList={new Image("军备合成/橙色品质.png")}}
        )->click(
                {.finishUntilList={new Image("军备合成/合成军备.png")}}
        )->click(
                {.finishUntilList={new AnyImage({"军备合成/滚动条A.png", "军备合成/滚动条B.png"})}}
        )->drag(
                {.runUntilList={new Image(choice(candidates))}}
        );

        if (!clicker->founded()) clicker = std::make_unique<Clicker>(candidates);
    }

    clicker = clicker->click({.finishUntilList={new IfImage("军备合成/合成军备.png", {.mode=Mode::RGB})}});

    if (clicker->founded()) clicker->click({.runUntilList={new Image("军备合成/获取碎片.png")}})->end();


    clicker = std::make_unique<Clicker>(
            "军备合成/获取碎片.png"
    )->click(
            {.finishUntilList={new Image("军备合成/开始战斗.png")}}
    );

    while (!env.stopFlag->load()) {
        clicker = clicker->click({.finishUntilList={new AnyImage({"军备合成/跳过战斗.png", "军备合成/次数不足.png"})}});

        if (clicker->imgPath == "军备合成/次数不足.png") {
            clicker->click(
                    {.runUntilList{new Image("军备合成/次数不足.png", InnerReverse), new Image("军备合成/关闭窗口.png", {.startWait=1})}}
            )->click(
                    {.finishUntilList={new Image("军备合成/关闭窗口.png", InnerReverse), new Image("军备合成/关闭窗口.png", {.startWait=1})}}
            )->click(
                    {.finishUntilList={new Image("军备合成/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        clicker = clicker->click(
                {.runUntilList={new Image("军备合成/结束战斗.png")}}
        )->click(
                {.finishUntilList={new Image("军备合成/开始战斗.png")}}
        );
    }
}

void countryWar(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("国家战争", "checkbox", setting);

    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);
    int hour = local_time->tm_hour;

    if (hour >= 20 && (boolSetting["8点领签到体力"] || boolSetting["8点领VIP体力"] || boolSetting["8点领VIP国战大礼"])) {
        clicker = std::make_unique<Clicker>("国家战争/返回基地.png");

        if (clicker->founded()) {
            clicker->click(
                    {.finishUntilList={new Image("国家战争/活动及公告.png")}}
            )->end();
        }

        if (boolSetting["8点领VIP体力"]) {
            clicker = std::make_unique<Clicker>("国家战争/vip福利礼包.png")->click(
                    {.runUntilList={new Image("国家战争/vip福利礼包标题.png")}}
            )->locate(
                    {.finishUntilList={new IfImage("国家战争/点击领取.png", {.mode=Mode::RGB})}}
            );

            if (clicker->founded()) {
                clicker->click(
                        {.finishUntilList={new AnyImage({"国家战争/未购买vip月卡.png", "国家战争/已成功领取.png"}), new Image("国家战争/关闭窗口.png")}}//TODO 未购买vip
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
                )->end();
            }

            clicker = std::make_unique<Clicker>(
                    "国家战争/每周折扣礼包.png"
            )->click(
                    {.finishUntilList={new Image("国家战争/vip恢复卡.png"), new IfImage("国家战争/免费领取.png", {.mode=Mode::RGB})}}
            );

            if (clicker->founded()) {
                clicker->click(
                        {.finishUntilList={new AnyImage({"国家战争/未开通vip.png", "国家战争/未购买vip月卡.png", "国家战争/已成功领取.png"}), new Image("国家战争/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
                )->end();
            }

            std::make_unique<Clicker>(
                    "国家战争/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
            )->end();
        }


        if (boolSetting["8点领VIP国战大礼"]) {
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


        if (boolSetting["8点领签到体力"]) {
            clicker = std::make_unique<Clicker>("国家战争/活动及公告.png")->click(
                    {
                            .runUntilList={new Image("国家战争/连续登录.png")},
                            .finishUntilList={new IfImage("国家战争/领取连续登录奖励.png", {.mode=Mode::RGB, .startWait=1})}
                    }
            );

            if (clicker->founded()) {
                clicker->click({.runUntilList={new Image("国家战争/领取连续登录奖励.png", {.onPrevious=Previous::INNER, .mode=Mode::RGB, .reverse=true})}})->end();
            }

            std::make_unique<Clicker>(
                    "国家战争/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("国家战争/关闭窗口.png", InnerReverse)}}
            )->end();
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
            std::make_unique<Clicker>(
                    "国家战争/返回基地.png"
            )->click(
                    {.finishUntilList={new Image("国家战争/国家.png")}}
            )->end();

            emit env.emitter->error("[国家战争] 提前结束: 请先移动到与摩多城相邻的城");
            env.context["不在摩多城"] = true;
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

void loopCountryWar(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("开卡国战", "checkbox", setting);
    auto intSetting = parseIntSetting("开卡国战", "input", setting);
    std::vector<std::unique_ptr<Until>> runUntil;

    auto current = intSetting["开卡数"];
    auto rounds = intSetting["开卡数"];

    std::vector<QString> candidates;
    if (boolSetting["行动力恢复卡"]) candidates.emplace_back("开卡国战/行动力恢复卡.png");
    if (boolSetting["国战恢复卡"]) candidates.emplace_back("开卡国战/国战恢复卡.png");

    while (!env.stopFlag->load()) {
        countryWar(e);

        if (env.context["不在摩多城"]) return;

        if (candidates.empty()) {
            emit env.emitter->error("[开卡国战]提前结束: 未设置开卡");
            return;
        }

        if (intSetting["开卡数"] > 0) {
            if (current == 0) {
                emit env.emitter->log("开卡数已达到目标", "blue");
                return;
            }

            emit env.emitter->log(QString("轮次: %1/%2").arg(QString::number(rounds - current + 1), QString::number(rounds)), "blue");
        }

        clicker = std::make_unique<Clicker>(
                "开卡国战/背包.png"
        )->click(
                {.runUntilList={new AnyImage({"开卡国战/滚动条A.png", "开卡国战/滚动条B.png"})}}
        )->drag(
                {.runUntilList={new AnyImage(candidates)}}
        );

        if (!clicker->founded()) {
            std::make_unique<Clicker>(
                    "开卡国战/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("开卡国战/关闭窗口.png", InnerReverse)}}
            )->end();

            emit env.emitter->error("[开卡国战]提前结束: 恢复卡不足");
            return;
        }

        clicker->click(
                {.finishUntilList={new Image("开卡国战/使用.png")}}
        )->click(
                {.finishUntilList={new Image("开卡国战/使用.png", InnerReverse), new Image("开卡国战/关闭窗口.png")}}
        )->click(
                {.finishUntilList={new Image("开卡国战/关闭窗口.png", InnerReverse)}}
        )->end();

        current--;
    }
}
