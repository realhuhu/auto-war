#include "daily.h"

void heroCenter(const Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = parseBoolSetting("英雄中心", "checkbox", env.setting);

    clicker = std::make_unique<Clicker>("英雄中心/英雄.png")->click(
            {.runUntilList={new Image("英雄中心/英雄中心.png")}}
    );

    if (setting["免费抽奖"]) {
        while (!env.stopFlag->load()) {
            clicker = clicker->locate({.finishUntilList={new Image("英雄中心/英雄免费.png")}});

            if (!clicker->founded()) break;

            clicker = clicker->locate(
                    {.finishUntilList={new Image("英雄中心/英雄抽奖.png", {.onPrevious=Previous::TOP_CENTER})}}
            )->click(
                    {.finishUntilList={new IfImage("英雄中心/确定.png", {.finishWait=1})}}
            );

            if (!clicker->founded()) continue;

            clicker->click({.finishUntilList={new Image("英雄中心/确定.png", InnerReverse)}})->end();
        }
    }

    if (setting["英雄训练"]) {
        clicker = clicker->locate(
                {.finishUntilList={new Image("英雄中心/英雄培养.png")}}
        )->click(
                {.finishUntilList={new Image("英雄中心/英雄列表.png")}}
        );

        while (!env.stopFlag->load()) {
            clicker = clicker->locate({.finishUntilList={new IfImage("英雄中心/选择英雄.png")}});

            if (!clicker->founded()) break;

            clicker = clicker->click(
                    {.runUntilList={new AnyImage({"英雄中心/星星亮.png", "英雄中心/星星暗.png"})}}
            )->click(
                    {.selector=positionSelector("yCenter", "min"), .finishUntilList={new Image("英雄中心/开始训练.png")}}
            )->click(
                    {.finishUntilList={new Image("英雄中心/英雄列表.png"), new IfImage("英雄中心/确定.png")}, .finishWait=1}
            );

            if (!clicker->founded()) continue;

            clicker = clicker->click({.finishUntilList={new Image("英雄中心/确定.png", InnerReverse)}});
        }
    }

    clicker->locate(
            {.finishUntilList={new Image("英雄中心/关闭窗口.png")}}
    )->click(
            {.finishUntilList={new Image("英雄中心/关闭窗口.png", InnerReverse)}}
    )->end();
}