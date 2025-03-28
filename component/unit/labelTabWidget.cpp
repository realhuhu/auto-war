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

    const QStringList lines = {
            "1. 点击[QQ账号]登录QQ",
            "2. 点击[红警账号]配置红警账号",
            "3. 点击[打开游戏]多开游戏窗口"
    };

    QFontMetrics fm = painter.fontMetrics();

    int maxNumberWidth = 0;
    for (const QString &line: lines) {
        int dotPos = line.indexOf('.');
        QString numberPart = line.left(dotPos + 1);
        maxNumberWidth = qMax(maxNumberWidth, fm.horizontalAdvance(numberPart));
    }

    int textWidth = 0;
    int textHeight = fm.lineSpacing() * lines.count();
    for (const QString &line: lines) {
        int dotPos = line.indexOf('.');
        QString content = line.mid(dotPos + 1);
        textWidth = qMax(textWidth, maxNumberWidth + fm.horizontalAdvance(content));
    }

    QRect centerRect = rect();
    int startX = centerRect.center().x() - textWidth / 2;
    int startY = centerRect.center().y() - textHeight / 2;

    int yPos = startY + fm.ascent();
    for (const QString &line: lines) {
        int dotPos = line.indexOf('.');
        QString numberPart = line.left(dotPos + 1);
        QString content = line.mid(dotPos + 1);

        painter.drawText(startX, yPos, numberPart);
        painter.drawText(startX + maxNumberWidth, yPos, content);
        yPos += fm.lineSpacing();
    }
}
