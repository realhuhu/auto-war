#include "expedition.h"


void closeAllConfirm() {
    while (!env.stopFlag->load()) {
        auto clicker = std::make_unique<Clicker>("征战/确定.png");

        if (!clicker->founded()) break;

        clicker->click()->end();
    }
}

void expedition(Env &e) {
    env = e;
    std::unique_ptr<Clicker> clicker;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("征战", "checkbox", setting);
    auto intSetting = parseIntSetting("征战", "input", setting);

    if (env.context["presupposition"] && !boolSetting["自动刷新"]) {
        emit env.emitter->error("[征战]提前结束: 未开启自动刷新，将不在一键预设中执行");
        return;
    }

    clicker = std::make_unique<Clicker>(
            "征战/征战图标.png"
    )->click(
            {.finishUntilList={new Image("征战/征战标题.png"), new IfAnyImage({"征战/次数已用完A.png", "征战/次数已用完B.png"})}}
    );

    if (clicker->founded()) {
        clicker->locate(
                {.finishUntilList={new Image("征战/关闭窗口.png")}}
        )->click(
                {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("征战/关闭窗口.png", InnerReverse)}}
        )->end();
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, env.pid);
    uintptr_t address = 0;
    try {
        auto scanner = new MemoryScanner(hProcess);
        auto startTime = std::chrono::steady_clock::now();
        auto res = scanner->Search("?? 00 00 00 96 00 00 00 01 00 00 00 ?? 00 00 00 01 00 00 00 01 00 00 00");
        auto endTime = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        emit env.emitter->log(QString("用时 %1 ms").arg(QString::number(ms.count())));

        if (res.empty()) {
            emit env.emitter->log("无结果");
        } else if (res.size() > 1) {
            emit env.emitter->log("结果不唯一");
        } else {
            address = res.front();
        }

    } catch (const std::exception &e) {
        emit env.emitter->error(QString("操作失败: %1").arg(QString::fromStdString(e.what())));
    }

    while (true) {
        clicker = std::make_unique<Clicker>("征战/自动战斗.png", ClickerInitConfig{.mode=Mode::RGB});

        if (!clicker->founded() && !boolSetting["自动刷新"]) {
            CloseHandle(hProcess);

            emit env.emitter->error("[征战]提前结束: 未开启自动刷新，已到关底，自动结束");

            return;
        }

        if (env.context["refresh"] && !boolSetting["自动刷新"]) {
            CloseHandle(hProcess);

            emit env.emitter->error("[征战]提前结束: 未开启自动刷新，已到关底，自动结束");

            return;
        }

        if (!clicker->founded() || env.context["refresh"]) {
            env.context["refresh"] = false;

            clicker = std::make_unique<Clicker>("征战/重新征战.png", ClickerInitConfig{.mode=Mode::RGB});

            if (!clicker->founded()) {
                clicker = std::make_unique<Clicker>("征战/特权征战.png", ClickerInitConfig{.mode=Mode::RGB});
                if (!boolSetting["VIP征战"] || !clicker->founded()) break;

                clicker->click(
                        {.finishUntilList={new Image("征战/确定.png")}}
                )->click(
                        {.finishUntilList={new Image("征战/确定.png", InnerReverse)}, .finishWait=2}
                )->end();

                continue;
            }

            clicker = clicker->click({.finishUntilList={new AnyImage({"征战/免费重置.png", "征战/勋章重置.png"})}});
            if (clicker->imgPath == "征战/免费重置.png") {
                clicker->locate(
                        {.finishUntilList={new Image("征战/确定.png")}}
                )->click(
                        {.finishUntilList={new Image("征战/确定.png", InnerReverse)}, .finishWait=2}
                )->end();

                continue;
            } else if (clicker->imgPath == "征战/勋章重置.png") {
                if (!boolSetting["勋章征战"]) {
                    clicker->locate(
                            {.finishUntilList={new Image("征战/关闭窗口.png")}}
                    )->click(
                            {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("征战/关闭窗口.png", InnerReverse)}}
                    )->end();

                    break;
                }

                clicker->locate(
                        {.finishUntilList={new Image("征战/确定.png")}}
                )->click(
                        {.finishUntilList={new Image("征战/确定.png", InnerReverse)}, .finishWait=2}
                )->end();

                continue;
            }
        } else {
            clicker->click(
                    {.finishUntilList={new Image("征战/确定战斗.png")}}
            )->click(
                    {.finishUntilList={new Image("征战/确定战斗.png", InnerReverse)}}
            )->end();

            if (address) {
                auto mem = new Memory(hProcess, address);
                while (!env.stopFlag->load() && mem->Read<int>(0x0C)) {
                    auto num = mem->Read<int>(0x30);
                    if (num >= intSetting["关卡数"])mem->Write<int>(0x0C, 0);

                    auto cd = mem->Read<int>(0x00);
                    if (cd > 1) mem->Write<int>(0x00, 1);

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                mem->Write<int>(0x00, 1);
            }

            clicker = std::make_unique<Clicker>(
                    std::vector<QString>{"征战/无法治疗.png", "征战/战斗失败.png", "征战/次数已用完A.png", "征战/次数已用完B.png", "征战/金属不足.png"},
                    ClickerInitConfig{.wait=2}
            );

            if (clicker->imgPath == "征战/无法治疗.png") {
                clicker->locate(
                        {.finishUntilList={new Image("征战/关闭窗口.png", {.onPrevious=Previous::RIGHT})}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("征战/关闭窗口.png", InnerReverse)}}
                )->end();

                closeAllConfirm();

                continue;
            } else if (clicker->imgPath == "征战/战斗失败.png" || clicker->imgPath == "征战/次数已用完A.png" || clicker->imgPath == "征战/次数已用完B.png") {
                clicker->locate(
                        {.finishUntilList={new Image("征战/关闭窗口.png", {.onPrevious=Previous::RIGHT})}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("征战/关闭窗口.png", InnerReverse)}}
                )->end();

                closeAllConfirm();

                env.context["refresh"] = true;

                continue;
            } else if (clicker->imgPath == "征战/金属不足.png") {
                clicker->locate(
                        {.finishUntilList={new Image("征战/关闭窗口.png", {.onPrevious=Previous::RIGHT})}}
                )->click(
                        {.selector=positionSelector("yCenter", "max"), .finishUntilList={new Image("征战/关闭窗口.png", InnerReverse)}}
                )->end();

                closeAllConfirm();

                emit env.emitter->error("[征战]提前结束: 资源不足，无法维修坦克");

                break;
            } else {
                env.context["refresh"] = true;

                closeAllConfirm();

                continue;
            }
        }
    }

    std::make_unique<Clicker>(
            "征战/关闭窗口.png"
    )->click(
            {.finishUntilList={new Image("征战/关闭窗口.png", InnerReverse)}}
    )->end();

    CloseHandle(hProcess);
}
