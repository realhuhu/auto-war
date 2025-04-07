# include "action.h"

QReadWriteLock dragLock;

void Mouse::moveTo(HWND hwnd, int x, int y) {
    QReadLocker locker(&dragLock);
    LPARAM lparam = (y << 16) | x;
    PostMessageW(hwnd, WM_MOUSEMOVE, MK_LBUTTON, lparam);
}


void Mouse::leftDown(HWND hwnd, int x, int y) {
    QReadLocker locker(&dragLock);
    LPARAM lparam = (y << 16) | x;
    PostMessageW(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lparam);
}


void Mouse::leftUp(HWND hwnd, int x, int y) {
    QReadLocker locker(&dragLock);
    LPARAM lparam = (y << 16) | x;
    PostMessageW(hwnd, WM_LBUTTONUP, MK_LBUTTON, lparam);
}

void Mouse::drag(HWND hwnd, int x_start, int y_start, int x_end, int y_end) {
    QWriteLocker locker(&dragLock);
    LPARAM lparam_start = (y_start << 16) | x_start;
    LPARAM lparam_end = (y_end << 16) | x_end;

    PostMessageW(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lparam_start);
    std::this_thread::sleep_for(std::chrono::duration<float>(0.1));

    PostMessageW(hwnd, WM_MOUSEMOVE, MK_LBUTTON, lparam_end);
    std::this_thread::sleep_for(std::chrono::duration<float>(0.1));

    PostMessageW(hwnd, WM_LBUTTONUP, MK_LBUTTON, lparam_end);
    std::this_thread::sleep_for(std::chrono::duration<float>(0.1));
}
