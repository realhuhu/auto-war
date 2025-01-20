// task.cpp
#include "battle.h"

#include "common.h"
#include "../state.h"
#include "../flow/runner.h"

void country_arena() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/国家争霸/争霸战图标.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/国家争霸/争霸战标题.png", "/国家争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    if (clicker->templatePath == "/国家争霸/未开启.png") {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png", "inner", true));
        clicker->click(start_until, click_until, run_until);

        return;
    }

    clicker = std::make_unique<ImageClicker>("/国家争霸/领取积分.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/领取积分.png", "inner", true));
    clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/国家争霸/攻击.png");

        clear_until(start_until, click_until, run_until);
        start_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/可攻击.png"));
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/国家争霸/跳过战斗.png", "/国家争霸/购买次数.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until, random_selector);

        if (clicker->templatePath == "/国家争霸/购买次数.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png", "top"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/购买次数.png", "none", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until, position_selector("x_center", "min"));

            clicker->click();
            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/争霸战标题.png"));
        clicker->click(start_until, click_until, run_until);

        clicker = std::make_unique<ImageClicker>("/国家争霸/确认失败.png", 1);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/国家争霸/确认失败.png", "none", true));
        clicker->clickIfFound(start_until, click_until, run_until, similarity_selector);
    }
}

void world_arena() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/世界争霸/争霸战图标.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/世界争霸/争霸战标题.png", "/世界争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    if (clicker->templatePath == "/世界争霸/未开启.png") {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png", "inner", true));
        clicker->click(start_until, click_until, run_until);

        return;
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/世界争霸/攻击.png");

        clear_until(start_until, click_until, run_until);
        start_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/可攻击.png"));
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/世界争霸/跳过战斗.png", "/世界争霸/购买次数.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until, random_selector);

        if (clicker->templatePath == "/世界争霸/购买次数.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png", "top"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/购买次数.png", "none", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until, position_selector("x_center", "min"));

            clicker->click();
            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/争霸战标题.png"));
        clicker->click(start_until, click_until, run_until);

        clicker = std::make_unique<ImageClicker>("/世界争霸/确认失败.png", 1);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/世界争霸/确认失败.png", "none", true));
        clicker->clickIfFound(start_until, click_until, run_until, similarity_selector);
    }
}

void exterminate_enemy() {
    auto config = parse("剿灭将领", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/剿灭将领/剿灭将领标题.png");

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/剿灭将领/剿灭将领.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/剿灭将领/剿灭将领标题.png", "/剿灭将领/等级不足.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/剿灭将领/等级不足.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定关闭.png", "down"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定关闭.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
            return;
        }
    }

    std::vector<std::string> chosen;

    // 遍历std::map
    for (const auto &pair: config) {
        if (pair.second) { // 如果值为true
            chosen.emplace_back("/剿灭将领/" + pair.first + ".png"); // 将键添加到trueKeys中
        }
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/剿灭将领/信物商店.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/合成.png"));
        run_until.emplace_back(std::make_unique<UntilIfAnyImage>(chosen, "right", false, "rgb"));
        clicker = clicker->locate(start_until, run_until);

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/剿灭将领/刷新.png");

            clear_until(start_until, click_until, run_until);
            click_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定刷新.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定刷新.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
            continue;
        }

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/战斗进行中.png", "none", true));
        run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/进攻.png", "down_center"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/剿灭将领/确认胜利.png", "/剿灭将领/已集齐.png", "/剿灭将领/次数不足.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/剿灭将领/次数不足.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "right"));
            clicker = clicker->locate(start_until, run_until, position_selector("y_center", "max"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "inner", true));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
            break;
        } else if (clicker->templatePath == "/剿灭将领/已集齐.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "right"));
            clicker = clicker->locate(start_until, run_until, position_selector("y_center", "max"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/关闭窗口.png", "inner", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/合成.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定奖励.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/确定奖励.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
            continue;
        }

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/剿灭将领/信物商店.png"));
        clicker->click(start_until, click_until, run_until);
    }
}

void arms_compound() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;


    clicker = std::make_unique<ImageClicker>("/军备合成/合成军备.png");

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/军备合成/军备研究图标.png");

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/军备合成/军备合成.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/选择品质.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/橙色品质.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/合成军备.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>(choice<std::string>({
                                                                                        "/军备合成/碎甲弹.png",
                                                                                        "/军备合成/高爆弹.png",
                                                                                        "/军备合成/电磁炮.png",
                                                                                        "/军备合成/破甲弹.png",
                                                                                        "/军备合成/陶瓷复合装甲.png"
                                                                                })));
        clicker = clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/合成军备.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until, similarity_selector, 0, 0.5);
    }

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/军备合成/获取碎片.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/开始战斗.png"));
    clicker->click(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/军备合成/开始战斗.png");

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/军备合成/跳过战斗.png", "/军备合成/次数不足.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/军备合成/次数不足.png") {
            clear_until(start_until, click_until, run_until);
            click_until.emplace_back(std::make_unique<UntilImage>("/军备合成/次数不足.png", "inner", true));
            click_until.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png", "inner", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/关闭窗口.png", "inner", true));
            clicker->click(start_until, click_until, run_until);

            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/军备合成/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/军备合成/开始战斗.png"));
        clicker->click(start_until, click_until, run_until);
    }
}

void country_war() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/国家战争/国家.png");

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/国家战争/世界地图.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/前往.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/国家战争/当前所在.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilIfAnyImage>(std::initializer_list<const std::string>{
                "/国家战争/纽约.png", "/国家战争/芝加哥.png", "/国家战争/波尔多.png",
                "/国家战争/马赛.png", "/国家战争/慕尼黑.png", "/国家战争/法兰克福.png",
                "/国家战争/列宁格勒.png", "/国家战争/叶卡捷琳堡.png", "/国家战争/都灵.png",
                "/国家战争/米兰.png", "/国家战争/伯明翰.png", "/国家战争/曼彻斯特.png"
        }, "right"));
        clicker = clicker->locate(start_until, run_until);

        if (!clicker->founded()) {
            emit Emitter::instance()->log("请先移动到与摩多城相邻的城", "red");
            break;
        }
        auto city = std::filesystem::path(clicker->templatePath).stem().string();
        emit Emitter::instance()->log(QString::fromStdString("当前所在: " + city), "blue");

        clicker = std::make_unique<ImageClicker>("/国家战争/前往.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImageStable>("/国家战争/" + city + "城.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        int offset_x = 0;
        int offset_y = 0;
        if (city == "芝加哥") {
            offset_x = 87;
            offset_y = -154;
        } else if (city == "纽约") {
            offset_x = -136;
            offset_y = -120;
        } else if (city == "米兰") {
            offset_x = 100;
            offset_y = -133;
        } else if (city == "都灵") {
            offset_x = -167;
            offset_y = -81;
        } else if (city == "波尔多") {
            offset_x = -211;
            offset_y = -89;
        } else if (city == "马赛") {
            offset_x = -29;
            offset_y = 46;
        } else if (city == "列宁格勒") {
            offset_x = -126;
            offset_y = -37;
        } else if (city == "叶卡捷琳堡") {
            offset_x = 280;
            offset_y = -47;
        } else if (city == "法兰克福") {
            offset_x = 163;
            offset_y = 61;
        } else if (city == "慕尼黑") {
            offset_x = 159;
            offset_y = 20;
        } else if (city == "伯明翰") {
            offset_x = 159;
            offset_y = 54;
        } else if (city == "曼彻斯特") {
            offset_x = 89;
            offset_y = -206;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/国家战争/城市信息.png"));
        clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1, offset_x, offset_y);

        clicker = std::make_unique<ImageClicker>("/国家战争/扫荡.png");

        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/国家战争/召唤支援兵.png");

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/国家战争/去购买.png", "/国家战争/确定兑换.png"
            }));
            clicker = clicker->click(start_until, click_until, run_until);

            if (clicker->templatePath == "/国家战争/去购买.png") {
                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/兑换.png"));
                clicker = clicker->click(start_until, click_until, run_until);

                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/确定兑换.png"));
                clicker = clicker->click(start_until, click_until, run_until);

                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/确定兑换.png", "inner", true));
                run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/召唤支援兵.png"));
                clicker = clicker->click(start_until, click_until, run_until, similarity_selector);

                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/确定兑换.png"));
                clicker = clicker->click(start_until, click_until, run_until);
            }

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/扫荡.png"));
            clicker = clicker->click(start_until, click_until, run_until);
        }

        clear_until(start_until, click_until, run_until);
        start_until.emplace_back(std::make_unique<UntilImage>("/国家战争/可战斗.png", "none", false, "rgb", 0.6));
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/国家战争/恢复行动力.png", "/国家战争/跳过战斗.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/国家战争/恢复行动力.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", "right"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/关闭窗口.png", "inner", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/领取图纸.png"));
            clicker = clicker->clickIfFound(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/返回基地.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/国家.png"));
            clicker->click(start_until, click_until, run_until);
            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/国家战争/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/国家战争/结束战斗.png", "inner", true));
        run_until.emplace_back(std::make_unique<UntilImage>("/国家战争/前往.png"));
        clicker->click(start_until, click_until, run_until);
    }
}
