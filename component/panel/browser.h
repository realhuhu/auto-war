#ifndef RED_BROWSER_H
#define RED_BROWSER_H

#include <QDialog>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "controller.h"
#include "../../util/tool.h"


class AutoWarBrowser : public QDialog {
Q_OBJECT

public:
    ControlPanel *panel;
    QWebEngineView *browser;

    explicit AutoWarBrowser(QTabWidget *tabWidget);

    void closeEvent(QCloseEvent *event) override;

    ~AutoWarBrowser() override;

public slots:

    void refresh() const;

signals:

    void closed(WId wid);
};

#endif //RED_BROWSER_H
