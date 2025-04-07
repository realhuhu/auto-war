#ifndef RED_ACTION_H
#define RED_ACTION_H

#include <chrono>
#include <thread>
#include <windows.h>

#include <QReadWriteLock>

class Mouse {
public:
    static void moveTo(HWND hwnd, int x, int y);

    static void leftDown(HWND hwnd, int x, int y);

    static void leftUp(HWND hwnd, int x, int y);

    static void drag(HWND hwnd, int x_start, int y_start, int x_end, int y_end);
};

#endif //RED_ACTION_H
