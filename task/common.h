#ifndef QT_COMMON_H
#define QT_COMMON_H

#include <map>
#include <QJsonObject>
#include <QJsonArray>

std::map<std::string, bool> parse(const QString &category, const QString &key, const QJsonObject &jsonObject) ;

#endif //QT_COMMON_H
