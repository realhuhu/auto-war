#ifndef RED_RED_CONTROLLER_H
#define RED_RED_CONTROLLER_H

#include <QLabel>
#include <QWidget>
#include <QDateTime>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "cmdSelector.h"

class RedController : public QWidget {
Q_OBJECT

public:
    QString remark;
    QTextEdit *logTextEdit;

    explicit RedController(QString redRemark, QWidget *parent = nullptr);

    void log(const QString &text, const QString &color = "black") const;

public slots:

    void onLog(const QString &text, const QString &color = "black") const;

    void onRefresh();

    void onRun();

    void onStop();

    void onClear() const;

    void onTaskCreated(const QString &command, std::function<void(Env &env)> task);

signals:

    void toRefreshBrowser();

    void toRunTask(std::function<void(Env &env)> task);

    void toStopTask();
};


#endif //RED_RED_CONTROLLER_H
