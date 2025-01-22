#include "battle.h"

#include "common.h"
#include "../state.h"
#include "../flow/runner.h"

void countryArena() {
    auto config = parseIntConfig("国家争霸", "input", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/国家争霸/争霸战图标.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/国家争霸/争霸战标题.png", "/国家争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (clicker->templatePath == "/国家争霸/未开启.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png", "inner", true));
        clicker->click(startUntil, clickUntil, runUntil);

        return;
    }

    clicker = std::make_unique<ImageClicker>("/国家争霸/领取积分.png", 0, 0.9, 60, "rgb");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/领取积分.png", "inner", true, "rgb"));
    clicker->clickIfFound(startUntil, clickUntil, runUntil);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/国家争霸/攻击.png");

        clearUntil(startUntil, clickUntil, runUntil);
        startUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/可攻击.png"));
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/国家争霸/跳过战斗.png", "/国家争霸/购买次数.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil,
                                 orderedRandomSelector("yCenter", "min", config["打前"]));

        if (clicker->templatePath == "/国家争霸/购买次数.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png", "top"));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/购买次数.png", "none", true));
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
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家争霸/确认失败.png", "none", true));
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
    runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/世界争霸/争霸战标题.png", "/世界争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

    if (clicker->templatePath == "/世界争霸/未开启.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png", "inner", true));
        clicker->click(startUntil, clickUntil, runUntil);

        return;
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/世界争霸/攻击.png");

        clearUntil(startUntil, clickUntil, runUntil);
        startUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/可攻击.png"));
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/世界争霸/跳过战斗.png", "/世界争霸/购买次数.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil,
                                 orderedRandomSelector("yCenter", "min", config["打前"]));

        if (clicker->templatePath == "/世界争霸/购买次数.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png", "top"));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/购买次数.png", "none", true));
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
        runUntil.emplace_back(std::make_unique<UntilImage>("/世界争霸/确认失败.png", "none", true));
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
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/剿灭将领/剿灭将领标题.png", "/剿灭将领/等级不足.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/剿灭将领/等级不足.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定关闭.png", "down"));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定关闭.png", "inner", true));
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
        clicker = std::make_unique<ImageClicker>("/剿灭将领/合成.png", 0, 0.95, 60, "rgb");

        if (clicker->founded()) {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定奖励.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定奖励.png", "inner", true, "gray", 1));
            clicker->click(startUntil, clickUntil, runUntil);
        }

        clicker = std::make_unique<ImageClicker>("/剿灭将领/信物商店.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilIfAnyImage>(chosen, "right", false, "rgb", 0, 0.95));
        clicker = clicker->locate(startUntil, runUntil);

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/剿灭将领/刷新.png");

            clearUntil(startUntil, clickUntil, runUntil);
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 0.5);

            clicker = std::make_unique<ImageClicker>("/剿灭将领/确定刷新.png");

            if (clicker->founded()) {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定刷新.png", "inner", true));
                clicker->click(startUntil, clickUntil, runUntil);
            }
            continue;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/战斗进行中.png", "none", true));
        runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/进攻.png", "down_center"));
        clicker = clicker->locate(startUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/剿灭将领/确认胜利.png", "/剿灭将领/次数不足.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/剿灭将领/次数不足.png") {
            clicker = std::make_unique<ImageClicker>("/军备合成/关闭窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "inner", true));
            clicker = clicker->click(startUntil, clickUntil, runUntil, positionSelector("yCenter", "max"));

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "inner", true));
            clicker->click(startUntil, clickUntil, runUntil);
            break;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/剿灭将领/信物商店.png"));
        clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 3);
    }
}

void armsCompound() {
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
        runUntil.emplace_back(std::make_unique<UntilImage>(choice<std::string>({
                                                                                       "/军备合成/碎甲弹.png",
                                                                                       "/军备合成/高爆弹.png",
                                                                                       "/军备合成/电磁炮.png",
                                                                                       "/军备合成/破甲弹.png",
                                                                                       "/军备合成/陶瓷复合装甲.png"
                                                                               })));
        clicker = clicker->clickIfFound(startUntil, clickUntil, runUntil);

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
        clickUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/军备合成/跳过战斗.png", "/军备合成/次数不足.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/军备合成/次数不足.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/次数不足.png", "inner", true));
            clickUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png", "inner", true));
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png", "inner", true));
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
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

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
        runUntil.emplace_back(std::make_unique<UntilIfAnyImage>(std::initializer_list<const std::string>{
                "/国家战争/纽约.png", "/国家战争/芝加哥.png", "/国家战争/波尔多.png",
                "/国家战争/马赛.png", "/国家战争/慕尼黑.png", "/国家战争/法兰克福.png",
                "/国家战争/列宁格勒.png", "/国家战争/叶卡捷琳堡.png", "/国家战争/都灵.png",
                "/国家战争/米兰.png", "/国家战争/伯明翰.png", "/国家战争/曼彻斯特.png"
        }, "right"));
        clicker = clicker->locate(startUntil, runUntil);

        if (!clicker->founded()) {
            emit Emitter::instance()->log("请先移动到与摩多城相邻的城", "red");
            break;
        }
        auto city = std::filesystem::path(clicker->templatePath).stem().string();
        emit Emitter::instance()->log(QString::fromStdString("当前所在: " + city), "blue");

        clicker = std::make_unique<ImageClicker>("/国家战争/前往.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImageStable>("/国家战争/" + city + "城.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        int offsetX = 0;
        int offsetY = 0;
        if (city == "芝加哥") {
            offsetX = 87;
            offsetY = -154;
        } else if (city == "纽约") {
            offsetX = -136;
            offsetY = -120;
        } else if (city == "米兰") {
            offsetX = 100;
            offsetY = -133;
        } else if (city == "都灵") {
            offsetX = -167;
            offsetY = -81;
        } else if (city == "波尔多") {
            offsetX = -211;
            offsetY = -89;
        } else if (city == "马赛") {
            offsetX = -29;
            offsetY = 46;
        } else if (city == "列宁格勒") {
            offsetX = -126;
            offsetY = -37;
        } else if (city == "叶卡捷琳堡") {
            offsetX = 280;
            offsetY = -47;
        } else if (city == "法兰克福") {
            offsetX = 163;
            offsetY = 61;
        } else if (city == "慕尼黑") {
            offsetX = 159;
            offsetY = 20;
        } else if (city == "伯明翰") {
            offsetX = 159;
            offsetY = 54;
        } else if (city == "曼彻斯特") {
            offsetX = 89;
            offsetY = -206;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/城市信息.png"));
        clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 0, 1, offsetX, offsetY);

        clicker = std::make_unique<ImageClicker>("/国家战争/扫荡.png");

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/国家战争/召唤支援兵.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/国家战争/去购买.png", "/国家战争/确定兑换.png"
            }));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            if (clicker->templatePath == "/国家战争/去购买.png") {
                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/兑换.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定兑换.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定兑换.png", "inner", true));
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/召唤支援兵.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil, similaritySelector);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定兑换.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);
            }

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/扫荡.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);
        }

        clearUntil(startUntil, clickUntil, runUntil);
        startUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/可战斗.png", "none", false, "rgb", 0.6));
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/国家战争/恢复行动力.png", "/国家战争/跳过战斗.png", "/国家战争/请稍后.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/国家战争/恢复行动力.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", "right"));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", "inner", true));
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

        if (clicker->templatePath == "/国家战争/请稍后.png") {
            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/确定稍后.png", "down"));
            clicker = clicker->locate(startUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/扫荡.png"));
            clicker = clicker->click(startUntil, clickUntil, runUntil);

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/跳过战斗.png"));
            clicker->click(startUntil, clickUntil, runUntil, similaritySelector, 2);
        }

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/结束战斗.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/结束战斗.png", "inner", true));
        runUntil.emplace_back(std::make_unique<UntilImage>("/国家战争/前往.png"));
        clicker->click(startUntil, clickUntil, runUntil);
    }
}
