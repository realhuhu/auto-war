#ifndef QT_TASK_H
#define QT_TASK_H
#include "runner.h"

void clear_until(
        std::vector<std::unique_ptr<Until>> &start_until,
        std::vector<std::unique_ptr<Until>> &click_until,
        std::vector<std::unique_ptr<Until>> &run_until
) {
    start_until.clear();
    click_until.clear();
    run_until.clear();
}
//F:/UIBot/qt/F:/UIBot/qt/res
void exterminate_enemy() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> start_until;
    std::vector<std::unique_ptr<Until>> click_until;
    std::vector<std::unique_ptr<Until>> run_until;

    clear_until(start_until, click_until, run_until);
    run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
            "F:/UIBot/qt/res/剿灭将领/剿灭将领标题.png", "F:/UIBot/qt/res/剿灭将领/等级不足.png"
    }));
    clicker = std::make_unique<ImageClicker>(ImageClicker("F:/UIBot/qt/res/剿灭将领/剿灭将领.png"));
    clicker = clicker->clickIfFound(start_until, click_until, run_until);

    if (clicker->templatePath == "F:/UIBot/qt/res/剿灭将领/等级不足.png") {
        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/确定.png", "down"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/确定.png", "inner", true));
        clicker->click(start_until, click_until, run_until);
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>(ImageClicker("F:/UIBot/qt/res/剿灭将领/信物商店.png"));

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/合成.png"));
        run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/战斗进行中.png", "none", true));

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "F:/UIBot/qt/res/剿灭将领/紫色元.png", "F:/UIBot/qt/res/剿灭将领/紫色帜.png", "F:/UIBot/qt/res/剿灭将领/紫色番.png",
                "F:/UIBot/qt/res/剿灭将领/蓝色元.png", "F:/UIBot/qt/res/剿灭将领/蓝色帜.png", "F:/UIBot/qt/res/剿灭将领/蓝色番.png",
                "F:/UIBot/qt/res/剿灭将领/绿色元.png", "F:/UIBot/qt/res/剿灭将领/绿色帜.png", "F:/UIBot/qt/res/剿灭将领/绿色番.png"
        }, "right", false, "rgb"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/进攻.png", "down_center"));
        clicker = clicker->locate(start_until, run_until);

        clear_until(start_until, click_until, run_until);
        click_until.emplace_back(std::make_unique<UntilAnyImage>(std::initializer_list<const std::string>{
                "F:/UIBot/qt/res/剿灭将领/确认胜利.png", "F:/UIBot/qt/res/剿灭将领/已集齐.png", "F:/UIBot/qt/res/剿灭将领/次数不足.png"
        }));
        clicker = clicker->click(start_until, click_until, run_until);

        if (clicker->templatePath == "F:/UIBot/qt/res/剿灭将领/已集齐.png" ||
            clicker->templatePath == "F:/UIBot/qt/res/剿灭将领/次数不足.png") {
            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/关闭窗口.png", "right"));
            clicker = clicker->locate(start_until, run_until);

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/合成.png"));
            clicker = clicker->click(start_until, click_until, run_until, position_selector("y_center", "max"));

            clear_until(start_until, click_until, run_until);
            run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/关闭窗口.png"));
            clicker = clicker->click(start_until, click_until, run_until);

            clicker->click();
            break;
        }


        clear_until(start_until, click_until, run_until);
        run_until.emplace_back(std::make_unique<UntilImage>("F:/UIBot/qt/res/剿灭将领/信物商店.png"));
        clicker->click(start_until, click_until, run_until);
    }
}
#endif //QT_TASK_H
