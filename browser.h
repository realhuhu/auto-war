#ifndef QT_BROWSER_H
#define QT_BROWSER_H

#include <QAction>
#include <QMessageBox>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "widget/panelWidget.h"

class AutoWarBrowser : public PanelWidget {
Q_OBJECT
public:
    explicit AutoWarBrowser(QWidget *parent = nullptr);

    ~AutoWarBrowser() override;

private slots:

    void refresh();

private:
    QWebEngineView *browser;

    void closeEvent(QCloseEvent *event) override;
};

#endif //QT_BROWSER_H
