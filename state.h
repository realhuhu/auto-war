// state.h
#ifndef QT_STATE_H
#define QT_STATE_H

#include <windows.h>
#include <QThread>
#include <atomic>

struct State {
    HWND hwnd = nullptr;
    QThread *currentThread = nullptr;
    std::atomic<bool> stopFlag{false};
};

extern State state;

#endif // QT_STATE_H