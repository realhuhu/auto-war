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
                    {.finishUntilList={new Image("英雄中心/英雄抽奖.png", {.onPrevious=Previous::TOP_CENTER, .finishWait=1})}}
            )->click(
                    {.finishUntilList={new IfImage("英雄中心/确定.png", {.finishWait=1})}}
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
                    {.finishUntilList={new Image("英雄中心/英雄列表.png"), new IfImage("英雄中心/确定.png")}, .finishWait=1}
            );

            if (!clicker->founded()) continue;

            clicker->click({.finishUntilList={new Image("英雄中心/确定.png", InnerReverse)}})->end();
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
            {.runUntilList={new Image("战争学院/战争学院标题.png")}}
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