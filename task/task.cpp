// task.cpp
#include "task.h"
#include <algorithm>
#include <stdexcept>
#include <QDebug>
#include <QCoreApplication>


#include "../state.h"
#include "../flow/cv.h"
#include "../flow/runner.h"

void clear_until(
        std::vector<std::unique_ptr<Until>> &start_until,
        std::vector<std::unique_ptr<Until>> &click_until,
        std::vector<std::unique_ptr<Until>> &run_until
) {
    start_until.clear();
    click_until.clear();
    run_until.clear();
}

void guild_war() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/公会战报名/参加公会战.png"));

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会战报名/领取奖励.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会战报名/领取奖励.png", "right"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会战报名/确定领取.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会战报名/报名公会战.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会战报名/确定报名.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会战报名/关闭窗口.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clicker->clickIfFound();
}

void country_war() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国战/国家.png"));

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/国战/世界地图.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/前往.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国战/前往.png"));

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImageStable>("/res/国战/摩多军团.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImageStable>("/res/国战/摩多军团旗帜.png"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/res/国战/城市信息.png"));
        clicker->click(start_until, click_until, run_until, similarity_selector, -1, 0, 1);

        if (CV::find_positions(CV::get_screen(), "/res/国战/游牧民族.png").empty()) {
            clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国战/召唤支援兵.png"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/res/国战/去购买.png", "/res/国战/确定兑换.png"
            }));
            clicker = clicker->click(start_until, click_until, run_until);

            if (clicker->templatePath == "/res/国战/去购买.png") {
                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/兑换.png"));
                clicker = clicker->click(start_until, click_until, run_until);

                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/确定兑换.png"));
                clicker = clicker->click(start_until, click_until, run_until);

                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/召唤支援兵.png"));
                clicker = clicker->click(start_until, click_until, run_until);

                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/确定兑换.png"));
                clicker->click(start_until, click_until, run_until);
            }

            clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国战/确定兑换.png"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/扫荡.png"));
            clicker->click(start_until, click_until, run_until);
        }

        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国战/扫荡.png"));

        clear_until(start_until, click_until, run_until);
        start_until.emplace_back(std::make_unique<UntilImage>("/res/国战/可战斗.png"));
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/国战/恢复行动力.png", "/res/国战/跳过战斗.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/res/国战/恢复行动力.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/关闭窗口.png", "right"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/关闭窗口.png", "inner", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/领取图纸.png"));
            clicker = clicker->clickIfFound(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/返回基地.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clicker->click();

            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/res/国战/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/国战/前往.png"));
        clicker->click(start_until, click_until, run_until);
    }
}

void country_arena() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国家争霸/争霸战图标.png"));

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/res/国家争霸/争霸战标题.png", "/res/国家争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    if (clicker->templatePath == "/res/国家争霸/未开启.png") {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/关闭窗口.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/关闭窗口.png", "inner", true));
        clicker->click(start_until, click_until, run_until);

        return;
    }

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国家争霸/领取积分.png"));

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/领取积分.png", "inner", true));
    clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国家争霸/攻击.png"));

        clear_until(start_until, click_until, run_until);
        start_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/可攻击.png"));
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/国家争霸/跳过战斗.png", "/res/国家争霸/购买次数.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until, random_selector);

        if (clicker->templatePath == "/res/国家争霸/购买次数.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/关闭窗口.png", "right"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/购买次数.png", "none", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clicker->click();
            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/争霸战标题.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clicker->click();

        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/国家争霸/确认失败.png"));
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/国家争霸/确认失败.png", "none", true));
        clicker->clickIfFound(start_until, click_until, run_until);
    }
}

void world_arena() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/世界争霸/争霸战图标.png"));

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/res/世界争霸/争霸战标题.png", "/res/世界争霸/未开启.png"
    }));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    if (clicker->templatePath == "/res/世界争霸/未开启.png") {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/关闭窗口.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/关闭窗口.png", "inner", true));
        clicker->click(start_until, click_until, run_until);

        return;
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/世界争霸/攻击.png"));

        clear_until(start_until, click_until, run_until);
        start_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/可攻击.png"));
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/世界争霸/跳过战斗.png", "/res/世界争霸/购买次数.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until, random_selector);

        if (clicker->templatePath == "/res/世界争霸/购买次数.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/关闭窗口.png", "right"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/购买次数.png", "none", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clicker->click();
            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/争霸战标题.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clicker->click();

        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/世界争霸/确认失败.png"));
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/世界争霸/确认失败.png", "none", true));
        clicker->clickIfFound(start_until, click_until, run_until);
    }
}

void arms_compound() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/军备合成/军备研究图标.png"));

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/军备合成.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/选择品质.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/橙色品质.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/合成军备.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>(choice<std::string>({
                                                                                    "/res/军备合成/碎甲弹.png",
                                                                                    "/res/军备合成/高爆弹.png",
                                                                                    "/res/军备合成/电磁炮.png",
                                                                                    "/res/军备合成/破甲弹.png",
                                                                                    "/res/军备合成/陶瓷复合装甲.png"
                                                                            })));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/合成军备.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/获取碎片.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/开始战斗.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/军备合成/开始战斗.png"));

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/军备合成/跳过战斗.png", "/res/军备合成/次数不足.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/res/军备合成/次数不足.png") {
            clear_until(start_until, click_until, run_until);
            click_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/次数不足.png", "inner", true));
            click_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            click_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/关闭窗口.png", "inner", true));
            clicker->click(start_until, click_until, run_until);

            break;
        }

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/结束战斗.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/军备合成/开始战斗.png"));
        clicker->click(start_until, click_until, run_until);
    }
}

void exterminate_enemy() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>(ImageClicker("/res/剿灭将领/剿灭将领.png"));

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/res/剿灭将领/剿灭将领标题.png", "/res/剿灭将领/等级不足.png"
    }));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    if (clicker->templatePath == "/res/剿灭将领/等级不足.png") {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/确定.png", "down"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/确定.png", "inner", true));
        clicker->click(start_until, click_until, run_until);
        return;
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>(ImageClicker("/res/剿灭将领/信物商店.png"));

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/合成.png"));
        run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/战斗进行中.png", "none", true));

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/剿灭将领/紫色元.png", "/res/剿灭将领/紫色帜.png", "/res/剿灭将领/紫色番.png",
                "/res/剿灭将领/蓝色元.png", "/res/剿灭将领/蓝色帜.png", "/res/剿灭将领/蓝色番.png",
                "/res/剿灭将领/绿色元.png", "/res/剿灭将领/绿色帜.png", "/res/剿灭将领/绿色番.png"
        }, "right", false, "rgb"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/进攻.png", "down_center"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/剿灭将领/确认胜利.png", "/res/剿灭将领/已集齐.png", "/res/剿灭将领/次数不足.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/res/剿灭将领/已集齐.png" ||
            clicker->templatePath == "/res/剿灭将领/次数不足.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/关闭窗口.png", "right"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/合成.png"));
            clicker = clicker->click(start_until, click_until, run_until, position_selector("y_center", "max"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clicker->click();
            break;
        }

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/剿灭将领/信物商店.png"));
        clicker->click(start_until, click_until, run_until);
    }
}
