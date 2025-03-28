#ifndef QT_BUTTONWITHSETTING_H
#define QT_BUTTONWITHSETTING_H

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>

class ButtonWithSetting : public QWidget {
Q_OBJECT
public:
    QPushButton *textButton;
    QToolButton *settingButton;

    explicit ButtonWithSetting(
            const QString &text,
            bool showSettingButton = true,
            QWidget *parent = nullptr
    );
};


#endif //QT_BUTTONWITHSETTING_H
