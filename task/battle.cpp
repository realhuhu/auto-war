#include "battle.h"


void countryArena() {
    auto config = parseIntConfig("国家争霸", "input", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/国家争霸/争霸战图标.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
            "/国家争霸/争霸战标题.png", "/国家争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (clicker->templatePath == "/国家争霸/未开启.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);

        return;
    }

    clicker = std::make_unique<ImageClicker>("/国家争霸/领取积分.png", 0, 0.9, 60, Mode::RGB);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/领取积分.png", Previous::INNER, true, Mode::RGB));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/国家争霸/攻击.png");

        clearUntil(startUntil, clickUntil, runUntil);
        startUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/可攻击.png"));
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/国家争霸/跳过战斗.png", "/国家争霸/购买次数.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil,
                                 orderedRandomSelector("yCenter", "min", config["打前"]));

        if (clicker->templatePath == "/国家争霸/购买次数.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png", Previous::TOP));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/购买次数.png", Previous::NONE, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "min"));

            clicker->click();
            break;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/结束战斗.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/争霸战标题.png"));
        clicker->click(startUntil, clickUntil, runUntil);

        clicker = std::make_unique<ImageClicker>("/国家争霸/确认失败.png", 1);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/确认失败.png", Previous::NONE, true));
        clicker->clickIfFound(startUntil, clickUntil, runUntil, similaritySelector);
    }
}

void worldArena() {
    auto config = parseIntConfig("世界争霸", "input", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/世界争霸/争霸战图标.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
            "/世界争霸/争霸战标题.png", "/世界争霸/未开启.png", "/世界争霸/未参加.png"
    }));
    clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (clicker->templatePath == "/世界争霸/未开启.png" || clicker->templatePath == "/世界争霸/未参加.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);

        return;
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/世界争霸/攻击.png");

        clearUntil(startUntil, clickUntil, runUntil);
        startUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/可攻击.png"));
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/世界争霸/跳过战斗.png", "/世界争霸/购买次数.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil,
                                 orderedRandomSelector("yCenter", "min", config["打前"]));

        if (clicker->templatePath == "/世界争霸/购买次数.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png", Previous::TOP));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/购买次数.png", Previous::NONE, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "min"));

            clicker->click();
            break;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/结束战斗.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/争霸战标题.png"));
        clicker->click(startUntil, clickUntil, runUntil);

        clicker = std::make_unique<ImageClicker>("/世界争霸/确认失败.png", 1);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/确认失败.png", Previous::NONE, true));
        clicker->clickIfFound(startUntil, clickUntil, runUntil, similaritySelector);
    }
}

void exterminateEnemy() {
    auto config = parseBoolConfig("剿灭将领", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/剿灭将领/剿灭将领标题.png");

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/剿灭将领/剿灭将领.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/剿灭将领/剿灭将领标题.png", "/剿灭将领/等级不足.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/剿灭将领/等级不足.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定.png", Previous::DOWN));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
            return;
        }
    }

    std::vector<std::string> chosen;

    for (const auto &pair: config) {
        if (pair.second) {
            chosen.emplace_back("/剿灭将领/" + pair.first + ".png");
        }
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/剿灭将领/合成.png", 0, 0.95, 60, Mode::RGB);

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(
                    std::make_unique<UntilImage>("/剿灭将领/确定.png", Previous::INNER, true, Mode::GRAY, 1));
            clicker->click(startUntil, clickUntil, runUntil);
        }

        clicker = std::make_unique<ImageClicker>("/剿灭将领/信物商店.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilIfAnyImage>(chosen, Previous::RIGHT, false, Mode::RGB, 0, 0.95));
        clicker = clicker->locate(startUntil, runUntil);

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/剿灭将领/刷新.png");

            clearUntil(startUntil, clickUntil, runUntil);
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 0.5);

            clicker = std::make_unique<ImageClicker>("/剿灭将领/确定.png");

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
            }
            continue;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/战斗进行中.png", Previous::NONE, true));
        runUntil.emplace_back(
                std::make_unique<UntilImage>("/剿灭将领/进攻.png", Previous::DOWN_CENTER, false, Mode::GRAY, 1));
        clicker = clicker->locate(startUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/剿灭将领/确定.png", "/剿灭将领/次数不足.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/剿灭将领/次数不足.png") {
            clicker = std::make_unique<ImageClicker>("/军备合成/关闭窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", Previous::INNER, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil, positionSelector("yCenter", "max"));

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
            break;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/信物商店.png"));
        clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 3);
    }
}

void armsCompound() {
    auto config = parseBoolConfig("军备合成", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;


    clicker = std::make_unique<ImageClicker>("/军备合成/合成军备.png");

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/军备合成/军备研究图标.png");

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/军备合成.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/选择品质.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/橙色品质.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/合成军备.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/军备合成/滚动条A.png", "/军备合成/滚动条B.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        std::vector<std::string> candidates;
        for (const auto &[key, value]: config) {
            if (value) candidates.push_back(key);
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/" + choice(candidates) + ".png"));
        clicker = clicker->drag(startUntil, clickUntil);


        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/合成军备.png"));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil, similaritySelector, 0, 0.5);
    }

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/获取碎片.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/开始战斗.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/军备合成/开始战斗.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/军备合成/跳过战斗.png", "/军备合成/次数不足.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/军备合成/次数不足.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/次数不足.png", Previous::INNER, true));
            clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png", Previous::INNER, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);

            break;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/结束战斗.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/开始战斗.png"));
        clicker->click(startUntil, clickUntil, runUntil);
    }
}

void countryWar() {
    auto config = parseBoolConfig("国家战争", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    if (config["8点时自动领体力"]) {
        std::time_t now = std::time(nullptr);
        std::tm *local_time = std::localtime(&now);

        int hour = local_time->tm_hour;

        // 判断是否在晚上8点到9点之间
        if ((hour >= 20 && hour < 21)) {
            clicker = std::make_unique<ImageClicker>("/国家战争/返回基地.png");

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/活动及公告.png"));
                clicker->click(startUntil, clickUntil, runUntil);
            }

            clicker = std::make_unique<ImageClicker>("/国家战争/vip福利礼包.png");

            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/vip福利礼包标题.png"));
            clicker->click(startUntil, clickUntil, runUntil);

            clicker = std::make_unique<ImageClicker>("/国家战争/点击领取.png", 0, 0.95, 60, Mode::RGB);

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                        "/国家战争/未购买vip月卡.png", "/国家战争/已成功领取.png"
                }));
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil, positionSelector("yCenter", "max"));
            }

            clicker = std::make_unique<ImageClicker>("/国家战争/每周折扣礼包.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/vip恢复卡.png"));
            clicker->click(startUntil, clickUntil, runUntil);

            clicker = std::make_unique<ImageClicker>("/国家战争/免费领取.png", 0, 0.95, 60, Mode::RGB);

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                        "/国家战争/未购买vip月卡.png", "/国家战争/已成功领取.png"
                }));
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil, positionSelector("yCenter", "max"));
            }

            clicker = std::make_unique<ImageClicker>("/国家战争/关闭窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);


            clicker = std::make_unique<ImageClicker>("/国家战争/活动及公告.png");

            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/连续登录.png"));
            clicker->click(startUntil, clickUntil, runUntil);

            clicker = std::make_unique<ImageClicker>("/国家战争/领取连续登录奖励.png", 2, 0.9, 60, Mode::RGB);

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>(
                        "/国家战争/领取连续登录奖励.png",
                        Previous::INNER,
                        true,
                        Mode::RGB));
                clicker->click(startUntil, clickUntil, runUntil);
            }

            clicker = std::make_unique<ImageClicker>("/国家战争/关闭窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
        }
    }

    clicker = std::make_unique<ImageClicker>("/国家战争/国家.png");

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/世界地图.png"));
    clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/前往.png"));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/国家战争/当前所在.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilIfAnyImage>(list{
                "/国家战争/纽约.png", "/国家战争/芝加哥.png", "/国家战争/波尔多.png",
                "/国家战争/马赛.png", "/国家战争/慕尼黑.png", "/国家战争/法兰克福.png",
                "/国家战争/列宁格勒.png", "/国家战争/叶卡捷琳堡.png", "/国家战争/都灵.png",
                "/国家战争/米兰.png", "/国家战争/伯明翰.png", "/国家战争/曼彻斯特.png"
        }, Previous::RIGHT_CENTER));
        clicker = clicker->locate(startUntil, runUntil);

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/国家战争/返回基地.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/国家.png"));
            clicker->click(startUntil, clickUntil, runUntil);
            throw std::runtime_error("请先移动到与摩多城相邻的城");
        }

        auto city = QFileInfo(QString::fromStdString(clicker->templatePath)).baseName();
        emit Emitter::instance()->log(QString::fromStdString("当前所在: " + city.toStdString()), "blue");

        clicker = std::make_unique<ImageClicker>("/国家战争/前往.png");

        int offsetX = 0;
        int offsetY = 0;
        std::string anchor;
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


        auto cityPath = "/国家战争/" + anchor + ".png";
        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>(cityPath));
        runUntil.emplace_back(std::make_unique<UntilImageStable>(cityPath,
                                                                 Previous::NONE,
                                                                 false,
                                                                 Mode::GRAY,
                                                                 0,
                                                                 0.9,
                                                                 0.5));
        clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0.5);

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/城市信息.png"));
        clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1.5, offsetX, offsetY);

        clicker = std::make_unique<ImageClicker>("/国家战争/扫荡.png");

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/国家战争/召唤支援兵.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                    "/国家战争/去购买.png", "/国家战争/确定.png"
            }));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            if (clicker->templatePath == "/国家战争/去购买.png") {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/兑换.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定.png", Previous::INNER, true));
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/召唤支援兵.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 0.5);
            }

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/扫荡.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);
        }

        clearUntil(startUntil, clickUntil, runUntil);
        startUntil.emplace_back(
                std::make_unique<UntilImage>("/国家战争/可战斗.png", Previous::NONE, false, Mode::RGB, 0.6));
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/国家战争/恢复行动力.png", "/国家战争/跳过战斗.png", "/国家战争/确定.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/国家战争/恢复行动力.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", Previous::RIGHT));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", Previous::INNER, true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/领取图纸.png"));
            clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/返回基地.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/国家.png"));
            clicker->click(startUntil, clickUntil, runUntil);
            break;
        }

        if (clicker->templatePath == "/国家战争/确定.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
            continue;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/结束战斗.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/结束战斗.png", Previous::INNER, true));
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/前往.png"));
        clicker->click(startUntil, clickUntil, runUntil);
    }
}

void loopCountryWar() {
    auto boolConfig = parseBoolConfig("开卡国战", "checkbox", state.config);
    auto intConfig = parseIntConfig("开卡国战", "input", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    auto current = intConfig["开卡数"];
    auto rounds = intConfig["开卡数"];

    std::vector<std::string> candidates;
    if (boolConfig["行动力恢复卡"]) candidates.emplace_back("/开卡国战/行动力恢复卡.png");
    if (boolConfig["国战恢复卡"]) candidates.emplace_back("/开卡国战/国战恢复卡.png");

    while (!state.stopFlag.load()) {
        countryWar();

        if (candidates.empty()) {
            emit Emitter::instance()->log("未设置开卡", "red");
            break;
        }

        if (intConfig["开卡数"] > 0) {
            if (current == 0) {
                emit Emitter::instance()->log("开卡数已达到目标", "blue");
                return;
            }

            emit Emitter::instance()->log(QString("轮次: %1/%2").arg(
                    QString::number(rounds - current + 1),
                    QString::number(rounds)), "blue");
        }

        clicker = std::make_unique<ImageClicker>("/开卡国战/背包.png");

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/开卡国战/滚动条A.png", "/开卡国战/滚动条B.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);


        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilAnyImage>(candidates));
        clicker = clicker->drag(startUntil, clickUntil);

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/开卡国战/关闭窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/开卡国战/关闭窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
            emit Emitter::instance()->log("恢复卡不足", "red");
            break;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/开卡国战/使用.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/开卡国战/使用.png", Previous::INNER, true));
        runUntil.emplace_back(std::make_unique<UntilImage>("/开卡国战/关闭窗口.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/开卡国战/关闭窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);

        current--;
    }
}