#ifndef RED_MAIN_H
#define RED_MAIN_H

#include <QWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>
#include <QStyleFactory>
#include <QJsonDocument>
#include <QPlainTextEdit>
#include <QListWidgetItem>
#include <QWebEngineUrlScheme>

#include "util/tool.h"
#include "util/state.h"
#include "util/emitter.h"
#include "component/panel/qqManger.h"
#include "component/panel/redManager.h"
#include "component/panel/cmdSelector.h"
#include "component/panel/redController.h"
#include "component/unit/labelTabWidget.h"
#include "component/panel/redBrowser.h"


class AutoWar : public QWidget {
Q_OBJECT

public:
    QTextEdit *consoleTextEdit;
    LabelTabWidget *panelTabWidget;
    QMap<QString, RedBrowser *> browsers;

    explicit AutoWar(QWidget *parent = nullptr);

    void consolePrint(const QString &text, const QString &color = "blue") const;

    void loadConfig() const;

    void closeEvent(QCloseEvent *event) override;

public slots:

    void onOpenAllBrowser();

    void onOpenQQManager();

    void onOpenRedManager();

    void onOpenCmdSelector();

    void onClearConsole() const;

    void onBrowserClosed(const QString &remark);

    void onTaskCreated(const std::function<void(Env &env)>& task) const;

    void onConfigChanged() const;

};

#endif //RED_MAIN_H
