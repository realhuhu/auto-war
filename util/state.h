#ifndef QT_STATE_H
#define QT_STATE_H

#include <atomic>
#include <windows.h>

#include <QThread>
#include <QJsonObject>

struct State {
    HWND hwnd = nullptr;
    float scale = 1;
    QThread *currentThread = nullptr;
    std::atomic<bool> stopFlag{false};
    std::vector<std::string> errorList = {};
    QJsonObject config;
};

extern State state;

#endif // QT_STATE_H