#ifndef QT_SIMPLE_H
#define QT_SIMPLE_H

#include "widget/panelWidget.h"

class AutoWarSimple : public PanelWidget {
Q_OBJECT
public:
    explicit AutoWarSimple(QWidget *parent = nullptr);

private slots:

    void startCapture();

private:
    HHOOK hook = nullptr;
    bool isWaiting = false;


    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    void closeEvent(QCloseEvent *event) override;
};

#endif //QT_SIMPLE_H
