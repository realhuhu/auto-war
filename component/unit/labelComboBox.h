#ifndef QT_LABELEDCOMBOBOX_H
#define QT_LABELEDCOMBOBOX_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QHBoxLayout>

class LabelComboBox : public QWidget {
Q_OBJECT
public:
    QLabel *label;
    QComboBox *comboBox;

    explicit LabelComboBox(const QString &text, const QJsonArray &options, const QString &initialValue = "");
};

#endif //QT_LABELEDCOMBOBOX_H
