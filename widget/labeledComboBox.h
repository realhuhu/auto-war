#ifndef QT_LABELEDCOMBOBOX_H
#define QT_LABELEDCOMBOBOX_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QHBoxLayout>

class LabeledComboBox : public QWidget {
Q_OBJECT
public:
    explicit LabeledComboBox(const QString &text, const QJsonArray &options, const QString &initialValue = "");

    QComboBox *getComboBox();

private:
    QLabel *label;
    QComboBox *comboBox;
};

#endif //QT_LABELEDCOMBOBOX_H
