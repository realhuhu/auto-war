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
    explicit LabelSpinBox(const QString &text, int value, QWidget *parent = nullptr);

    [[nodiscard]] QSpinBox *getSpinBox() const;

private:
    QLabel *label;
    QSpinBox *spinBox;
};

#endif //QT_LABELEDSPINBOX_H
