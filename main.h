#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Windows.h>
#include <functional>

#include <QMap>
#include <QWidget>
#include <QDialog>
#include <QThread>
#include <QScreen>
#include <QDateTime>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QApplication>

#include "state.h"
#include "task/battle.h"

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

    void onLogMessage(const QString &text, const QString &color = "black");

    void startCapture();

    void selectCommand();

    void stopCommand();

    void clearText();

    void setCommand(const QString &command);

    void runCommand(const QString &command);

    void closeEvent(QCloseEvent *event) override;

    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    QStringList commandBattle;
    QStringList commandDaily;
    QMap<QString, std::function<void()>> tasks;
    bool isWaiting;
    HHOOK hook;
    QTextEdit *outputText;
    QString previousLog;
};

#endif // MAINWINDOW_H