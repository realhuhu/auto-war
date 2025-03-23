#include "placeholderDelegate.h"

void PlaceholderDelegate::paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
) const {
    QStyledItemDelegate::paint(painter, option, index);

    if (index.data().toString().isEmpty() && !(option.state & QStyle::State_Editing)) {
        painter->save();
        painter->setPen(Qt::gray);
        QRect rect = option.rect.adjusted(2, 0, 0, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, placeholder);
        painter->restore();
    }
}

QWidget *PlaceholderDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
) const {
    QLineEdit *editor = new QLineEdit(parent);
    editor->setPlaceholderText(placeholder);
    return editor;
}
