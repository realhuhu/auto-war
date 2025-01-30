#ifndef QT_CLICKER_H
#define QT_CLICKER_H

#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <windows.h>

#include <QLabel>
#include <QThread>
#include <QObject>
#include <QDialog>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

# include "../state.h"

class ClickThread : public QThread {
Q_OBJECT

public:
    explicit ClickThread(const std::vector<POINT> &points, HWND hwnd, int interval = 100, QObject *parent = nullptr);

    void stop();

protected:
    void run() override;

private:
    HWND hwnd;
    int interval;
    std::vector<POINT> pointList;
    std::atomic<bool> stopFlag{false};
};

class ClickerDialog : public QDialog {
Q_OBJECT

public:
    explicit ClickerDialog(QWidget *parent = nullptr);

private slots:

    void appendCoordinate(int x, int y);

    void startRecord();

    void endRecord();

    void startClick();

    void endClick();

private:
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    static ClickerDialog *instance;
    HHOOK hook = nullptr;
    bool isWaiting = false;
    QTextEdit *textEdit;
    QSpinBox *intervalSpinBox;
    std::vector<POINT> pointList;
    ClickThread *clickThread = nullptr;

    void closeEvent(QCloseEvent *) override;
};

#endif //QT_CLICKER_H
