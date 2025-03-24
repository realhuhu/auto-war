#ifndef RED_WID_TAB_WIDGET_H
#define RED_WID_TAB_WIDGET_H

#include <QTabBar>
#include <QPainter>
#include <QVariant>
#include <QTabWidget>

class LabelTabWidget : public QTabWidget {
Q_OBJECT
public:
    explicit LabelTabWidget(QWidget *parent = nullptr);

    int addTabWithLabel(QWidget *page, const QString &label);

    bool removeTabByLabel(const QString &label);

    void paintEvent(QPaintEvent *event) override;
};

#endif //RED_WID_TAB_WIDGET_H
