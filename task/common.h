#ifndef QT_COMMON_H
#define QT_COMMON_H

#include <map>

#include <QJsonArray>
#include <QJsonObject>

#include "../flow/until.h"

std::map<std::string, bool> parseBoolConfig(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
);

std::map<std::string, int> parseIntConfig(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
);

std::map<std::string, std::string> parseStringConfig(
        const QString &category,
        const QString &key,
        const QJsonObject &jsonObject
);

void clearUntil(
        std::vector<std::unique_ptr<Until>> &startUntil,
        std::vector<std::unique_ptr<Until>> &clickUntil,
        std::vector<std::unique_ptr<Until>> &runUntil
);

#endif //QT_COMMON_H
