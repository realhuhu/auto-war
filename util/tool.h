#ifndef RED_TOOL_H
#define RED_TOOL_H

#include <QJsonArray>
#include <QJsonObject>

QJsonArray mergeCheckbox(QJsonArray defaultCheckboxArray, QJsonArray rawCheckboxArray);

QJsonArray mergeInput(QJsonArray defaultInputArray, QJsonArray rawInputArray);

QJsonArray mergeSelect(QJsonArray defaultSelectArray, QJsonArray rawSelectArray);

QJsonObject merge(QJsonObject defaultConfig, const QJsonObject &rawConfig);

#endif //RED_TOOL_H
