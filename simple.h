#ifndef QT_SIMPLE_H
#define QT_SIMPLE_H

#include "widget/panelWidget.h"

class AutoWarSimple : public PanelWidget {
Q_OBJECT
public:
    explicit AutoWarSimple(QWidget *parent = nullptr);

private slots:

    void startCapture();

    [[maybe_unused]] [[maybe_unused]] void getHwnd(int x, int y);

private:
    static AutoWarSimple *instance;
    HHOOK hook = nullptr;
    bool isWaiting = false;

    void closeEvent(QCloseEvent *event) override;

    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif //QT_SIMPLE_H
