#include "presupposition.h"

void presupposition(std::map<std::string, bool> &setting) {
    env.context["presupposition"] = true;

    if (setting["国家争霸"]) {
        countryArena(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["世界争霸"]) {
        worldArena(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["剿灭将领"]) {
        exterminateEnemy(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["军备合成"]) {
        armsCompound(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["国家战争"]) {
        countryWar(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["征战"]) {
        expedition(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["英雄中心"]) {
        heroCenter(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["战争学院"]) {
        warCenter(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["国家宝箱"]) {
        countryChest(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["将领抽奖"]) {
        admiral(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["参谋抽奖"]){
        adviser(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["火炮抽奖"]){
        mortar(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["配件抽奖"]) {
        equipment(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["军备抽奖"]) {
        arms(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["每日签到"]) {
        signIn(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["矿区争夺"]) {
        oreField(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["月卡领取"]) {
        monthlyCard(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["其它活动"]) {
        otherActivity(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["每日任务"]){
        dailyTask(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["周任务"]) {
        weeklyTask(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["公会领奖"]) {
        guild(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }


    if (setting["公会建筑"]){
        guildBuilding(env);
        Mouse::leftDown(env.hwnd, 0, 0);
        sleep(env.stopFlag, 0.5);
    }
}

void presupposition1(Env &e) {
    env = e;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("预设1", "checkbox", setting);

    presupposition(boolSetting);
}

void presupposition2(Env &e) {
    env = e;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("预设2", "checkbox", setting);

    presupposition(boolSetting);
}

void presupposition3(Env &e) {
    env = e;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("预设3", "checkbox", setting);

    presupposition(boolSetting);
}

void presupposition4(Env &e) {
    env = e;
    auto setting = loadSetting(state.config, env.qqRemark, env.redRemark, state.settingDefault);
    auto boolSetting = parseBoolSetting("预设4", "checkbox", setting);

    presupposition(boolSetting);
}
