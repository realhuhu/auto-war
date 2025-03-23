#ifndef RED_MAIN_H
#define RED_MAIN_H

#include <QWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QApplication>
#include <QPlainTextEdit>

#include "util/tool.h"
#include "util/state.h"
#include "component/panel/qqManger.h"
#include "component/panel/browser.h"
#include "component/panel/controller.h"
#include "component/unit/widTabWidget.h"


class AutoWar : public QWidget {
Q_OBJECT

public:
    QMap<WId, AutoWarBrowser *> browsers;
    QTextEdit *logTextEdit = new QTextEdit(this);
    WIdTabWidget *panelTabWidget = new WIdTabWidget(this);

    explicit AutoWar(QWidget *parent = nullptr);

    void log(const QString &text, const QString &color = "blue") const;

    void loadConfig() const;

    void closeEvent(QCloseEvent *event) override;

public slots:

    void openBrowser();

    void openQQManager();

    void saveConfig() const;

    void clearLog() const;
};

#endif //RED_MAIN_H
