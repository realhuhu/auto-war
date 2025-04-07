#include "presupposition.h"

void presupposition(std::map<std::string, bool> &setting) {
    if (setting["国家争霸"]) countryArena(env);
    if (setting["世界争霸"]) worldArena(env);
    if (setting["剿灭将领"]) exterminateEnemy(env);
    if (setting["军备合成"]) armsCompound(env);
    if (setting["国家战争"]) countryWar(env);

    if (setting["英雄中心"]) heroCenter(env);
    if (setting["战争学院"]) warCenter(env);
    if (setting["国家宝箱"]) countryChest(env);
    if (setting["将领抽奖"]) admiral(env);
    if (setting["参谋抽奖"]) adviser(env);
    if (setting["火炮抽奖"]) mortar(env);
    if (setting["配件抽奖"]) equipment(env);
    if (setting["军备抽奖"]) arms(env);
    if (setting["每日签到"]) signIn(env);
    if (setting["矿区争夺"]) oreField(env);
    if (setting["月卡领取"]) monthlyCard(env);
    if (setting["其它活动"]) otherActivity(env);
    if (setting["每日任务"]) dailyTask(env);
    if (setting["周任务"]) weeklyTask(env);
    if (setting["公会领奖"]) guild(env);
    if (setting["公会建筑"]) guildBuilding(env);
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
