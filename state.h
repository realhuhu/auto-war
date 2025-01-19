// state.h
#ifndef QT_STATE_H
#define QT_STATE_H

#include <windows.h>
#include <QThread>
#include <atomic>
#include <QJsonObject>

struct State {
    HWND hwnd = nullptr;
    float scale = 1;
    QThread *currentThread = nullptr;
    std::atomic<bool> stopFlag{false};
    QJsonObject config;
};

extern State state;

#endif // QT_STATE_H