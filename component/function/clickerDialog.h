#ifndef RED_CLICKER_DIALOG_H
#define RED_CLICKER_DIALOG_H


#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
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

#include "../panel/redBrowser.h"
# include "../../util/action.h"

class ClickThread : public QThread {
Q_OBJECT

public:
    explicit ClickThread(
            const std::vector<POINT> &points,
            HWND hwnd,
            int interval = 100,
            int rounds = 0,
            QObject *parent = nullptr
    );

    void stop();

signals:

    void logText(const QString &text);

protected:
    void run() override;

private:
    HWND hwnd;
    int interval;
    int rounds;
    std::vector<POINT> pointList;
    std::atomic<bool> stopFlag{false};
};

class ClickerDialog : public QDialog {
Q_OBJECT

public:
    explicit ClickerDialog(RedBrowser *redBrowser, QWidget *parent = nullptr);

private slots:

    void updateTextEdit(const QString &text);

    [[maybe_unused]] void appendCoordinate(int x, int y);

    void startRecord();

    void endRecord();

    void startClick();

    void endClick();

private:
    RedBrowser *browser;
    static ClickerDialog *instance;
    HHOOK hook = nullptr;
    bool isWaiting = false;
    QTextEdit *textEdit;
    QSpinBox *intervalSpinBox;
    QSpinBox *roundsSpinBox;
    std::vector<POINT> pointList;
    ClickThread *clickThread = nullptr;

    void closeEvent(QCloseEvent *) override;

    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif //RED_CLICKER_DIALOG_H
