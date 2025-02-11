#ifndef QT_ACTION_H
#define QT_ACTION_H

#include <windows.h>

class Mouse {
public:
    static void moveTo(HWND hwnd, int x, int y);

    static void leftDown(HWND hwnd, int x, int y);

    static void leftUp(HWND hwnd, int x, int y);
};

#endif //QT_ACTION_H
