#include "widTabWidget.h"


WIdTabWidget::WIdTabWidget(QWidget *parent) : QTabWidget(parent) {}

int WIdTabWidget::addTabWithWId(QWidget *page, const QString &label, WId wid) {
    int index = addTab(page, label);
    tabBar()->setTabData(index, QVariant(static_cast<quint64>(wid)));
    return index;
}

bool WIdTabWidget::removeTabByWId(WId wid) {
    auto targetWId = static_cast<quint64>(wid);
    for (int i = 0; i < count(); ++i) {
        QVariant data = tabBar()->tabData(i);

        if (!data.isValid()) continue;
        if (data.toULongLong() != targetWId) continue;

        removeTab(i);
        return true;
    }
    return false;
}

void WIdTabWidget::paintEvent(QPaintEvent *event) {
    QTabWidget::paintEvent(event);
    if (count() != 0) return;

    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.drawText(rect(), Qt::AlignCenter, "点击[打开游戏]按钮创建游戏窗口");
}
