#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>

#include <QMap>
#include <QWidget>
#include <QDialog>
#include <QThread>
#include <QScreen>
#include <QGroupBox>
#include <QCheckBox>
#include <QDateTime>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>

#include "util/state.h"
#include "util/emitter.h"
#include "task/daily.h"
#include "task/battle.h"
#include "widget/labeledSpinBox.h"
#include "widget/labeledComboBox.h"
#include "widget/buttonWithSetting.h"
#include "function/imageDialog.h"
#include "function/clickerDialog.h"

class MainWindow : public QWidget {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

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

    void batchRunCommand(const QString &command);

    void closeEvent(QCloseEvent *event) override;

private:
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    HHOOK hook;
    bool isWaiting;

    QStringList commandBattle;
    QStringList commandDaily;
    QMap<QString, std::function<void()>> tasks;

    QString previousLog;
    QTextEdit *outputText;
};

#endif // MAINWINDOW_H