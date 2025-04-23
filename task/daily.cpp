#include "daily.h"

void heroCenter(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("英雄中心", "checkbox", setting);

    std::make_unique<Clicker>(
            "英雄中心/英雄.png"
    )->click(
            {.runUntilList={new Image("英雄中心/英雄中心.png")}}
    )->end();

    if (boolSetting["免费抽奖"]) {
        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>("英雄中心/英雄免费.png");

            if (!clicker->founded()) break;

            clicker = clicker->locate(
                    {.finishUntilList={new Image("英雄中心/英雄抽奖.png", {.onPrevious=Previous::TOP_CENTER})}}
            )->click(
                    {.finishUntilList={new IfImage("英雄中心/确定.png", {.startWait=1})}}
            );

            if (!clicker->founded()) continue;

            clicker->click({.finishUntilList={new Image("英雄中心/确定.png", InnerReverse)}})->end();
        }
    }

    if (boolSetting["英雄训练"]) {
        std::make_unique<Clicker>(
                "英雄中心/英雄培养.png"
        )->click(
                {.finishUntilList={new Image("英雄中心/英雄列表.png")}}
        )->end();

        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>("英雄中心/选择英雄.png");

            if (!clicker->founded()) break;

            clicker = clicker->click(
                    {.runUntilList={new AnyImage({"英雄中心/星星亮.png", "英雄中心/星星暗.png"})}}
            )->click(
                    {.selector=positionSelector("yCenter", "min"), .finishUntilList={new Image("英雄中心/开始训练.png")}}
            )->click(
                    {.finishUntilList={new Image("英雄中心/英雄列表.png"), new IfImage("英雄中心/资源不足.png")}, .finishWait=1}
            );

            if (clicker->founded()) {
                clicker->locate(
                        {.finishUntilList={new Image("英雄中心/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("英雄中心/关闭窗口.png", InnerReverse)}}
                )->end();

                emit env.emitter->error("[英雄中心]提前结束: 资源不足, 英雄训练失败");
                break;
            }
        }
    }

    std::make_unique<Clicker>(
            "英雄中心/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("英雄中心/关闭窗口.png", InnerReverse)}}
    )->end();
}

void warCenter(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("战争学院", "checkbox", setting);
    auto stringSetting = parseStringSetting("战争学院", "select", setting);

    std::make_unique<Clicker>(
            "战争学院/战争学院.png"
    )->click(
            {.runUntilList={new Image("战争学院/战争学院标题.png")}, .finishWait=1}
    )->end();

    if (boolSetting["技能训练"]) {
        clicker = std::make_unique<Clicker>("战争学院/训练.png");

        if (clicker->founded()) {
            clicker->click(
                    {.startWait=0.5, .selector=randomSelector, .runUntilList={new Image("战争学院/次数不足.png")}}
            )->locate(
                    {.finishUntilList={new Image("战争学院/关闭窗口.png")}}
            )->click(
                    {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("战争学院/关闭窗口.png", InnerReverse)}}
            )->end();
        }
    }

    std::make_unique<Clicker>(
            "战争学院/坦克研究.png"
    )->click(
            {.finishUntilList={new Image("战争学院/改装说明.png")}}
    )->end();

    if (boolSetting["坦克改装"]) {
        auto refitTank = stringSetting["改造"];
        auto refitType = stringSetting["改造项目"];

        if (refitTank != "天启") {
            std::make_unique<Clicker>(
                    QString("战争学院/%1.png").arg(QString::fromStdString(refitTank))
            )->click()->end();
        }

        clicker = std::make_unique<Clicker>(
                QString("战争学院/改装%1图片.png").arg(QString::fromStdString(refitType))
        )->click(
                {.finishUntilList={new Image(QString("战争学院/%1提示.png").arg(QString::fromStdString(refitType))), new IfImage("战争学院/确定研究完成.png")}}
        );

        if (clicker->founded()) {
            clicker->click({.finishUntilList={new Image("战争学院/确定研究完成.png", InnerReverse)}})->end();
        }


        clicker = std::make_unique<Clicker>("战争学院/改装.png");

        if (clicker->founded()) {
            clicker = clicker->click({.finishUntilList={new AnyImage({"战争学院/星级不足.png", "战争学院/材料不足.png", "战争学院/正在工作.png", "战争学院/加速.png"})}});

            if (clicker->imgPath == "战争学院/星级不足.png" || clicker->imgPath == "战争学院/材料不足.png") {
                clicker->locate(
                        {.finishUntilList={new Image("战争学院/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("战争学院/关闭窗口.png", InnerReverse)}}
                )->end();

                emit env.emitter->error(QString("[战争学院] 改造失败: %1").arg(clicker->imgPath == "战争学院/星级不足.png" ? "坦克星级不足" : "改造图纸不足"));
            } else if (clicker->imgPath == "战争学院/正在工作.png") {
                clicker->click(
                        {.finishUntilList={new Image("战争学院/正在工作.png", InnerReverse)}}
                )->end();

                emit env.emitter->error(QString("[战争学院] 改造失败: 正在工作中"));
            }
        }
    }

    std::make_unique<Clicker>(
            "战争学院/坦克干扰.png"
    )->click(
            {.finishUntilList={new Image("战争学院/干扰说明.png")}}

    )->end();

    if (boolSetting["坦克干扰"]) {
        auto jamTank = stringSetting["干扰"];
        auto jamType = stringSetting["干扰项目"];

        if (jamTank != "天启") {
            std::make_unique<Clicker>(
                    QString("战争学院/%1.png").arg(QString::fromStdString(jamTank))
            )->click()->end();
        }

        std::make_unique<Clicker>(
                QString("战争学院/干扰%1图片.png").arg(QString::fromStdString(jamType))
        )->click(
                {.finishUntilList={new Image(QString("战争学院/%1提示.png").arg(QString::fromStdString(jamType))), new IfImage("战争学院/确定研究完成.png")}}
        )->end();

        clicker = std::make_unique<Clicker>("战争学院/干扰.png");

        if (clicker->founded()) {
            clicker = clicker->click({.finishUntilList={new AnyImage({"战争学院/星级不足.png", "战争学院/材料不足.png", "战争学院/正在工作.png", "战争学院/加速.png"})}});

            if (clicker->imgPath == "战争学院/星级不足.png" || clicker->imgPath == "战争学院/材料不足.png") {
                clicker->locate(
                        {.finishUntilList={new Image("战争学院/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("战争学院/关闭窗口.png", InnerReverse)}}
                )->end();

                emit env.emitter->error(QString("[战争学院] 干扰失败: %1").arg(clicker->imgPath == "战争学院/星级不足.png" ? "坦克星级不足" : "干扰图纸不足"));
            } else if (clicker->imgPath == "战争学院/正在工作.png") {
                clicker->click(
                        {.finishUntilList={new Image("战争学院/正在工作.png", InnerReverse)}}
                )->end();

                emit env.emitter->error(QString("[战争学院] 干扰失败: 正在工作中"));
            }
        }
    }

    if (boolSetting["军事演习"]) {
        clicker = std::make_unique<Clicker>(
                "战争学院/军事演习.png"
        )->click(
                {.finishUntilList={new AnyImage({"战争学院/空演习场.png", "战争学院/演习中.png"})}}
        );

        if (clicker->imgPath == "战争学院/空演习场.png") {
            clicker = clicker->click({.finishUntilList{new IfImage("战争学院/占领演习场.png")}});

            if (clicker->founded()) {
                clicker->click({.finishUntilList={new Image("战争学院/演习中.png")}})->end();
            } else {
                emit env.emitter->error(QString("[战争学院] 演习失败: 没有空演习场"));
            }
        }
    }

    std::make_unique<Clicker>(
            "战争学院/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("战争学院/关闭窗口.png", InnerReverse)}}
    )->end();
}

void countryChest(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("国家宝箱", "checkbox", setting);

    std::make_unique<Clicker>(
            "国家宝箱/国家.png"
    )->click(
            {.runUntilList={new Image("国家宝箱/国家宝箱图标.png")}}
    )->click(
            {.finishUntilList={new Image("国家宝箱/国家宝箱标题.png")}}
    )->end();

    if (boolSetting["开一个战功宝箱"]) {
        std::make_unique<Clicker>(
                "国家宝箱/开启.png"
        )->click(
                {.selector=positionSelector("xCenter", "min")}
        )->end();
    }

    if (boolSetting["领取排名奖励"]) {
        clicker = std::make_unique<Clicker>("国家宝箱/领取排名奖励.png", ClickerInitConfig{.mode=Mode::RGB});

        if (clicker->founded()) clicker->click({.finishUntilList={new Image("国家宝箱/领取排名奖励.png", {.onPrevious=Previous::INNER, .mode=Mode::RGB, .reverse=true})}})->end();
    }

    if (boolSetting["领取战功奖励"]) {
        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>("国家宝箱/战功进度宝箱.png", ClickerInitConfig{.mode=Mode::RGB});

            if (!clicker->founded())break;

            clicker->click({.finishUntilList={new Image("国家宝箱/战功进度宝箱.png", InnerReverse)}})->end();
        }
    }

    if (boolSetting["领取月卡宝箱"]) {
        clicker = std::make_unique<Clicker>("国家宝箱/月卡宝箱.png");

        if (clicker->founded()) {
            clicker->click(
                    {.finishUntilList={new Image("国家宝箱/确定.png")}}
            )->click(
                    {.finishUntilList={new Image("国家宝箱/确定.png", InnerReverse)}}
            )->end();
        }
    }

    std::make_unique<Clicker>(
            "国家宝箱/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("国家宝箱/关闭窗口.png", InnerReverse), new Image("国家宝箱/关闭窗口.png")}}
    )->click(
            {.finishUntilList={new Image("国家宝箱/关闭窗口.png", InnerReverse)}}
    )->end();
}

void admiral(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("将领抽奖", "checkbox", setting);

    std::make_unique<Clicker>(
            "将领抽奖/将领.png"
    )->click(
            {.runUntilList={new Image("将领抽奖/将领面板.png")}}
    )->end();

    if (boolSetting["免费抽奖"]) {
        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>("将领抽奖/将领免费.png");

            if (!clicker->founded()) break;

            clicker->click(
                    {.finishUntilList={new Image("将领抽奖/将领抽奖.png", {.onPrevious=Previous::TOP_CENTER})}}
            )->click(
                    {.finishUntilList={new Image("将领抽奖/确定.png", {.finishWait=2})}}
            )->click(
                    {.finishUntilList={new Image("将领抽奖/确定.png", InnerReverse)}}
            )->end();
        }
    }

    if (boolSetting["免费学习书"]) {
        clicker = std::make_unique<Clicker>(
                "将领抽奖/将领技能.png"
        )->click(
                {.finishUntilList={new Image("将领抽奖/免费收集.png")}}
        )->click(
                {.finishUntilList={new IfImage("将领抽奖/次数已用尽.png", {.finishWait=1})}}

        );

        if (clicker->founded()) {
            clicker->locate(
                    {.finishUntilList={new Image("将领抽奖/关闭窗口.png")}}
            )->click(
                    {.selector=positionSelector("xCenter", "min"), .finishUntilList{new Image("将领抽奖/关闭窗口.png", InnerReverse)}}
            )->end();
        }
    }

    std::make_unique<Clicker>(
            "将领抽奖/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("将领抽奖/关闭窗口.png", InnerReverse)}}
    )->end();
}

void adviser(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("参谋抽奖", "checkbox", setting);

    clicker = std::make_unique<Clicker>(
            "参谋抽奖/参谋.png"
    )->click(
            {.runUntilList={new AnyImage({"参谋抽奖/参谋面板.png", "参谋抽奖/确定未开启.png"})}, .finishWait=1}
    );

    if (clicker->imgPath == "参谋抽奖/确定未开启.png") {
        clicker->click(
                {.finishUntilList={new Image("参谋抽奖/确定未开启.png", InnerReverse)}}
        )->end();
        return;
    }

    if (boolSetting["免费抽奖"]) {
        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>(std::vector<QString>{"参谋抽奖/参谋免费.png", "参谋抽奖/必得紫.png"}, ClickerInitConfig{.wait=2});

            if (!clicker->founded()) break;

            clicker->click(
                    {.finishUntilList={new Image("参谋抽奖/参谋抽奖.png", {.onPrevious=Previous::TOP_CENTER})}}
            )->click(
                    {.finishUntilList={new Image("参谋抽奖/确定.png", {.finishWait=2})}}
            )->click(
                    {.finishUntilList={new Image("参谋抽奖/确定.png", InnerReverse)}}
            )->end();
        }
    }

    if (boolSetting["免费技能书"]) {
        clicker = std::make_unique<Clicker>(
                "参谋抽奖/参谋技能.png"
        )->click(
                {.finishUntilList={new Image("参谋抽奖/参谋列表.png"), new IfImage("参谋抽奖/免费收集.png", {.mode=Mode::RGB})}}
        );

        if (clicker->founded()) clicker->click()->end(); //TODO
    }

    std::make_unique<Clicker>(
            "参谋抽奖/关闭窗口.png", ClickerInitConfig{.wait=1}
    )->click(
            {.finishUntilList={new Image("参谋抽奖/关闭窗口.png", InnerReverse)}}
    )->end();
}

void mortar(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    clicker = std::make_unique<Clicker>(
            "火炮抽奖/远程部队.png"
    )->click(
            {.runUntilList={new AnyImage({"火炮抽奖/远程火炮.png", "火炮抽奖/确定未开启.png"})}}
    );

    if (clicker->imgPath == "火炮抽奖/确定未开启.png") {
        clicker->click(
                {.finishUntilList={new Image("火炮抽奖/确定未开启.png", InnerReverse)}}
        )->end();
        return;
    }

    for (const auto &i: std::vector<QString>{"绿", "蓝", "紫"}) {
        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>(QString("火炮抽奖/%1色免费派遣.png").arg(i), ClickerInitConfig{.mode=Mode::RGB});

            if (!clicker->founded()) break;

            clicker->click(
                    {.finishUntilList={new Image("火炮抽奖/派遣.png", {.onPrevious=Previous::TOP_CENTER})}}
            )->click(
                    {.finishUntilList={new Image("火炮抽奖/确定.png", {.finishWait=1})}}
            )->click(
                    {.finishUntilList={new Image("火炮抽奖/确定.png", InnerReverse)}}
            )->end();
        }
    }

    std::make_unique<Clicker>(
            "火炮抽奖/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("火炮抽奖/关闭窗口.png", InnerReverse)}}
    )->end();
}

void equipment(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    std::make_unique<Clicker>(
            "配件抽奖/配件中心.png"
    )->click(
            {.runUntilList={new Image("配件抽奖/配件中心标题.png"), new Image("配件抽奖/野外.png")}}
    )->click(
            {.finishUntilList={new Image("配件抽奖/野外场景.png"), new Image("配件抽奖/免费探索.png")}}
    )->click(
            {.runUntilList={new Image("配件抽奖/次数用尽.png")}, .finishWait=1}
    )->click(
            {.finishUntilList={new Image("配件抽奖/次数用尽.png", InnerReverse)}}
    )->end();

    for (const auto &i: std::vector<QString>{"工厂", "实验室"}) {
        clicker = std::make_unique<Clicker>(
                QString("配件抽奖/%1.png").arg(i)
        )->click(
                {.finishUntilList={new IfImage("配件抽奖/免费探索次数.png")}, .finishWait=1}
        );
        if (!clicker->founded())continue;

        clicker->locate(
                {.finishUntilList={new Image("配件抽奖/免费探索.png", {.onPrevious=Previous::TOP})}}
        )->click(
                {.finishUntilList={new Image("配件抽奖/免费探索.png", InnerReverse)}, .finishWait=1}
        )->end();
    }

    std::make_unique<Clicker>(
            "配件抽奖/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("配件抽奖/关闭窗口.png", InnerReverse)}}
    )->end();
}

void arms(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    std::make_unique<Clicker>(
            "军备抽奖/军备研究.png"
    )->click(
            {.runUntilList={new Image("军备抽奖/军备研究提示.png")}}
    )->end();

    for (const auto &i: std::vector<QString>{"机械", "自动化"}) {
        std::make_unique<Clicker>(
                QString("军备抽奖/%1制造厂.png").arg(i)
        )->click(
                {.finishUntilList={new Image(QString("军备抽奖/%1制造厂场景.png").arg(i))}},
                1, 0, 0, Click::LEFT
        )->end();

        while (!env.stopFlag->load()) {
            clicker = std::make_unique<Clicker>("军备抽奖/免费次数.png");

            if (!clicker->founded()) break;

            clicker->locate(
                    {.finishUntilList={new Image("军备抽奖/免费制造.png", {.onPrevious=Previous::TOP_CENTER})}}
            )->click(
                    {.finishWait=1}
            )->end();
        }
    }

    std::make_unique<Clicker>(
            "军备抽奖/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image{"军备抽奖/关闭窗口.png", InnerReverse}}}
    )->end();
}

void signIn(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    clicker = std::make_unique<Clicker>(
            "每日签到/每日签到.png"
    )->click(
            {.runUntilList={new AnyImage({"每日签到/签到.png", "每日签到/已签到.png",}, {.mode=Mode::RGB})}}
    );

    if (clicker->imgPath == "每日签到/签到.png") {
        clicker->click({.runUntilList={new Image("每日签到/签到.png", InnerReverse)}})->end();
    }

    for (const auto &i: QList<QString>{"7天", "14天", "21天", "28天"}) {
        auto templateImage = QString("每日签到/%1.png").arg(i);
        clicker = std::make_unique<Clicker>(templateImage, ClickerInitConfig{.mode=Mode::RGB});

        if (!clicker->founded())continue;

        clicker->click(
                {.finishUntilList={new Image(templateImage, {.onPrevious=Previous::INNER, .mode=Mode::RGB, .finishWait=1, .reverse=true}), new Image("每日签到/关闭窗口.png")}}
        )->click(
                {.selector=positionSelector("yCenter", "max"), .runUntilList={new Image("每日签到/关闭窗口.png", InnerReverse)}}
        )->end();
    }

    std::make_unique<Clicker>(
            "每日签到/关闭窗口.png"
    )->click(
            {.runUntilList={new Image("每日签到/关闭窗口.png", InnerReverse)}}
    )->end();
}

void oreField(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("矿区争夺", "checkbox", setting);

    clicker = std::make_unique<Clicker>(
            "矿区争夺/矿区争夺战.png"
    )->click(
            {.runUntilList{new Image("矿区争夺/矿区争夺.png")}}
    )->locate(
            {.finishUntilList={new IfImage("矿区争夺/撤军.png")}}
    );

    if (clicker->founded()) {
        emit env.emitter->log("已有占领的矿区", "red");

        std::make_unique<Clicker>(
                "矿区争夺/关闭窗口.png"
        )->click(
                {.finishUntilList={new Image("矿区争夺/关闭窗口.png", InnerReverse)}}
        )->end();

        return;
    }

    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>("矿区争夺/刷新次数.png");

        if (!clicker->founded()) {
            emit env.emitter->log("免费次数已用完", "red");

            std::make_unique<Clicker>(
                    "矿区争夺/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("矿区争夺/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        clicker = clicker->click(
                {
                        .selector=positionSelector("xCenter", "max"),
                        .finishUntilList={new AnyImage({"矿区争夺/确定.png", "矿区争夺/占领.png", "矿区争夺/抢占.png"})},
                        .finishWait=1
                }, 1, 0, -26
        );

        if (boolSetting["只刷新不打"]) {
            clicker->locate(
                    {.finishUntilList={new Image("矿区争夺/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("矿区争夺/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        if (clicker->imgPath == "矿区争夺/确定.png") {
            clicker->click(
                    {.finishUntilList={new Image("矿区争夺/确定.png", InnerReverse), new Image("矿区争夺/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("矿区争夺/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        if (clicker->imgPath == "矿区争夺/占领.png") {
            clicker->click(
                    {.finishUntilList={new Image("矿区争夺/撤军.png")}}
            )->click(
                    {.finishUntilList={new Image("矿区争夺/关闭窗口.png", {.onPrevious=Previous::TOP})}}
            )->click(
                    {.finishUntilList={new Image("矿区争夺/关闭窗口.png", InnerReverse)}}
            )->end();

            return;
        }

        auto ps = CV::findPositions(CV::getScreen(env.hwnd), "矿区争夺/抢占.png");

        for (const auto &p: ps) {
            clicker = std::make_unique<Clicker>(
                    "矿区争夺/抢占.png", p
            )->click(
                    {.finishUntilList={new Image("矿区争夺/跳过.png")}}
            )->click(
                    {.runUntilList={new Image("矿区争夺/确定.png")}}
            )->click(
                    {.finishUntilList={new Image("矿区争夺/矿区争夺.png"), new IfImage("矿区争夺/撤军.png")}}
            );


            if (clicker->founded()) {
                std::make_unique<Clicker>(
                        "矿区争夺/关闭窗口.png"
                )->click(
                        {.finishUntilList={new Image("矿区争夺/关闭窗口.png", InnerReverse)}}
                )->end();

                return;
            }
        }
    }
}

void monthlyCard(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    std::make_unique<Clicker>(
            "月卡领取/畅玩月卡.png"
    )->click(
            {.runUntilList={new Image("月卡领取/一键领取.png")}}
    )->click()->end();

    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>("月卡领取/关闭窗口.png");

        if (!clicker->founded()) break;

        clicker->click(
                {.selector=positionSelector("xCenter", "min"), .runUntilList={new Image("月卡领取/关闭窗口.png", InnerReverse)}}
        )->end();
    }
}

void otherActivity(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("其它活动", "checkbox", setting);


    if (boolSetting["七天乐"]) {
        clicker = std::make_unique<Clicker>("其它活动/七天乐.png");

        if (clicker->founded()) {
            clicker = clicker->click({.finishUntilList={new Image("其它活动/七天乐标题.png"), new IfImage("其它活动/免费签到.png")}});

            if (clicker->founded()) {
                clicker->click(
                        {.finishUntilList={new Image("其它活动/签到奖励.png"), new Image("其它活动/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
                )->end();
            }

            std::make_unique<Clicker>(
                    "其它活动/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
            )->end();
        }
    }

    if (boolSetting["捕猎火鸡"]) {
        clicker = std::make_unique<Clicker>("其它活动/捕猎火鸡.png");

        if (clicker->founded()) {
            clicker = clicker->click({.finishUntilList={new Image("其它活动/捕猎火鸡标题.png"), new IfImage("其它活动/捕猎一次.png", {.mode=Mode::RGB, .threshold=0.95})}});


            if (!clicker->founded()) {
                std::make_unique<Clicker>(
                        "其它活动/关闭窗口.png"
                )->click(
                        {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
                )->end();
            } else {
                clicker->click(
                        {
                                .runUntilList={new Image("其它活动/超级玉米不足.png")},
                                .finishUntilList={new Image("其它活动/关闭窗口.png")}
                        }
                )->click(
                        {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse), new Image("其它活动/关闭窗口.png")}}
                )->click(
                        {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
                )->end();

                auto targetSegmentList = std::make_unique<Clicker>("其它活动/好友基地.png")->targetSegmentList;

                bool finished = false;
                for (const auto &i: targetSegmentList) {
                    clicker = std::make_unique<Clicker>(
                            "其它活动/好友基地.png", i
                    )->click(
                            {.finishUntilList={new Image("其它活动/返回基地.png"), new Image("其它活动/捕猎火鸡.png")}}
                    )->click(
                            {.finishUntilList={new AnyImage({"其它活动/捕猎一次.png", "其它活动/好友未上线.png"})}}
                    );

                    if (clicker->imgPath == "其它活动/好友未上线.png") {
                        std::make_unique<Clicker>(
                                "其它活动/关闭窗口.png"
                        )->click(
                                {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse), new Image("其它活动/返回基地.png")}}
                        )->click(
                                {.finishUntilList={new Image("其它活动/捕猎火鸡.png")}}
                        )->end();

                        continue;
                    }

                    clicker = clicker->click({.runUntilList={new AnyImage({"其它活动/捕猎上限.png", "其它活动/普通玉米不足.png"})}});


                    if (clicker->imgPath == "其它活动/普通玉米不足.png") finished = true;

                    std::make_unique<Clicker>(
                            "其它活动/关闭窗口.png"
                    )->click(
                            {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse), new Image("其它活动/关闭窗口.png")}}
                    )->click(
                            {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse), new Image("其它活动/返回基地.png")}}
                    )->click(
                            {.finishUntilList={new Image("其它活动/捕猎火鸡.png")}}
                    )->end();


                    if (finished) break;
                }
            }

        }
    }

    if (boolSetting["探索藏宝库"]) {
        clicker = std::make_unique<Clicker>("其它活动/探索藏宝库.png");

        if (clicker->founded()) {
            clicker->click(
                    {.finishUntilList={new Image("其它活动/抽取一次.png")}}
            )->click(
                    {.finishUntilList={new Image("其它活动/关闭窗口.png", {.startWait=1})}}
            )->click(
                    {.selector= positionSelector("xCenter", "min"), .finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse), new Image("其它活动/关闭窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
            )->end();
        }
    }

    if (boolSetting["在线领好礼"]) {
        clicker = std::make_unique<Clicker>("其它活动/在线领好礼.png");

        if (clicker->founded()) {


            auto targetList = clicker->click(
                    {.finishUntilList={new Image("其它活动/在线领好礼标题.png"), new Image("其它活动/免费领取.png", {.mode=Mode::RGB})}}
            )->targetSegmentList;

            for (const auto &i: targetList) {
                std::make_unique<Clicker>(
                        "其它活动/免费领取.png", i
                )->click(
                        {.finishUntilList={new AnyImage({"其它活动/领取成功.png", "其它活动/已领取过.png", "其它活动/时间已到.png", "其它活动/时间未到.png"}), new Image("其它活动/关闭窗口.png")}}
                )->click(
                        {.selector=positionSelector("xCenter", "min"), .finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
                )->end();
            }

            std::make_unique<Clicker>(
                    "其它活动/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
            )->end();
        }
    }

    if (boolSetting["论军功兑好礼"]) {
        clicker = std::make_unique<Clicker>("其它活动/论军功兑好礼.png");

        if (clicker->founded()) {
            clicker->click({.runUntilList={new Image("其它活动/论军功兑好礼标题.png")}})->end();

            while (!env.stopFlag->load()) {
                clicker = std::make_unique<Clicker>("其它活动/兑换.png", ClickerInitConfig{.mode=Mode::RGB});

                if (!clicker->founded()) break;

                clicker->click({.finishWait=3})->end();
            }

            std::make_unique<Clicker>(
                    "其它活动/关闭窗口.png"
            )->click(
                    {.finishUntilList={new Image("其它活动/关闭窗口.png", InnerReverse)}}
            )->end();
        }
    }
}

void dailyTask(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    std::make_unique<Clicker>(
            "每日任务/每日任务.png"
    )->click(
            {.runUntilList={new Image("每日任务/奖励.png")}}
    )->end();

    for (const auto &i: std::vector<QString>{"10", "30", "50", "80", "100"}) {
        clicker = std::make_unique<Clicker>(QString("每日任务/%1活跃度.png").arg(i), ClickerInitConfig{.mode=Mode::RGB});

        if (!clicker->founded()) continue;

        clicker->click(
                {.finishUntilList={new Image("每日任务/领取活跃度奖励.png")}, .finishWait=1}
        )->click(
                {.finishUntilList={new Image("每日任务/领取活跃度奖励.png", InnerReverse)}}
        )->end();
    }

    std::make_unique<Clicker>(
            "每日任务/每日激战.png"
    )->click(
            {.finishUntilList={new Image("每日任务/领取积分奖励.png")}}
    )->click(
            {.finishUntilList={new Image("每日任务/关闭窗口.png")}}
    )->click(
            {.finishUntilList={new Image("每日任务/关闭窗口.png", InnerReverse)}}
    )->end();
}

void weeklyTask(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;

    std::make_unique<Clicker>(
            "周任务/周任务.png"
    )->click(
            {.runUntilList={new Image("周任务/周目标标题.png")}}
    )->end();

    while (!env.stopFlag->load()) {
        clicker = std::make_unique<Clicker>("周任务/领取.png", ClickerInitConfig{.mode=Mode::RGB});

        if (!clicker->founded()) break;

        clicker->click({.runUntilList={new Image("周任务/领取.png", {.onPrevious=Previous::INNER, .mode=Mode::RGB, .reverse=true})}})->end();
    }

    std::make_unique<Clicker>(
            "周任务/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("周任务/关闭窗口.png", InnerReverse)}}
    )->end();
}

void guild(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("公会领奖", "checkbox", setting);

    if (boolSetting["领取公会战奖励"]) {
        clicker = std::make_unique<Clicker>("公会领奖/参加公会战.png");

        if (clicker->founded()) {
            clicker = clicker->click({.finishUntilList={new Image("公会领奖/公会战争标题.png"), new IfImage("公会领奖/报名公会战.png")}});

            if (!clicker->founded()) {
                std::make_unique<Clicker>(
                        "公会领奖/领取奖励.png"
                )->click(
                        {.finishUntilList={new Image("公会领奖/领取奖励.png", {.onPrevious=Previous::RIGHT})}}
                )->click(
                        {.finishUntilList={new Image("公会领奖/确定.png")}}
                )->click(
                        {.finishUntilList={new Image("公会领奖/报名公会战.png")}}
                )->end();
            }

            std::make_unique<Clicker>(
                    "公会领奖/报名公会战.png"
            )->click(
                    {.finishUntilList={new Image("公会领奖/确定.png")}}
            )->click(
                    {.finishUntilList={new Image("公会领奖/关闭公会战窗口.png")}}
            )->click(
                    {.finishUntilList={new Image("公会领奖/公会.png")}}
            )->end();
        }
    }

    std::make_unique<Clicker>(
            "公会领奖/公会.png"
    )->click(
            {.runUntilList={new Image("公会领奖/公会福利.png")}}
    )->end();


    if (boolSetting["领取活跃度奖励"]) {
        std::make_unique<Clicker>(
                "公会领奖/捐献.png"
        )->locate(
                {.finishUntilList={new Image("公会领奖/活跃任务.png", {.onPrevious=Previous::RIGHT})}}
        )->click(
                {.runUntilList={new Image("公会领奖/活跃任务按钮.png")}}
        )->click(
                {.finishUntilList={new Image("公会领奖/今日公会活跃.png")}}
        )->end();

        for (const auto &i: std::vector<QString>{"第一档", "第二档", "第三档", "第四档", "第五档"}) {
            clicker = std::make_unique<Clicker>(QString("公会领奖/%1.png").arg(i), ClickerInitConfig{.mode=Mode::RGB});

            if (!clicker->founded()) continue;

            clicker->click(
                    {.runUntilList={new Image("公会领奖/确定.png")}, .finishWait=1}
            )->click(
                    {.runUntilList={new Image("公会领奖/确定.png", InnerReverse)}}
            )->end();
        }

        std::make_unique<Clicker>(
                "公会领奖/关闭公会捐献窗口.png"
        )->click(
                {.finishUntilList={new Image("公会领奖/关闭公会捐献窗口.png", InnerReverse)}}
        )->end();
    }


    std::make_unique<Clicker>(
            "公会领奖/公会争霸.png"
    )->click(
            {.finishUntilList={new Image("公会领奖/公会争霸标题.png")}}
    )->end();

    if (boolSetting["领取夺城战奖励"]) {
        clicker = std::make_unique<Clicker>("公会领奖/领取夺城战奖励.png");

        if (clicker->founded()) {
            clicker = clicker->click({.finishUntilList={new IfImage("公会领奖/确定.png")}});

            if (clicker->founded()) {
                clicker->click({.finishUntilList={new Image("公会领奖/确定.png", InnerReverse)}})->end();
            }
        }
    }

    if (boolSetting["领取公会战役宝箱"]) {
        clicker = std::make_unique<Clicker>(
                "公会领奖/公会战役.png"
        )->click(
                {.finishUntilList={new Image("公会领奖/查看成员排名.png"), new IfImage("公会领奖/领取公会战役奖励.png")}}
        );

        if (clicker->founded()) {
            clicker->click({.finishWait=0.5})->end();

            while (!env.stopFlag->load()) {
                clicker = std::make_unique<Clicker>("公会领奖/确定.png");

                if (!clicker->founded()) break;

                clicker->click({.finishWait=1})->end();
            }

        }
    }

    std::make_unique<Clicker>(
            "公会领奖/关闭公会争霸窗口.png"
    )->click(
            {.finishUntilList={new Image("公会领奖/关闭公会争霸窗口.png", InnerReverse)}}
    )->end();
}

void guildBuilding(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto intSetting = parseIntSetting("公会建筑", "input", setting);

    auto step_x = intSetting["拖动X"];
    auto step_y = intSetting["拖动Y"];
    auto start_x = 500;
    auto start_y = 500;


    clicker = std::make_unique<Clicker>("公会建筑/公会建筑.png");

    if (!clicker->founded()) {
        Mouse::drag(env.hwnd, start_x, start_y, start_x + step_x, start_y + step_y);

        clicker = std::make_unique<Clicker>("公会建筑/公会建筑.png", ClickerInitConfig{.wait=1});

        if (!clicker->founded()) {
            emit env.emitter->error("[公会建筑]提前结束: 无法定位公会建筑");
            return;
        }

    }

    clicker->click(
            {.finishUntilList={new Image("公会建筑/打开公会建筑.png")}}
    )->click(
            {.finishUntilList={new Image("公会建筑/公会建筑标题.png")}, .finishWait=1}
    )->click(
            {.finishUntilList={new Image("公会建筑/团体任务.png")}}
    )->click(
            {.finishUntilList={new Image("公会建筑/参与任务.png")}}
    )->click(
            {.runUntilList={new Image("公会建筑/勋章刷新.png", InnerReverse)}}
    )->end();


    auto iterator = CV::findPositions(CV::getScreen(env.hwnd), "公会建筑/需要人数.png");

    auto it = iterator.begin();

    while (it != iterator.end() && !env.stopFlag->load()) {
        it->click();

        clicker = std::make_unique<Clicker>("公会建筑/领取奖励.png", ClickerInitConfig{.wait=1, .mode=Mode::RGB});

        if (clicker->founded()) {
            clicker->click(
                    {.finishUntilList={new Image("公会建筑/确定领取.png")}}
            )->click(
                    {.finishUntilList={new Image("公会建筑/确定领取.png", InnerReverse)}}
            )->end();
        }

        ++it;
    }

    std::make_unique<Clicker>(
            "公会建筑/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("公会建筑/关闭窗口.png", InnerReverse)}}
    )->end();
}