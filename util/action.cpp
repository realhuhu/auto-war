# include "action.h"

void Mouse::moveTo(HWND hwnd, int x, int y) {
    LPARAM lparam = (y << 16) | x;
    PostMessageW(hwnd, WM_MOUSEMOVE, MK_LBUTTON, lparam);
}


void Mouse::leftDown(HWND hwnd, int x, int y) {
    LPARAM lparam = (y << 16) | x;
    PostMessageW(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lparam);
}


void Mouse::leftUp(HWND hwnd, int x, int y) {
    LPARAM lparam = (y << 16) | x;
    PostMessageW(hwnd, WM_LBUTTONUP, MK_LBUTTON, lparam);
}