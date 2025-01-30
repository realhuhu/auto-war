#include "daily.h"

void heroCenter() {
    auto config = parseBoolConfig("英雄中心", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/英雄中心/英雄.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/英雄中心.png"));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (config["免费抽奖"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/英雄中心/英雄免费.png");

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/英雄抽奖.png", Previous::TOP_CENTER));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilIfImage>("/英雄中心/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/确定.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
            }
        }
    }

    if (config["英雄训练"]) {
        clicker = std::make_unique<ImageClicker>("/英雄中心/英雄培养.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/英雄列表.png"));
        clicker->click(startUntil, clickUntil, runUntil);

        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/英雄中心/选择英雄.png");

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/英雄中心/星星亮.png", "/英雄中心/星星暗.png"
            }));
            clicker = clicker->click(startUntil, clickUntil, runUntil, positionSelector("yCenter", "min"));

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/开始训练.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/英雄列表.png"));
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);
        }
    }

    clicker = std::make_unique<ImageClicker>("/英雄中心/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/英雄中心/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void warCenter() {
    auto boolConfig = parseBoolConfig("战争学院", "checkbox", state.config);
    auto stringConfig = parseStringConfig("战争学院", "select", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/战争学院/战争学院.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/战争学院标题.png"));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (boolConfig["技能训练"]) {
        clicker = std::make_unique<ImageClicker>("/战争学院/训练.png");

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/次数不足.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil, randomSelector, 0.5);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png"));
        clicker = clicker->locate(startUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "min"));
    }

    clicker = std::make_unique<ImageClicker>("/战争学院/坦克研究.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/改装说明.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    if (boolConfig["坦克改装"]) {
        auto refitTank = stringConfig["改造"];
        auto refitType = stringConfig["改造项目"];

        if (refitTank != "天启") {
            clicker = std::make_unique<ImageClicker>("/战争学院/" + refitTank + ".png");

            clearUntil(startUntil, clickUntil, runUntil);
            clicker->click();
        }

        clicker = std::make_unique<ImageClicker>("/战争学院/改装" + refitType + "图片.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/" + refitType + "提示.png"));
        clicker->click(startUntil, clickUntil, runUntil);

        clicker = std::make_unique<ImageClicker>("/战争学院/确定研究完成.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/确定研究完成.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }

        clicker = std::make_unique<ImageClicker>("/战争学院/改装.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/战争学院/材料不足.png", "/战争学院/正在工作.png", "/战争学院/加速.png"
            }));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            if (clicker->templatePath == "/战争学院/材料不足.png") {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png"));
                clicker = clicker->locate(startUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "min"));
            } else if (clicker->templatePath == "/战争学院/正在工作.png") {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/正在工作.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
            }
        }
    }

    if (boolConfig["坦克干扰"]) {
        auto jamTank = stringConfig["干扰"];
        auto jamType = stringConfig["干扰项目"];

        clicker = std::make_unique<ImageClicker>("/战争学院/坦克干扰.png");
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/干扰说明.png"));
        clicker->click(startUntil, clickUntil, runUntil);


        if (jamTank != "天启") {
            clicker = std::make_unique<ImageClicker>("/战争学院/" + jamTank + ".png");

            clearUntil(startUntil, clickUntil, runUntil);
            clicker->click();
        }

        clicker = std::make_unique<ImageClicker>("/战争学院/干扰" + jamType + "图片.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/" + jamType + "提示.png"));
        clicker->click(startUntil, clickUntil, runUntil);

        clicker = std::make_unique<ImageClicker>("/战争学院/确定研究完成.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/确定研究完成.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }

        clicker = std::make_unique<ImageClicker>("/战争学院/干扰.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/战争学院/材料不足.png", "/战争学院/正在工作.png", "/战争学院/加速.png"
            }));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            if (clicker->templatePath == "/战争学院/材料不足.png") {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png"));
                clicker = clicker->locate(startUntil, runUntil, positionSelector("xCenter", "min"));

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
            } else if (clicker->templatePath == "/战争学院/正在工作.png") {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/正在工作.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
            }
        }
    }

    if (boolConfig["军事演习"]) {
        clicker = std::make_unique<ImageClicker>("/战争学院/军事演习.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/战争学院/空演习场.png", "/战争学院/演习中.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/战争学院/空演习场.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/占领演习场.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/演习中.png"));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    clicker = std::make_unique<ImageClicker>("/战争学院/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/战争学院/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void countryChest() {
    auto config = parseBoolConfig("国家宝箱", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/国家宝箱/国家.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/国家宝箱图标.png"));
    clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/国家宝箱标题.png"));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (config["开一个战功宝箱"]) {
        clicker = std::make_unique<ImageClicker>("/国家宝箱/开启.png");

        clicker->click();
    }

    if (config["领取排名奖励"]) {
        clicker = std::make_unique<ImageClicker>("/国家宝箱/领取排名奖励.png");

        if (clicker->founded()) clicker->click();
    }

    if (config["领取战功奖励"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/国家宝箱/战功进度宝箱.png", 0.5, 0.9, 60, Mode::RGB);

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/战功进度宝箱.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    if (config["领取月卡宝箱"]) {
        clicker = std::make_unique<ImageClicker>("/国家宝箱/月卡宝箱.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/确定.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    clicker = std::make_unique<ImageClicker>("/国家宝箱/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/关闭窗口.png", Previous::INNER, true));
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/关闭窗口.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家宝箱/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void guild() {
    auto config = parseBoolConfig("公会领奖", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    if (config["领取公会战奖励"]) {
        clicker = std::make_unique<ImageClicker>("/公会领奖/参加公会战.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/公会战争标题.png"));
            clicker->click(startUntil, clickUntil, runUntil);

            clicker = std::make_unique<ImageClicker>("/公会领奖/报名公会战.png");

            if (!clicker->founded()) {
                clicker = std::make_unique<ImageClicker>("/公会领奖/领取奖励.png");

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/领取奖励.png", Previous::RIGHT));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/确定.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/报名公会战.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);
            }

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/关闭公会战窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/公会.png"));
            clicker->click(startUntil, clickUntil, runUntil);
        }

    }

    clicker = std::make_unique<ImageClicker>("/公会领奖/公会.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/公会福利.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    if (config["领取活跃度奖励"]) {
        clicker = std::make_unique<ImageClicker>("/公会领奖/捐献.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/活跃任务.png", Previous::RIGHT));
        clicker = clicker->locate(startUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/活跃任务按钮.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/今日公会活跃.png"));
        clicker->click(startUntil, clickUntil, runUntil);

        for (const auto &i: std::vector<std::string>{"第一档", "第二档", "第三档", "第四档", "第五档"}) {
            clicker = std::make_unique<ImageClicker>("/公会领奖/" + i + ".png", 0, 0.95, 60, Mode::RGB);

            if (!clicker->founded()) continue;

            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/确定.png", Previous::NONE, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/关闭公会捐献窗口.png"));
            clicker->click(startUntil, clickUntil, runUntil);
        }

        clicker = std::make_unique<ImageClicker>("/公会领奖/关闭公会捐献窗口.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/关闭公会捐献窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
    }


    clicker = std::make_unique<ImageClicker>("/公会领奖/公会争霸.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/公会争霸标题.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    if (config["领取夺城战奖励"]) {
        clicker = std::make_unique<ImageClicker>("/公会领奖/领取夺城战奖励.png");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/领取夺城战奖励.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    if (config["领取公会战役宝箱"]) {
        clicker = std::make_unique<ImageClicker>("/公会领奖/公会战役.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/查看成员排名.png"));
        runUntil.emplace_back(std::make_unique<UntilIfImage>("/公会领奖/领取公会战役奖励.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 0.5);

            while (!state.stopFlag.load()) {
                clicker = std::make_unique<ImageClicker>("/公会领奖/确定.png");

                if (!clicker->founded()) break;

                clearUntil(startUntil, clickUntil, runUntil);
                clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);
            }

        }
    }

    clicker = std::make_unique<ImageClicker>("/公会领奖/关闭公会争霸窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会领奖/关闭公会争霸窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void admiral() {
    auto config = parseBoolConfig("将领抽奖", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/将领抽奖/将领.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/将领面板.png"));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (config["免费抽奖"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/将领抽奖/将领免费.png");

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/将领抽奖.png", Previous::TOP_CENTER));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 2);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/确定.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    if (config["免费学习书"]) {
        clicker = std::make_unique<ImageClicker>("/将领抽奖/将领技能.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/免费收集.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilIfImage>("/将领抽奖/次数已用尽.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/关闭窗口.png"));
            clicker = clicker->locate(startUntil, runUntil, positionSelector("xCenter", "min"));

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "min"));
        }
    }

    clicker = std::make_unique<ImageClicker>("/将领抽奖/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/将领抽奖/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void adviser() {
    auto config = parseBoolConfig("参谋抽奖", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/参谋抽奖/参谋.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/参谋抽奖/参谋面板.png", "/参谋抽奖/确定未开启.png"
    }));
    clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

    if (clicker->templatePath == "/参谋抽奖/确定未开启.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/参谋抽奖/确定未开启.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
        return;
    }

    if (config["免费抽奖"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>(
                    std::vector<std::string>{"/参谋抽奖/参谋免费.png", "/参谋抽奖/必得免费.png"}
            );

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/参谋抽奖/参谋抽奖.png", Previous::TOP_CENTER));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/参谋抽奖/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/参谋抽奖/确定.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }

    }

    if (config["免费技能书"]) {
        clicker = std::make_unique<ImageClicker>("/参谋抽奖/参谋技能.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/参谋抽奖/参谋列表.png"));
        runUntil.emplace_back(
                std::make_unique<UntilIfImage>("/参谋抽奖/免费收集.png", Previous::NONE, false, Mode::RGB));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(
                    std::make_unique<UntilImage>("/参谋抽奖/免费收集.png", Previous::NONE, true, Mode::RGB));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    clicker = std::make_unique<ImageClicker>("/参谋抽奖/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/参谋抽奖/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void mortar() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/火炮抽奖/远程部队.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/火炮抽奖/远程火炮.png", "/火炮抽奖/确定未开启.png"
    }));
    clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

    if (clicker->templatePath == "/火炮抽奖/确定未开启.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/火炮抽奖/确定未开启.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
        return;
    }

    for (const auto &i: std::vector<std::string>{"绿", "蓝", "紫"}) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/火炮抽奖/" + i + "色免费派遣.png", 0, 0.95, 60, Mode::RGB);

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/火炮抽奖/派遣.png", Previous::TOP_CENTER));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/火炮抽奖/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/火炮抽奖/确定.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);
        }
    }

    clicker = std::make_unique<ImageClicker>("/火炮抽奖/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/火炮抽奖/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);

}

void equipment() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/配件抽奖/配件中心.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/配件中心标题.png"));
    clicker->click(startUntil, clickUntil, runUntil, similaritySelector);

    clicker = std::make_unique<ImageClicker>("/配件抽奖/野外.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/野外场景.png"));
    runUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/免费探索.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);


    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/次数用尽.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/次数用尽.png", Previous::INNER, "true"));
    clicker->click(startUntil, clickUntil, runUntil);

    for (const auto &i: std::vector<std::string>{"工厂", "实验室"}) {
        clicker = std::make_unique<ImageClicker>("/配件抽奖/" + i + ".png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilIfImage>("/配件抽奖/免费探索次数.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

        if (!clicker->founded())continue;

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/免费探索.png", Previous::TOP));
        clicker = clicker->locate(startUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/免费探索.png", Previous::INNER, "true"));
        clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);
    }

    clicker = std::make_unique<ImageClicker>("/配件抽奖/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/配件抽奖/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void arms() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/军备抽奖/军备研究.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/军备抽奖/军备研究提示.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    for (const auto &i: std::vector<std::string>{"机械", "自动化"}) {
        clicker = std::make_unique<ImageClicker>("/军备抽奖/" + i + "制造厂.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/军备抽奖/" + i + "制造厂场景.png"));
        clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 0, 0, 0, Click::LEFT);

        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/军备抽奖/免费次数.png");

            if (!clicker->founded())break;

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备抽奖/免费制造.png", Previous::TOP_CENTER));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);
        }
    }

    clicker = std::make_unique<ImageClicker>("/军备抽奖/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/军备抽奖/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void dailyTask() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/每日任务/每日任务.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/每日任务/奖励.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    for (const auto &i: std::vector<std::string>{"1", "3", "5", "8", "10"}) {
        clicker = std::make_unique<ImageClicker>("/每日任务/" + i + "0活跃度.png", 0, 0.9, 60, Mode::RGB);

        if (!clicker->founded())continue;

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilIfImage>("/每日任务/领取活跃度奖励.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1);

        if (!clicker->founded())continue;

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/每日任务/领取活跃度奖励.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
    }

    clicker = std::make_unique<ImageClicker>("/每日任务/每日激战.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/每日任务/领取积分奖励.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/每日任务/领取积分奖励.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/每日任务/关闭窗口.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/每日任务/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void weeklyTask() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/周任务/周任务.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/周任务/周目标标题.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/周任务/领取.png", 0, 0.9, 60, Mode::RGB);

        if (!clicker->founded()) break;

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/周任务/领取.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
    }

    clicker = std::make_unique<ImageClicker>("/周任务/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/周任务/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void signIn() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/每日签到/每日签到.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/每日签到/签到.png", "/每日签到/已签到.png"
    }, Previous::NONE, false, Mode::RGB));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    if (clicker->templatePath == "/每日签到/签到.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/每日签到/签到.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
    }

    clicker = std::make_unique<ImageClicker>("/每日签到/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/每日签到/关闭窗口.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);
}

void oreField() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/矿区争夺/矿区争夺战.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/矿区争夺.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    clicker = std::make_unique<ImageClicker>("/矿区争夺/撤军.png");

    if (clicker->founded()) {
        emit Emitter::instance()->log("已有占领的矿区", "red");

        clicker = std::make_unique<ImageClicker>("/矿区争夺/关闭窗口.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);
        return;
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/矿区争夺/刷新次数.png");

        if (!clicker->founded()) {
            emit Emitter::instance()->log("免费次数已用完", "red");

            clicker = std::make_unique<ImageClicker>("/矿区争夺/关闭窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);

            return;
        }

        clicker = std::make_unique<ImageClicker>("/矿区争夺/刷新次数.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/矿区争夺/确定.png", "/矿区争夺/占领.png", "/矿区争夺/抢占.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "max"), 0, 1, 0, -26);

        if (clicker->templatePath == "/矿区争夺/确定.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/确定.png", Previous::INNER, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);

            return;
        }

        if (clicker->templatePath == "/矿区争夺/占领.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/撤军.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png", Previous::TOP));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);

            return;
        }

        auto ps = CV::findPositions(CV::getScreen(), "/矿区争夺/抢占.png");

        for (const auto &p: ps) {
            clicker = std::make_unique<ImageClicker>("/矿区争夺/抢占.png", p);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/跳过.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/矿区争夺.png"));
            clicker->click(startUntil, clickUntil, runUntil);

            clicker = std::make_unique<ImageClicker>("/矿区争夺/撤军.png");

            if (clicker->founded()) {
                clicker = std::make_unique<ImageClicker>("/矿区争夺/关闭窗口.png");

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/矿区争夺/关闭窗口.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
                return;
            }
        }
    }
}

void monthlyCard() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/月卡领取/畅玩月卡.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/月卡领取/一键领取.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clicker->click();

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/月卡领取/关闭窗口.png");

        if (!clicker->founded()) break;

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/月卡领取/关闭窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "min"));
    }
}

void guildBuilding() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;
    clicker = std::make_unique<ImageClicker>(
            std::vector<std::string>{"/公会建筑/公会建筑小.png", "/公会建筑/公会建筑大.png"});

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/公会建筑/展开按钮.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/场景缩放.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil, similaritySelector, 0, 0, 0, 0, Click::RIGHT);

        clicker->clickIfFound(startUntil, runUntil, clickUntil, similaritySelector, 0, 1);

        clicker = std::make_unique<ImageClicker>("/公会建筑/公会建筑小.png");

        if (clicker->founded()) {
            emit Emitter::instance()->log("无法找到公会建筑!", "red");
            return;
        }
    }


    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/打开公会建筑.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/公会建筑标题.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/团体任务.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/参与任务.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/勋章刷新.png", Previous::NONE, true));
    clicker->click(startUntil, clickUntil, runUntil);

    auto iterator = CV::findPositions(CV::getScreen(), "/公会建筑/需要人数.png");

    auto it = iterator.begin();

    while (it != iterator.end() and !state.stopFlag.load()) {
        it->click();

        clicker = std::make_unique<ImageClicker>("/公会建筑/领取奖励.png", 0, 0.9, 60, Mode::RGB);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/确定领取.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/确定领取.png", Previous::INNER, true));
        clicker->clickIfFound(startUntil, clickUntil, runUntil);

        ++it;
    }

    clicker = std::make_unique<ImageClicker>("/公会建筑/关闭窗口.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会建筑/关闭窗口.png", Previous::INNER, "true"));
    clicker->click(startUntil, clickUntil, runUntil);
}
