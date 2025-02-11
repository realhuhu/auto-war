#include "common.h"

std::map<std::string, bool> parseBoolConfig(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
) {
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

std::map<std::string, int> parseIntConfig(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
) {
    std::map<std::string, int> resultMap;
    QJsonArray spinBoxArray = jsonObject[category].toObject()[key].toArray();

    for (const auto &spinBoxObj: spinBoxArray) {
        QJsonObject spinBox = spinBoxObj.toObject();
        QString text = spinBox["text"].toString();
        int value = spinBox["value"].toInt();
        resultMap[text.toStdString()] = value;
    }

    return resultMap;
}

std::map<std::string, std::string> parseStringConfig(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
) {
    std::map<std::string, std::string> resultMap;
    QJsonArray comboBoxArray = jsonObject[category][key].toArray();

    for (const auto &comboBoxObj: comboBoxArray) {
        QJsonObject comboBox = comboBoxObj.toObject();
        QString text = comboBox["text"].toString();
        QString value = comboBox["value"].toString();
        resultMap[text.toStdString()] = value.toStdString();
    }

    return resultMap;
}

void clearUntil(
        std::vector<std::unique_ptr<Until>> &startUntil,
        std::vector<std::unique_ptr<Until>> &clickUntil,
        std::vector<std::unique_ptr<Until>> &runUntil
) {
    startUntil.clear();
    clickUntil.clear();
    runUntil.clear();
}