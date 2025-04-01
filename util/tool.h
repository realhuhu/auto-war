#ifndef RED_TOOL_H
#define RED_TOOL_H

#include <chrono>
#include <thread>

#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>

QJsonArray mergeCheckbox(QJsonArray defaultCheckboxArray, QJsonArray rawCheckboxArray);

QJsonArray mergeInput(QJsonArray defaultInputArray, QJsonArray rawInputArray);

QJsonArray mergeSelect(QJsonArray defaultSelectArray, QJsonArray rawSelectArray);

QJsonObject mergeSetting(QJsonObject defaultSetting, const QJsonObject &rawSetting);

QJsonObject loadSetting(
        QJsonObject config,
        const QString &qqRemark,
        const QString &redRemark,
        QJsonObject defaultSetting
);

void sleep(std::atomic<bool> *stopFlag, float seconds);

QString res(const QString &name, const QString &dir = "游戏图片");

#endif //RED_TOOL_H
