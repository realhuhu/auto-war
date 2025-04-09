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
#include "component/unit/labelTabWidget.h"
#include "component/panel/qqManger.h"
#include "component/panel/redWorker.h"
#include "component/panel/redManager.h"
#include "component/panel/cmdSelector.h"
#include "component/panel/funcSelector.h"
#include "component/panel/redController.h"


class AutoWar : public QWidget {
Q_OBJECT

public:
    QTextEdit *consoleTextEdit;
    LabelTabWidget *panelTabWidget;
    QMap<QString, RedWorker *> workers;

    explicit AutoWar(QWidget *parent = nullptr);

    void consolePrint(const QString &text, const QString &color = "blue") const;

    void loadConfig() const;

    void closeEvent(QCloseEvent *event) override;

public slots:

    void onOpenAllBrowser();

    void onOpenQQManager();

    void onOpenRedManager();

    void onOpenCmdSelector();

    void onBatchStop();

    void onBatchRefresh();

    void onShowOther();

    void onConsolePrint(const QString &text, const QString &color) const;

    void onClearConsole() const;

    void onBrowserClosed(const QString &remark);

    void onTaskCreated(const QString &command, const std::function<void(Env &env)> &task) const;

    void onConfigChanged() const;

};

#endif //RED_MAIN_H
