#ifndef RED_ENV_H
#define RED_ENV_H

#include <windows.h>

#include <QString>
#include <QJsonObject>

#include "emitter.h"

struct Env {
    HWND hwnd;
    Emitter *emitter;
    std::atomic<bool> *stopFlag;

    int region;
    QString qqRemark;
    QString redRemark;
    QJsonObject setting;
};

class EnvBase {
public:
    static thread_local Env env;
};

#endif //RED_ENV_H
