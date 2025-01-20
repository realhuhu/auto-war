#include "daily.h"

#include "common.h"
#include "../state.h"
#include "../flow/cv.h"
#include "../flow/runner.h"
#include "../flow/emitter.h"

void hero_center() {
    auto config = parse("英雄中心", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/英雄中心/英雄.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/英雄中心.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    if (config["免费抽奖"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/英雄中心/英雄免费.png");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/英雄抽奖.png", "top_center"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilIfImage>("/res/英雄中心/确定.png"));
            clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

            if (clicker->founded()) {
                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/确定.png", "inner", true));
                clicker->click(start_until, click_until, run_until);
            }
        }
    }

    if (config["英雄训练"]) {
        clicker = std::make_unique<ImageClicker>("/res/英雄中心/英雄培养.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/英雄列表.png"));
        clicker->click(start_until, click_until, run_until);

        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/英雄中心/选择英雄.png");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/res/英雄中心/星星亮.png", "/res/英雄中心/星星暗.png"
            }));
            clicker = clicker->click(start_until, click_until, run_until, position_selector("y_center", "min"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/开始训练.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/英雄列表.png"));
            clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/英雄中心/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void war_center() {
    auto config = parse("战争学院", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/战争学院/战争学院.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/战争学院标题.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    if (config["技能训练"]) {
        clicker = std::make_unique<ImageClicker>("/res/战争学院/训练.png");

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/次数不足.png"));
        clicker = clicker->click(start_until, click_until, run_until, random_selector, 0.5);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/关闭窗口.png"));
        clicker = clicker->locate(start_until, run_until, position_selector("x_center", "min"));

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/关闭窗口.png", "inner", true));
        clicker->click(start_until, click_until, run_until);
    }

    if (config["军事演习"]) {
        clicker = std::make_unique<ImageClicker>("/res/战争学院/军事演习.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "/res/战争学院/空演习场.png", "/res/战争学院/演习中.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "/res/战争学院/空演习场.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/占领演习场.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/占领演习场.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/演习中.png"));
            clicker->click(start_until, click_until, run_until);
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/战争学院/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void country_chest() {
    auto config = parse("国家宝箱", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/国家宝箱/国家.png");

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/国家宝箱图标.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/国家宝箱标题.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    if (config["领取排名奖励"]) {
        clicker = std::make_unique<ImageClicker>("/res/国家宝箱/领取排名奖励.png");

        if (clicker->founded()) clicker->click();
    }

    if (config["领取战功奖励"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/国家宝箱/战功进度宝箱.png", 0.5, 0.9, 60, "rgb");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/战功进度宝箱.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
        }
    }

    if (config["领取月卡宝箱"]) {
        clicker = std::make_unique<ImageClicker>("/res/国家宝箱/月卡宝箱.png");

        if (clicker->founded()) {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                    "/res/国家宝箱/确定没有月卡.png" //TODO 有月卡时
            }));
            clicker = clicker->click(start_until, click_until, run_until);

            if (clicker->templatePath == "/res/国家宝箱/确定没有月卡.png") {
                clear_until(start_until, click_until, run_until);
                run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/确定没有月卡.png", "inner", true));
                clicker->click(start_until, click_until, run_until);
            }
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/国家宝箱/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/关闭窗口.png", "inner", true));
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/关闭窗口.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void guild() { //TODO
    auto config = parse("公会领奖", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    if (config["领取公会战奖励"]) {
        clicker = std::make_unique<ImageClicker>("/res/公会领奖/参加公会战.png");

        if (clicker->founded()) {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/领取奖励.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/领取奖励.png", "right"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/确定领取公会战奖励.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/报名公会战.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/确定报名.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/关闭公会战窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/公会.png"));
            clicker->click(start_until, click_until, run_until);
        }

    }

    clicker = std::make_unique<ImageClicker>("/res/公会领奖/公会.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/公会福利.png"));
    clicker->click(start_until, click_until, run_until);

    if (config["领取活跃度奖励"]) {
        clicker = std::make_unique<ImageClicker>("/res/公会领奖/活跃任务.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/活跃任务按钮.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/今日公会活跃.png"));
        clicker->click(start_until, click_until, run_until);

        for (const auto &i: std::vector<std::string>{"第一档", "第二档", "第三档", "第四档", "第五档"}) { //TODO
            clicker = std::make_unique<ImageClicker>("/res/公会领奖/" + i + ".png");

            if (!clicker->founded()) continue;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilIfAnyImage>(std::initializer_list<const std::string>{
                    "/res/公会领奖/确定领取活跃奖励.png", "/res/公会领奖/确定任务数不够.png"
            }));

            clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

            if (!clicker->founded()) continue;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/确定领取活跃奖励.png", "none", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/确定任务数不够.png", "none", true));
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/关闭公会捐献窗口.png"));
            clicker->click(start_until, click_until, run_until);
        }

        clicker = std::make_unique<ImageClicker>("/res/公会领奖/关闭公会捐献窗口.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/关闭公会捐献窗口.png", "inner", true));
        clicker->click(start_until, click_until, run_until);
    }


    clicker = std::make_unique<ImageClicker>("/res/公会领奖/公会争霸.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/公会争霸标题.png"));
    clicker->click(start_until, click_until, run_until);

    if (config["领取夺城战奖励"]) { //TODO
//        clear_until(start_until, click_until, run_until);
//        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
//                "/res/公会领奖/夺城战已领取.png"
//        }));
//        clicker = clicker->click(start_until, click_until, run_until);
//
//        if (clicker->templatePath == "/res/公会领奖/夺城战未领取.png") {
//
//        }
    }

    if (config["领取公会战役宝箱"]) {
        clicker = std::make_unique<ImageClicker>("/res/公会领奖/公会战役.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/查看成员排名.png"));
        run_until.emplace_back(std::make_unique<UntilIfImage>("/res/公会领奖/领取公会战役奖励.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->founded()) {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/领取公会战役奖励.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/公会领奖/关闭公会争霸窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会领奖/关闭公会争霸窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void admiral() {
    auto config = parse("将领抽奖", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/将领抽奖/将领.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/将领面板.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    if (config["免费抽奖"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/将领抽奖/将领免费.png");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/将领抽奖.png", "top_center"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/确定.png"));
            clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/确定.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
        }
    }

    if (config["免费学习书"]) {
        clicker = std::make_unique<ImageClicker>("/res/将领抽奖/将领技能.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/免费收集.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilIfImage>("/res/将领抽奖/次数已用尽.png"));
        clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

        if (clicker->founded()) {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/关闭窗口.png"));
            clicker = clicker->locate(start_until, run_until, position_selector("x_center", "min"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/关闭窗口.png", "inner", true));
            clicker->click(start_until, click_until, run_until, position_selector("x_center", "min"));
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/将领抽奖/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/将领抽奖/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void adviser() {
    auto config = parse("参谋抽奖", "checkbox", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/参谋抽奖/参谋.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/参谋面板.png"));
    clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);


    if (config["免费抽奖"]) {
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/参谋抽奖/参谋免费.png");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/参谋抽奖.png", "top_center"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/确定.png"));
            clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/确定.png", "inner", true));
            clicker->click(start_until, click_until, run_until);
        }

    }

    if (config["免费技能书"]) {
        clicker = std::make_unique<ImageClicker>("/res/参谋抽奖/参谋技能.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/参谋列表.png"));
        run_until.emplace_back(std::make_unique<UntilIfImage>("/res/参谋抽奖/免费收集.png", "none", false, "rgb"));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->founded()) {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/免费收集.png", "none", true, "rgb"));
            clicker->click(start_until, click_until, run_until);
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/参谋抽奖/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/参谋抽奖/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void mortar() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/火炮抽奖/远程部队.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/火炮抽奖/远程火炮.png"));
    clicker->clickIfFound(start_until, click_until, run_until, similarity_selector, 0, 1);

    for (const auto &i: std::vector<std::string>{"绿"}) { //TODO 蓝、紫色火炮派遣
        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/火炮抽奖/" + i + "色免费派遣.png", 0, 0.9, 60, "rgb");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/火炮抽奖/派遣.png", "top_center"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/火炮抽奖/确定.png"));
            clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/火炮抽奖/确定.png", "inner", true));
            clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/火炮抽奖/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/火炮抽奖/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);

}

void equipment() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/配件抽奖/配件中心.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/配件中心标题.png"));
    clicker->click(start_until, click_until, run_until, similarity_selector);

    clicker = std::make_unique<ImageClicker>("/res/配件抽奖/野外.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/野外场景.png"));
    run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/免费探索.png"));
    clicker = clicker->click(start_until, click_until, run_until);


    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/次数用尽.png"));
    clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/次数用尽.png", "inner", "true"));
    clicker->click(start_until, click_until, run_until);

    for (const auto &i: std::vector<std::string>{"工厂", "实验室"}) {
        clicker = std::make_unique<ImageClicker>("/res/配件抽奖/" + i + ".png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilIfImage>("/res/配件抽奖/免费探索次数.png"));
        clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

        if (!clicker->founded())continue;

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/免费探索.png", "top"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/免费探索.png", "inner", "true"));
        clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);
    }

    clicker = std::make_unique<ImageClicker>("/res/配件抽奖/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/配件抽奖/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void arms() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/军备抽奖/军备研究.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备抽奖/军备研究提示.png"));
    clicker->click(start_until, click_until, run_until, similarity_selector);

    for (const auto &i: std::vector<std::string>{"机械", "自动化"}) {
        clicker = std::make_unique<ImageClicker>("/res/军备抽奖/" + i + "制造厂.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/军备抽奖/" + i + "制造厂场景.png"));
        clicker->click(start_until, click_until, run_until, similarity_selector, 0, 0, 0, 0, "left");

        while (!state.stopFlag.load()) {
            clicker = std::make_unique<ImageClicker>("/res/军备抽奖/免费次数.png");

            if (!clicker->founded())break;

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("/res/军备抽奖/免费制造.png", "top_center"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);
        }
    }

    clicker = std::make_unique<ImageClicker>("/res/军备抽奖/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/军备抽奖/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void guild_building() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;
    clicker = std::make_unique<ImageClicker>(
            std::vector<std::string>{"/res/公会建筑/公会建筑小.png", "/res/公会建筑/公会建筑大.png"});

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/res/公会建筑/展开按钮.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/场景缩放.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until, similarity_selector, 0, 0, 15);

        clicker->clickIfFound(start_until, run_until, click_until, similarity_selector, 0, 1);

        clicker = std::make_unique<ImageClicker>("/res/公会建筑/公会建筑小.png");

        if (clicker->targetSegmentList.empty()) {
            emit Emitter::instance()->log("无法找到公会建筑!", "red");
            return;
        }
    }


    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/打开公会建筑.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/公会建筑标题.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/团体任务.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/参与任务.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clicker->click();

    auto iterator = CV::find_positions(CV::get_screen(), "/res/公会建筑/需要人数.png");

    auto it = iterator.begin();

    while (it != iterator.end() and !state.stopFlag.load()) {
        it->click();

        clicker = std::make_unique<ImageClicker>("/res/公会建筑/领取奖励.png", 0, 0.9, 60, "rgb");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/确定领取.png"));
        clicker = clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/确定领取.png", "inner", true));
        clicker->clickIfFound(start_until, click_until, run_until);

        ++it;
    }

    clicker = std::make_unique<ImageClicker>("/res/公会建筑/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/关闭窗口.png", "inner", "true"));
    clicker->click(start_until, click_until, run_until);
}
