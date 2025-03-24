#include "labelTabWidget.h"


LabelTabWidget::LabelTabWidget(QWidget *parent) : QTabWidget(parent) {}

int LabelTabWidget::addTabWithLabel(QWidget *page, const QString &label) {
    int index = addTab(page, label);
    tabBar()->setTabData(index, QVariant(label));
    return index;
}

bool LabelTabWidget::removeTabByLabel(const QString &label) {
    for (int i = 0; i < count(); ++i) {
        QVariant data = tabBar()->tabData(i);

        if (!data.isValid()) continue;
        if (data.toString() != label) continue;

        removeTab(i);
        return true;
    }
    return false;
}

void LabelTabWidget::paintEvent(QPaintEvent *event) {
    QTabWidget::paintEvent(event);
    if (count() != 0) return;

    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.drawText(rect(), Qt::AlignCenter, "点击[打开游戏]按钮创建游戏窗口");
}
