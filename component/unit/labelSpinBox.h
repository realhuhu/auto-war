#ifndef QT_LABELEDSPINBOX_H
#define QT_LABELEDSPINBOX_H

#include <QLabel>
#include <QWidget>
#include <QString>
#include <QSpinBox>
#include <QHBoxLayout>

class LabelSpinBox : public QWidget {
Q_OBJECT
public:
    QLabel *label;
    QSpinBox *spinBox;

    explicit LabelSpinBox(const QString &text, int value, QWidget *parent = nullptr);
};

#endif //QT_LABELEDSPINBOX_H
