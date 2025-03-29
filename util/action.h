#ifndef RED_ACTION_H
#define RED_ACTION_H

#include <windows.h>

class Mouse {
public:
    static void moveTo(HWND hwnd, int x, int y);

    static void leftDown(HWND hwnd, int x, int y);

    static void leftUp(HWND hwnd, int x, int y);
};

#endif //RED_ACTION_H
