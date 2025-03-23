#ifndef RED_WIDTABWIDGET_H
#define RED_WIDTABWIDGET_H

#include <QTabBar>
#include <QPainter>
#include <QVariant>
#include <QTabWidget>

class WIdTabWidget : public QTabWidget {
Q_OBJECT
public:
    explicit WIdTabWidget(QWidget *parent = nullptr);

    int addTabWithWId(QWidget *page, const QString &label, WId wid);

    bool removeTabByWId(WId wid);

    void paintEvent(QPaintEvent *event) override;
};

#endif //RED_WIDTABWIDGET_H
