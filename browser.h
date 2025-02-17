#ifndef QT_BROWSER_H
#define QT_BROWSER_H

#include <QWebEngineView>
#include <QWebEngineSettings>

#include "widget/panelWidget.h"

class BrowserWindow : public QWidget {
Q_OBJECT
public:
    explicit BrowserWindow(QWidget *parent = nullptr);

private:
    PanelWidget *panel;
};

#endif //QT_BROWSER_H
