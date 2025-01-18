#include "../state.h"
#include "../flow/cv.h"
#include "../flow/runner.h"
#include "../flow/emitter.h"


void hero_center() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/英雄中心/英雄.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/英雄中心.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/res/英雄中心/英雄免费.png");

        if (!clicker->founded())break;

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/英雄抽奖.png", "top_center"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilIfImage>("/res/英雄中心/确定.png"));
        clicker = clicker->click(start_until, click_until, run_until, similarity_selector, 0, 1);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilIfImage>("/res/英雄中心/确定.png", "inner", true));
        clicker->clickIfFound(start_until, click_until, run_until);
    }

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
        clicker->click(start_until, click_until, run_until);
    }

    clicker = std::make_unique<ImageClicker>("/res/英雄中心/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/英雄中心/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void war_center() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/战争学院/战争学院.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/战争学院标题.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/训练.png"));
    clicker = clicker->locate(start_until, run_until);

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/次数不足.png"));
    clicker = clicker->click(start_until, click_until, run_until, random_selector, 0.5);


    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/关闭窗口.png"));
    clicker = clicker->locate(start_until, run_until, position_selector("x_center", "min"));

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/关闭窗口.png", "inner", true));
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/军事演习.png"));
    clicker = clicker->click(start_until, click_until, run_until);

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

    clicker = std::make_unique<ImageClicker>("/res/战争学院/关闭窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/战争学院/关闭窗口.png", "inner", true));
    clicker->click(start_until, click_until, run_until);
}

void country_chest() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/国家宝箱/国家.png");

    clear_until(start_until, click_until, run_until);
    click_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/国家宝箱图标.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);


    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/月卡宝箱.png"));
    clicker = clicker->click(start_until, click_until, run_until);


    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/res/国家宝箱/确定没有月卡.png"
    }));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/国家宝箱/今日战功.png"));
    clicker->click(start_until, click_until, run_until);


    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/res/国家宝箱/战功进度宝箱.png", 0.5);

        if (!clicker->founded())break;

        clicker->click();
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

void guild() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/公会/参加公会战.png");

    if (clicker->founded()) {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/领取奖励.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/领取奖励.png", "right"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/确定领取公会战奖励.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/报名公会战.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/确定报名.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/关闭公会战窗口.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/公会.png"));
        clicker->click(start_until, click_until, run_until);
    }

    clicker = std::make_unique<ImageClicker>("/res/公会/公会.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/活跃任务.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/活跃任务按钮.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/今日公会活跃.png"));
    clicker->click(start_until, click_until, run_until);

    for (const auto &i: std::vector<std::string>{"第一档", "第二档", "第三档", "第四档", "第五档"}) {
        clicker = std::make_unique<ImageClicker>("/res/公会/" + i + ".png");

        if (!clicker->founded()) {
            continue;
        }

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/今日公会活跃.png"));
        clicker = clicker->click(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilIfAnyImage>(std::initializer_list<const std::string>{
                "/res/公会/确定领取活跃奖励.png", "/res/公会/确定任务数不够.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (!clicker->founded()) {
            continue;
        }

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/确定领取活跃奖励.png", "none", true));
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/确定任务数不够.png", "none", true));
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/关闭公会捐献窗口.png"));
        clicker->click(start_until, click_until, run_until);
    }

    clicker = std::make_unique<ImageClicker>("/res/公会/关闭公会捐献窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/公会争霸.png"));
    clicker = clicker->click(start_until, click_until, run_until);

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "/res/公会/夺城战已领取.png"
    }));
    clicker = clicker->click(start_until, click_until, run_until);

    if (clicker->templatePath == "/res/公会/夺城战未领取.png") {

    }

    clicker = std::make_unique<ImageClicker>("/res/公会/关闭公会争霸窗口.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/公会/关闭公会争霸窗口.png", "inner", true));
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
