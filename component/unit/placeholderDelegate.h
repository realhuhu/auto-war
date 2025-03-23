#ifndef RED_PLACEHOLDER_DELEGATE_H
#define RED_PLACEHOLDER_DELEGATE_H

#include <QPainter>
#include <QLineEdit>
#include <QStyledItemDelegate>

class PlaceholderDelegate : public QStyledItemDelegate {
public:
    PlaceholderDelegate(
            const QString &placeholder,
            QObject *parent = nullptr
    ) : QStyledItemDelegate(parent), placeholder(placeholder) {}

    QWidget *createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &option,
            const QModelIndex &index
    ) const override;

    void paint(
            QPainter *painter,
            const QStyleOptionViewItem &option,
            const QModelIndex &index
    ) const override;

private:
    QString placeholder;
};

#endif //RED_PLACEHOLDER_DELEGATE_H
