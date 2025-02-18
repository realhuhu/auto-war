#ifndef QT_STATE_H
#define QT_STATE_H

#include <atomic>
#include <windows.h>

#include <QThread>
#include <QJsonObject>
#include <QMutexLocker>

struct State {
    HWND hwnd = nullptr;
    float scale = 1;
    QJsonObject config;
    std::atomic<bool> stopFlag{false};
    QThread *currentThread = nullptr;
    QBasicMutex threadMutex;
    std::vector<std::string> errorList = {};
    QBasicMutex errorListMutex;
};

extern State state;

#endif // QT_STATE_H