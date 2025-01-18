#include "../state.h"
#include "../flow/cv.h"
#include "../flow/runner.h"
#include "../flow/emitter.h"

void guild_building_task() {
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

        clicker = std::make_unique<ImageClicker>("/res/公会建筑/领取奖励.png");

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/确定领取.png"));
        clicker->clickIfFound(start_until, click_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/公会建筑/确定领取.png", "inner", true));
        clicker->clickIfFound(start_until, click_until, run_until);

        ++it;
    }
}

void daily_task() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clicker = std::make_unique<ImageClicker>("/res/免费抽奖/英雄.png");

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilImage>("/res/免费抽奖/英雄中心.png"));
    clicker->clickIfFound(start_until, click_until, run_until);

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/res/免费抽奖/英雄免费.png");

        if (!clicker->founded()) {
            break;
        }

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("/res/免费抽奖/英雄抽奖.png", "top_center"));
        clicker = clicker->locate(start_until, run_until);

        clicker->click();

        break;
    }
}