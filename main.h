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
#include "component/panel/redBrowser.h"
#include "component/panel/qqManger.h"
#include "component/panel/redManager.h"
#include "component/panel/controller.h"
#include "component/unit/labelTabWidget.h"


class AutoWar : public QWidget {
Q_OBJECT

public:
    QMap<QString, RedBrowser *> browsers;
    QTextEdit *logTextEdit = new QTextEdit(this);
    LabelTabWidget *panelTabWidget = new LabelTabWidget(this);

    explicit AutoWar(QWidget *parent = nullptr);

    void log(const QString &text, const QString &color = "blue") const;

    void loadConfig() const;

    void closeEvent(QCloseEvent *event) override;

public slots:

    void openBrowser();

    void openQQManager();

    void openRedManager();

    void saveConfig() const;

    void clearLog() const;
};

#endif //RED_MAIN_H
