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
#include <QDesktopServices>

#include "../util/state.h"
#include "../util/emitter.h"
#include "../task/daily.h"
#include "../task/battle.h"
#include "../task/guildWar.h"
#include "labeledSpinBox.h"
#include "labeledComboBox.h"
#include "buttonWithSetting.h"
#include "../function/imageDialog.h"
#include "../function/clickerDialog.h"
#include "../function/replaceDialog.h"
#include "../function/activityDialog.h"

class PanelWidget : public QWidget {
Q_OBJECT
public:
    bool autoHide = true;
    QPushButton *stopButton = new QPushButton("停止命令");
    QPushButton *clearButton = new QPushButton("清空输出");
    QTextEdit *outputText = new QTextEdit();

    explicit PanelWidget(QWidget *parent = nullptr);

signals:

    void logMessage(const QString &message, const QString &color = "black");

public slots:

    void log(const QString &text, const QString &color = "red") const;

    void onLogMessage(const QString &text, const QString &color = "black");

    void clearText() const;

    QVBoxLayout *createCommandLayout(int colWidth = 100);

    void runCommand(const QString &command);

    void batchRunCommand(const QString &command);

    void stopCommand() const;

    void setCommand(const QString &command);

    void closeEvent(QCloseEvent *event) override;

private:
    QStringList commandBattle;
    QStringList commandDaily;
    QStringList commandSpecial;
    QMap<QString, std::function<void()>> tasks;

    QString previousLog;

};

#endif // MAINWINDOW_H