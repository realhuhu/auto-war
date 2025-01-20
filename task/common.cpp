#include "common.h"

std::map<std::string, bool> parse(const QString &category, const QString &key, const QJsonObject &jsonObject) {
    std::map<std::string, bool> resultMap;
    QJsonArray checkboxArray = jsonObject[category][key].toArray();

    for (const auto &checkboxObj: checkboxArray) {
        QJsonObject checkbox = checkboxObj.toObject();
        QString text = checkbox["text"].toString();
        bool value = checkbox["value"].toBool();

        // 将 QString 转换为 std::string 并添加到 map 中
        resultMap[text.toStdString()] = value;
    }

    return resultMap;
}