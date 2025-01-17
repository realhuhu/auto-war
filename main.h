#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QThread>
#include <QApplication>
#include <QScreen>
#include <QDateTime>
#include <QHBoxLayout>
#include <QGridLayout>
#include <Windows.h>
#include <QMap>
#include <functional>

#include "state.h"
#include "task/task.h"

class MainWindow : public QWidget {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override = default;

signals:

    void logMessage(const QString &message, const QString &color = "black");

    void logText(const QString &message, const QString &color = "red");

private slots:

    void onLogText(const QString &text, const QString &color = "red") const;

    void onLogMessage(const QString &text, const QString &color = "black") const;

    void start_hwnd_capture();

    void run_command(const QString &command);

    void set_command(const QString &command);

    void stop_command();

    void select_command();

    void clear_text();


    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    QStringList commands;
    QMap<QString, std::function<void()>> command_options;
    bool isWaitingForHwnd;
    HHOOK hook;
    QTextEdit *output_text;

    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H