#ifndef RED_COMMON_H
#define RED_COMMON_H

#include <map>

#include <QJsonArray>
#include <QJsonObject>

#include "../processor/runner.h"

std::map<std::string, bool> parseBoolSetting(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
);

std::map<std::string, int> parseIntSetting(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
);

std::map<std::string, std::string> parseStringSetting(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
);

extern UntilConfig InnerReverse;

#endif //RED_COMMON_H
