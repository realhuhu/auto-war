#ifndef RED_PLACEHOLDER_DELEGATE_H
#define RED_PLACEHOLDER_DELEGATE_H

#include <QPainter>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <utility>

class PlaceholderDelegate : public QStyledItemDelegate {
public:
    explicit PlaceholderDelegate(
            QString placeholder,
            QObject *parent = nullptr
    );

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
