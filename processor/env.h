#ifndef RED_ENV_H
#define RED_ENV_H

#include <windows.h>

#include <QHash>
#include <QString>
#include <QJsonObject>

#include "../util/emitter.h"

struct Env {
    HWND hwnd{};
    Emitter *emitter{};
    std::atomic<bool> *stopFlag{};

    int region{};
    QString qqRemark{};
    QString redRemark{};

    QHash<QString, bool> logFlag{};
    QHash<QString, bool> context{};
};

extern thread_local Env env;

#endif //RED_ENV_H
