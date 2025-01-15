#include <windows.h>

#ifndef QT_STATE_H
#define QT_STATE_H

struct State {
    HWND hwnd = nullptr;
    QTextEdit *output_text = nullptr;
    QThread *currentThread = nullptr;
};

State state;

#endif //QT_STATE_H
