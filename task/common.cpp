#include "common.h"

std::map<std::string, bool> parseBoolConfig(const QString &category, const QString &key, const QJsonObject &jsonObject) {
    std::map<std::string, bool> resultMap;
    QJsonArray checkboxArray = jsonObject[category][key].toArray();

    for (const auto &checkboxObj: checkboxArray) {
        QJsonObject checkbox = checkboxObj.toObject();
        QString text = checkbox["text"].toString();
        bool value = checkbox["value"].toBool();
        resultMap[text.toStdString()] = value;
    }

    return resultMap;
}

std::map<std::string, int> parseIntConfig(const QString &category, const QString &key, const QJsonObject &jsonObject) {
    std::map<std::string, int> resultMap;
    QJsonArray checkboxArray = jsonObject[category].toObject()[key].toArray();

    for (const auto &checkboxObj: checkboxArray) {
        QJsonObject checkbox = checkboxObj.toObject();
        QString text = checkbox["text"].toString();
        resultMap[text.toStdString()] = checkbox["value"].toInt();
    }

    return resultMap;
}